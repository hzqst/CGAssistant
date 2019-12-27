#pragma once

namespace timax { namespace rpc 
{
	class router_base : boost::noncopyable
	{
	public:
		using connection_ptr = std::shared_ptr<connection>;
		using invoker_t = std::function<void(connection_ptr, char const*, size_t)>;
		using invoker_container = std::unordered_map<uint64_t, invoker_t>;
		using on_read_func = std::function<void(connection_ptr)>;
		using on_error_func = std::function<void(connection_ptr, boost::system::error_code const& error)>;

	public:
		bool register_invoker(uint64_t name, invoker_t&& invoker)
		{
			return invokers_.emplace(name, std::move(invoker)).second;
		}

		bool has_invoker(uint64_t name) noexcept
		{
			invokers_.find(name) != invokers_.end();
		}

		void on_read(connection_ptr const& conn_ptr)
		{
			if(on_read_)
				on_read_(conn_ptr);
		}

		void on_error(connection_ptr const& conn_ptr, boost::system::error_code const& error)
		{
			if (on_error_)
				on_error_(conn_ptr, error);
		}

		void set_on_read(on_read_func&& on_read)
		{
			on_read_ = std::move(on_read);
		}

		void set_on_error(on_error_func&& on_error)
		{
			on_error_ = std::move(on_error);
		}

	protected:
		invoker_container			invokers_;
		on_read_func				on_read_;
		on_error_func				on_error_;
	};

	template <typename CodecPolicy>
	class router : public router_base
	{
	public:
		using codec_policy = CodecPolicy;
		using message_t = typename codec_policy::buffer_type;
		using connection_ptr = typename router_base::connection_ptr;
		using invoker_t = typename router_base::invoker_t;
		using invoker_container = typename router_base::invoker_container;
		using on_read_func = typename router_base::on_read_func;
		using on_error_func = typename router_base::on_error_func;
		using hash_engine = hash<bkdr_hash<std::string>, uint64_t>;

	public:
		template <typename Handler, typename PostFunc>
		bool register_invoker(std::string const& name, Handler&& handler, PostFunc&& post_func)
		{
			using handler_traits_type = handler_traits<Handler>;
			using invoker_traits_type = invoker_traits<
				typename handler_traits_type::return_tag, handler_exec_sync>;

			return register_invoker_impl<invoker_traits_type>(name,
				std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
		}

		template <typename Handler>
		bool register_invoker(std::string const& name, Handler&& handler)
		{
			using handler_traits_type = handler_traits<Handler>;
			using invoker_traits_type = invoker_traits<
				typename handler_traits_type::return_tag, handler_exec_sync>;

			return register_invoker_impl<invoker_traits_type>(name, std::forward<Handler>(handler));
		}

		template <typename Handler, typename PostFunc>
		bool async_register_invoker(std::string const& name, Handler&& handler, PostFunc&& post_func)
		{
			using handler_traits_type = handler_traits<Handler>;
			using invoker_traits_type = invoker_traits<
				typename handler_traits_type::return_tag, handler_exec_async>;

			return register_invoker_impl<invoker_traits_type>(name,
				std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
		}

		template <typename Handler>
		bool async_register_invoker(std::string const& name, Handler&& handler)
		{
			using handler_traits_type = handler_traits<Handler>;
			using invoker_traits_type = invoker_traits<
				typename handler_traits_type::return_tag, handler_exec_async>;

			return register_invoker_impl<invoker_traits_type>(name, std::forward<Handler>(handler));
		}

		template <typename Handler>
		bool register_raw_invoker(std::string const& name, Handler&& handler)
		{
			return register_invoker_impl(name, std::forward<Handler>(handler));
		}

		bool has_invoker(std::string const& name) const
		{
			auto name_hash = hash_(name);
			return router_base::has_invoker(name_hash);
		}

		void apply_invoker(connection_ptr conn, char const* data, size_t size) const
		{
			static auto cannot_find_invoker_error = codec_policy{}.pack(exception{ error_code::FAIL, "Cannot find handler!" });

			auto& header = conn->get_read_header();
			auto itr = this->invokers_.find(header.hash);
			if (this->invokers_.end() == itr)
			{
				auto ctx = context_t::make_error_message(header, cannot_find_invoker_error);
				conn->response(ctx);
			}
			else
			{
				auto& invoker = itr->second;
				if (!invoker)
				{
					auto ctx = context_t::make_error_message(header, cannot_find_invoker_error);
					conn->response(ctx);
				}

				try
				{
					invoker(conn, data, size);
				}
				catch (exception const& error)
				{
					// response serialized exception to client
					auto args_not_match_error = codec_policy{}.pack(error);
					auto args_not_match_error_message = context_t::make_error_message(header,
						std::move(args_not_match_error));
					conn->response(args_not_match_error_message);
				}
			}
		}

	private:
		template <typename InvokerTraits, typename ... Handlers>
		bool register_invoker_impl(std::string const& name, Handlers&& ... handlers)
		{
			auto hash = hash_(name);
			auto itr = this->invokers_.find(hash);
			if (this->invokers_.end() != itr)
				return false;

			auto invoker = InvokerTraits::template get<codec_policy>(std::forward<Handlers>(handlers)...);
			this->invokers_.emplace(hash, std::move(invoker));
			return true;
		}

		template <typename Handler>
		bool register_invoker_impl(std::string const& name, Handler&& handler)
		{
			auto hash = hash_(name);
			auto itr = this->invokers_.find(hash);
			if (this->invokers_.end() != itr)
				return false;

			invoker_t invoker = std::forward<Handler>(handler);
			this->invokers_.emplace(hash, std::move(invoker));
			return true;
		}

	private:
		// mutable std::mutex		mutex_;
		hash_engine				hash_;
	};
} }
