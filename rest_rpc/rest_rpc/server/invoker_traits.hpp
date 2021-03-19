#pragma once

namespace timax { namespace rpc 
{
	template <typename T, bool>
	struct element_type
	{
		using type = T;
	};

	template <typename T>
	struct element_type<T, true>
	{
		using type = typename T::element_type;
	};

	template <typename T>
	struct remove_all_wrapper_impl
	{
		using type = typename element_type<T, is_smart_pointer<T>::value>::type;
	};

	template <typename T>
	struct remove_all_wrapper : remove_all_wrapper_impl<
		std::remove_cv_t<std::remove_reference_t<T>>>
	{
	};

	template <typename ArgsTuple>
	struct is_first_arg_connection
	{
		static constexpr bool value = false;
		using type = ArgsTuple;
	};

	template <typename Arg0, typename ... Args>
	struct is_first_arg_connection<std::tuple<Arg0, Args...>>
	{
		static constexpr bool value = std::is_same<connection,
			typename remove_all_wrapper<Arg0>::type>::value;
		using type = std::conditional_t<value,
			std::tuple<Args...>, std::tuple<Arg0, Args...>>;
	};
} }

namespace timax { namespace rpc 
{
	struct handler_return_void {};
	struct handler_return_result {};

	struct handler_exec_sync {};
	struct handler_exec_async {};

	struct handler_with_conn {};
	struct handler_without_conn {};

	template <typename Handler>
	struct handler_traits
	{
	private:
		using function_traits_type = function_traits<std::remove_reference_t<Handler>>;
		using ifac_type = is_first_arg_connection<typename function_traits_type::tuple_type>;
	public:
		using return_tag = std::conditional_t<std::is_void<typename function_traits_type::result_type>::value,
			handler_return_void, handler_return_result>;
		using connection_tag = std::conditional_t<ifac_type::value,
			handler_with_conn, handler_without_conn>;
		using tuple_type = typename ifac_type::type;
	};

	template <typename Handler, typename ArgsTuple, size_t ... Is>
	inline auto invoker_call_handler_impl(handler_with_conn, Handler&& h, 
		std::shared_ptr<connection> conn, ArgsTuple&& args_tuple, std::index_sequence<Is...>)
	{
		return h(conn, std::forward<std::tuple_element_t<Is, std::remove_reference_t<ArgsTuple>>>(std::get<Is>(args_tuple))...);
	}

	template <typename Handler, typename ArgsTuple, size_t ... Is>
	inline auto invoker_call_handler_impl(handler_without_conn, Handler&& h, 
		std::shared_ptr<connection> conn, ArgsTuple&& args_tuple, std::index_sequence<Is...>)
	{
		using args_tuple_type = std::remove_reference_t<ArgsTuple>;
		return h(std::forward<std::tuple_element_t<Is, args_tuple_type>>(std::get<Is>(args_tuple))...);
	}

	template <typename Handler, typename ArgsTuple>
	inline auto invoker_call_handler(Handler&& h, std::shared_ptr<connection> conn, ArgsTuple&& args_tuple)
	{
		using handler_connection_tag = typename handler_traits<Handler>::connection_tag;
		using indices_type = std::make_index_sequence<std::tuple_size<std::remove_reference_t<ArgsTuple>>::value>;
		return invoker_call_handler_impl(handler_connection_tag{}, std::forward<Handler>(h), 
			conn, std::forward<ArgsTuple>(args_tuple), indices_type{});
	}

	template <typename ReturnTag, typename ExecuteTag>
	struct invoker_traits;

	// The return type of the handler is void;
	// The handler runs synchronously with io thread;
	template <>
	struct invoker_traits<handler_return_void, handler_exec_sync>
	{
		using invoker_t = std::function<void(std::shared_ptr<connection>, char const*, size_t)>;

		template <typename CodecPolicy, typename Handler>
		static auto get(Handler&& handler)
		{
			using args_tuple_type = typename handler_traits<Handler>::tuple_type;
			invoker_t invoker = [h = std::forward<Handler>(handler)]
				(std::shared_ptr<connection> conn, char const* data, size_t size)
			{
				CodecPolicy cp{};
				auto args_tuple = cp.template unpack<args_tuple_type>(data, size);
				invoker_call_handler(h, conn, args_tuple);
				auto ctx = context_t::make_message_with_head(conn->head_, context_t::message_t{});
				conn->response(ctx);
			};
			return invoker;
		}

		template <typename CodecPolicy, typename Handler, typename PostFunc>
		static auto get(Handler&& handler, PostFunc&& post_func)
		{
			using args_tuple_type = typename handler_traits<Handler>::tuple_type;
			invoker_t invoker = [h = std::forward<Handler>(handler), p = std::forward<PostFunc>(post_func)]
				(std::shared_ptr<connection> conn, char const* data, size_t size)
			{
				CodecPolicy cp{};
				auto args_tuple = cp.template unpack<args_tuple_type>(data, size);
				invoker_call_handler(h, conn, args_tuple);
				auto ctx = context_t::make_message_with_head(conn->head_, context_t::message_t{}, [conn, &p] { p(conn); });
				conn->response(ctx);
			};
			return invoker;
		}
	};

	// The return type of the handler is non void;
	// The handler runs synchronously with io thread;
	template <>
	struct invoker_traits<handler_return_result, handler_exec_sync>
	{
		using invoker_t = std::function<void(std::shared_ptr<connection>, char const*, size_t)>;

