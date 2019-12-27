#pragma once

#include <boost/type_traits.hpp>

#define TIMAX_DEFINE_PROTOCOL(handler, ...) static const ::timax::rpc::rpc_protocol<__VA_ARGS__> handler{ #handler }
#define TIMAX_DEFINE_FORWARD(handler, ...) static const ::timax::rpc::forward_protocol<__VA_ARGS__> handler{ #handler }

namespace timax { namespace rpc
{
	template <typename Func, typename ... Args>
	struct is_argument_match
	{
	private:
		template <typename T>
		static std::false_type test(...);
	
		template <typename T, typename =
			decltype(std::declval<T>()(std::declval<Args>()...))>
		static std::true_type test(int);
	
		using result_type = decltype(test<Func>(0));
	public:
		static constexpr bool value = result_type::value;
	};

	template <typename Func>
	struct rpc_protocol;

	template <typename Func>
	struct rpc_protocol_base;

	template <typename Ret, typename ... Args>
	struct rpc_protocol_base<Ret(Args...)>
	{
		using result_type = typename boost::function_traits<Ret(Args...)>::result_type;
		using signature_type = Ret(Args...);
		using hash_engine = hash<bkdr_hash<std::string>, uint64_t>;

		explicit rpc_protocol_base(std::string const& name)
			: name_(static_cast<uint64_t>(hash_engine{}(name)))
		{
		}

		uint64_t name() const noexcept
		{
			return name_;
		}

		template <typename CodecPolicy, typename ... TArgs>
		auto pack_args(CodecPolicy const& cp, TArgs&& ... args) const
		{
			static_assert(is_argument_match<signature_type, TArgs...>::value, "Arguments` types don`t match the protocol!");
			return cp.pack_args(std::forward<Args>(static_cast<Args>(args))...);
		}

		template <typename CodecPolicy>
		auto pack_topic(CodecPolicy const& cp) const
		{
			return cp.pack_args(name_);
		}

	private:
		uint64_t		name_;
	};

	template <typename Ret, typename ... Args>
	struct rpc_protocol<Ret(Args...)> : rpc_protocol_base<Ret(Args...)>
	{
		using base_type = rpc_protocol_base<Ret(Args...)>;
		using result_type = typename base_type::result_type;
		using signature_type = typename base_type::signature_type;

		explicit rpc_protocol(std::string const& name)
			: base_type(name)
		{
		}

		template <typename CodecPolicy, typename = std::enable_if_t<!std::is_void<result_type>::value>>
		auto pack_result(CodecPolicy const& cp, result_type&& ret) const
		{
			return cp.pack(std::forward<result_type>(ret));
		}

		template <typename CodecPolicy, typename = std::enable_if_t<!std::is_void<result_type>::value>>
		result_type unpack(CodecPolicy& cp, char const* data, size_t length) const
		{
			return cp.template unpack<result_type>(data, length);
		}
	};

	template <typename ... Args>
	struct rpc_protocol<void(Args...)> : rpc_protocol_base<void(Args...)>
	{
		using base_type = rpc_protocol_base<void(Args...)>;
		using result_type = typename base_type::result_type;
		using signature_type = typename base_type::signature_type;

		explicit rpc_protocol(std::string name)
			: base_type(std::move(name))
		{
		}
	};

	template <typename ... Args>
	struct check_forward_protocol
	{
		static constexpr bool value = false;
	};

	template <>
	struct check_forward_protocol<>
	{
		static constexpr bool value = true;
	};

	template <typename Arg0, typename ... Args>
	struct check_forward_protocol<Arg0, Args...>
	{
		static constexpr bool value = !std::is_void<Arg0>::value
			&& check_forward_protocol<Args...>::value;
	};

	template <typename ... Args>
	struct forward_protocol
	{
		static_assert(check_forward_protocol<Args...>::value, "illegal protocol parameters!");
		using tuple_type = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
		using hash_engine = hash<bkdr_hash<std::string>, uint64_t>;

		forward_protocol(std::string const& name)
			: topic_name_(name)
			, name_(static_cast<uint64_t>(hash_engine{}(name)))
		{
		}

		template <typename CodecPolicy, typename ... TArgs>
		auto pack(CodecPolicy const& cp, TArgs&& ... args) const
		{
			static_assert(is_argument_match<void(Args...), TArgs...>::value, "Arguments` types don`t match the protocol!");
			return cp.pack_args(std::forward<Args>(static_cast<Args>(args))...);
		}

		template <typename CodecPolicy, typename ... TArgs>
		auto pack_args(CodecPolicy const& cp, TArgs&& ... args) const
		{
			return pack(cp, std::forward<TArgs>(args)...);
		}

		template <typename CodecPolicy>
		auto unpack(CodecPolicy& cp, char const* data, size_t size) const
		{
			return cp.template unpack<tuple_type>(data, size);
		}

		uint64_t name() const
		{
			return name_;
		}

		std::string const& topic() const noexcept
		{
			return topic_name_;
		}

		template <typename CodecPolicy>
		auto pack_topic(CodecPolicy const& cp) const
		{
			return cp.pack_args(topic_name_);
		}

	private:
		std::string		topic_name_;
		uint64_t			name_;
	};

	template <>
	struct forward_protocol<>
	{
	};

	template <typename>
	struct is_forward_protocol : std::false_type {};

	template <typename ... Args>
	struct is_forward_protocol<forward_protocol<Args...>> : std::true_type {};

	template <typename>
	struct is_rpc_protocol : std::false_type {};

	template <typename Func>
	struct is_rpc_protocol<rpc_protocol<Func>> : std::true_type {};

	TIMAX_DEFINE_PROTOCOL(sub_topic, std::string(std::string const&));
} }