		template <typename CodecPolicy, typename Handler>
		static auto get(Handler&& handler)
		{
			using args_tuple_type = typename handler_traits<Handler>::tuple_type;
			invoker_t invoker = [h = std::forward<Handler>(handler)]
				(std::shared_ptr<connection> conn, char const* data, size_t size)
			{
				CodecPolicy cp{};
				auto args_tuple = cp.template unpack<args_tuple_type>(data, size);
				auto result = invoker_call_handler(h, conn, args_tuple);
				auto message = cp.pack(result);
				auto ctx = context_t::make_message_with_head(conn->head_, std::move(message));
				conn->response(ctx);
			};
			return invoker;
		}

		template <typename CodecPolicy, typename Handler, typename PostFunc>
		static auto get(Handler&& handler, PostFunc&& post_func)
		{
			using args_tuple_type = typename handler_traits<Handler>::tuple_type;
			invoker_t invoker = [h = std::forward<Handler>(handler), p = std::forward<PostFunc>(post_func)]
				(std::shared_ptr<connection> conn, char const* data, size_t size)
			{
				CodecPolicy cp{};
				auto args_tuple = cp.template unpack<args_tuple_type>(data, size);
				auto result = invoker_call_handler(h, conn, args_tuple);
				auto message = cp.pack(result);
				auto ctx = context_t::make_message_with_head(conn->head_, std::move(message),
					[conn, r = std::move(result), &p]
				{
					p(conn, r);
				});
				conn->response(ctx);
			};
			return invoker;
		}
	};

	// The return type of the handler is void;
	// The handler runs asynchronously with io thread;
	template <>
	struct invoker_traits<handler_return_void, handler_exec_async>
	{
		using invoker_t = std::function<void(std::shared_ptr<connection>, char const*, size_t)>;

		template <typename CodecPolicy, typename Handler>
		static auto get(Handler&& handler)
		{
			using args_tuple_type = typename handler_traits<Handler>::tuple_type;
			invoker_t invoker = [h = std::forward<Handler>(handler)]
				(std::shared_ptr<connection> conn, char const* data, size_t size)
			{
				CodecPolicy cp{};
				auto args_tuple = cp.template unpack<args_tuple_type>(data, size);
				std::async([conn, at = std::move(args_tuple), head = conn->head_, &h]
				{
					invoker_call_handler(h, conn, at);
					auto ctx = context_t::make_message_with_head(head, context_t::message_t{});
					conn->response(ctx);
				});
				
			};
			return invoker;
		}

		template <typename CodecPolicy, typename Handler, typename PostFunc>
		static auto get(Handler&& handler, PostFunc&& post_func)
		{
			using args_tuple_type = typename handler_traits<Handler>::tuple_type;
			invoker_t invoker = [h = std::forward<Handler>(handler), p = std::forward<PostFunc>(post_func)]
				(std::shared_ptr<connection> conn, char const* data, size_t size)
			{
				CodecPolicy cp{};
				auto args_tuple = cp.template unpack<args_tuple_type>(data, size);
				std::async([conn, at = std::move(args_tuple), head = conn->head_, &h, &p]
				{
					invoker_call_handler(h, conn, at);
					auto ctx = context_t::make_message_with_head(head, context_t::message_t{}, [conn, &p] { p(conn); });
					conn->response(ctx);
				});
				
			};
			return invoker;
		}
	};

	// The return type of the handler is non void;
	// The handler runs asynchronously with io thread;
	template <>
	struct invoker_traits<handler_return_result, handler_exec_async>
	{
		using invoker_t = std::function<void(std::shared_ptr<connection>, char const*, size_t)>;

		template <typename CodecPolicy, typename Handler>
		static auto get(Handler&& handler)
		{
			using args_tuple_type = typename handler_traits<Handler>::tuple_type;
			invoker_t invoker = [h = std::forward<Handler>(handler)]
				(std::shared_ptr<connection> conn, char const* data, size_t size)
			{
				CodecPolicy cp{};
				auto args_tuple = cp.template unpack<args_tuple_type>(data, size);
				std::async([conn, at = std::move(args_tuple), head = conn->head_, &h]
				{
					auto result = invoker_call_handler(h, conn, at);
					auto message = CodecPolicy{}.pack(result);
					auto ctx = context_t::make_message_with_head(head, std::move(message));
					conn->response(ctx);
				});
			};

			return invoker;
		}

		template <typename CodecPolicy, typename Handler, typename PostFunc>
		static auto get(Handler&& handler, PostFunc&& post_func)
		{
			using args_tuple_type = typename handler_traits<Handler>::tuple_type;
			invoker_t invoker = [h = std::forward<Handler>(handler), p = std::forward<PostFunc>(post_func)]
				(std::shared_ptr<connection> conn, char const* data, size_t size)
			{
				CodecPolicy cp{};
				auto args_tuple = cp.template unpack<args_tuple_type>(data, size);
				std::async([conn, at = std::move(args_tuple), head = conn->head_, &h, &p]
				{
					auto result = invoker_call_handler(h, conn, at);
					auto message = CodecPolicy{}.pack(result);
					auto ctx = context_t::make_message_with_head(head, std::move(message),
						[conn, &p, r = std::move(result)]
					{
						p(conn, r);
					});
					conn->response(ctx);
				});
			};

			return invoker;
		}
	};
} }