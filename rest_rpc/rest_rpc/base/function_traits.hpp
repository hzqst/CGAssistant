#pragma once

//member function
#define TIMAX_FUNCTION_TRAITS(...)\
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits_impl<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits_impl<ReturnType(Args...)>{};\

namespace std
{
	template <int Size>
	struct is_placeholder<boost::arg<Size>>
		: public std::integral_constant<int, Size>
	{
	};
}

namespace timax 
{
	 /*
	  * 1. function type							==>	Ret(Args...)
	  * 2. function pointer						==>	Ret(*)(Args...)
	  * 3. function reference						==>	Ret(&)(Args...)
	  * 4. pointer to non-static member function	==> Ret(T::*)(Args...)
	  * 5. function object and functor				==> &T::operator()
	  * 6. function with generic operator call		==> template <typeanme ... Args> &T::operator()
	  */
	template <typename T>
	struct function_traits_impl;

	template<typename T>
	struct function_traits : function_traits_impl<
		std::remove_cv_t<std::remove_reference_t<T>>>
	{};

	template<typename Ret, typename... Args>
	struct function_traits_impl<Ret(Args...)>
	{
	public:
		enum { arity = sizeof...(Args) };
		typedef Ret function_type(Args...);
		typedef Ret result_type;
		using stl_function_type = std::function<function_type>;
		typedef Ret(*pointer)(Args...);

		template<size_t I>
		struct args
		{
			static_assert(I < arity, "index is out of range, index must less than sizeof Args");
			using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
		};

		typedef std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...> tuple_type;
		using raw_tuple_type = std::tuple<Args...>;
	};

	// function pointer
	template<typename Ret, typename... Args>
	struct function_traits_impl<Ret(*)(Args...)> : function_traits<Ret(Args...)> {};

	// std::function
	template <typename Ret, typename... Args>
	struct function_traits_impl<std::function<Ret(Args...)>> : function_traits_impl<Ret(Args...)> {};

	// pointer of non-static member function
	TIMAX_FUNCTION_TRAITS()
	TIMAX_FUNCTION_TRAITS(const)
	TIMAX_FUNCTION_TRAITS(volatile)
	TIMAX_FUNCTION_TRAITS(const volatile)

	// functor
	template<typename Callable>
	struct function_traits_impl : function_traits_impl<decltype(&Callable::operator())> {};

	template <typename Function>
	typename function_traits<Function>::stl_function_type to_function(const Function& lambda)
	{
		return static_cast<typename function_traits<Function>::stl_function_type>(lambda);
	}

	template <typename Function>
	typename function_traits<Function>::stl_function_type to_function(Function&& lambda)
	{
		return static_cast<typename function_traits<Function>::stl_function_type>(std::forward<Function>(lambda));
	}

	template <typename Function>
	typename function_traits<Function>::pointer to_function_pointer(const Function& lambda)
	{
		return static_cast<typename function_traits<Function>::pointer>(lambda);
	}
}

namespace timax
{
	// tags for function overloaded resolution
	struct caller_is_a_pointer {};
	struct caller_is_a_smart_pointer {};
	struct caller_is_a_reference {};

	// voider ultility for SFINAE
	template <typename ...>
	struct voider
	{
		using type = void;
	};

	template <typename ... Args>
	using voider_t = typename voider<Args...>::type;

	namespace discard_temporary
	{
		// vs2015 update3+ supported
		template <typename T, typename = void>
		struct is_smart_pointer : std::false_type
		{
		};

		// this way of using SFINEA is type reference and cv qualifiers immuned
		template <typename T>
		struct is_smart_pointer<T,
			voider_t<
			decltype(std::declval<T>().operator ->()),
			decltype(std::declval<T>().get())
			>> : std::true_type
		{
		};
	}
	
	template <typename T>
	struct is_smart_pointer
	{
	private:
		template <typename U>
		static std::false_type test(...);

		template <typename U,
			typename = decltype(std::declval<U>().operator ->()),
			typename = decltype(std::declval<U>().get())>
		static std::true_type test(int);

		using type = decltype(test<T>(0));
	public:
		static constexpr bool value = type::value;
	};

	template <typename Arg0, typename Tuple>
	struct push_front_to_tuple_type;

	template <typename Arg0, typename ... Args>
	struct push_front_to_tuple_type<Arg0, std::tuple<Args...>>
	{
		using type = std::tuple<Arg0, Args...>;
	};

	template <size_t I, typename IndexSequence>
	struct push_front_to_index_sequence;

	template <size_t I, size_t ... Is>
	struct push_front_to_index_sequence<I, std::index_sequence<Is...>>
	{
		using type = std::index_sequence<I, Is...>;
	};

	template <typename ArgsTuple, typename ... BindArgs>
	struct make_bind_index_sequence_and_args_tuple;

	template <typename ArgsTuple>
	struct make_bind_index_sequence_and_args_tuple<ArgsTuple>
	{
		using index_sequence_type = std::index_sequence<>;
		using args_tuple_type = std::tuple<>;
	};

	template <typename Arg0, typename ... Args, typename BindArg0, typename ... BindArgs>
	struct make_bind_index_sequence_and_args_tuple<std::tuple<Arg0, Args...>, BindArg0, BindArgs...>
	{
	private:
		using bind_arg0_type = std::remove_reference_t<std::remove_cv_t<BindArg0>>;
		constexpr static auto ph_value = std::is_placeholder<bind_arg0_type>::value;
		constexpr static bool is_not_placeholder = 0 == ph_value;
		using rests_make_bind_type = make_bind_index_sequence_and_args_tuple<std::tuple<Args...>, BindArgs...>;
		using rests_index_sequence_type = typename rests_make_bind_type::index_sequence_type;
		using rests_args_tuple_type = typename rests_make_bind_type::args_tuple_type;
		
	public:
		using index_sequence_type = std::conditional_t<is_not_placeholder, rests_index_sequence_type,
			typename push_front_to_index_sequence<ph_value - 1, rests_index_sequence_type>::type>;
		using args_tuple_type = std::conditional_t<is_not_placeholder, rests_args_tuple_type,
			typename push_front_to_tuple_type<Arg0, rests_args_tuple_type>::type>;
	};

	template <typename Ret, typename ... Args>
	struct function_helper
	{
		using type = std::function<Ret(Args...)>;
	};

	template <typename IndexSequence, typename Ret, typename ArgsTuple>
	struct bind_traits;

	template <size_t ... Is, typename Ret, typename ArgsTuple>
	struct bind_traits<std::index_sequence<Is...>, Ret, ArgsTuple>
	{
		using type = typename function_helper<Ret, std::tuple_element_t<Is, ArgsTuple>...>::type;
	};

	template <typename F, typename Arg0, typename ... Args>
	struct bind_to_function
	{
	private:
		using function_traits_t = function_traits<F>;
		using raw_args_tuple_t = typename function_traits_t::raw_tuple_type;
		using make_bind_t = std::conditional_t<std::is_member_function_pointer<F>::value,
			make_bind_index_sequence_and_args_tuple<raw_args_tuple_t, Args...>,
			make_bind_index_sequence_and_args_tuple<raw_args_tuple_t, Arg0, Args...>>;
		using index_sequence_t = typename make_bind_t::index_sequence_type;
		using args_tuple_t = typename make_bind_t::args_tuple_type;
	public:
		using type = typename bind_traits<index_sequence_t, typename function_traits_t::result_type, args_tuple_t>::type;
	};

	// dealing with bugs from gcc, we cannot pass a constant reference of boost::arg<Size> object to std::bind
	template <typename T>
	struct forward
	{
		template <typename T1>
		static decltype(auto) apply(T1&& t1) noexcept
		{
			return std::forward<T>(t1);
		}
	};

	// thus, we wrap std::forward so that we can directly return a boost::arg<Size> object.
	template <int Size>
	struct forward<boost::arg<Size> const&>
	{
		template <typename T1>
		static auto apply(T1&& t1) noexcept
		{
			return t1;
		}
	};

	template <typename F, typename Arg0, typename ... Args>
	auto bind_impl(std::false_type /*IsNoPmf*/, F&& f, Arg0&& arg0, Args&& ... args)
		-> typename bind_to_function<F, Arg0, Args...>::type
	{
		return std::bind(std::forward<F>(f), std::forward<Arg0>(arg0), std::forward<Args>(args)...);
	}

	template <typename F>
	auto bind_impl(std::false_type /*IsNoPmf*/, F&& f)
		-> typename function_traits<F>::stl_function_type
	{
		return [func = std::forward<F>(f)](auto&& ... args){ return func(std::forward<decltype(args)>(args)...); };
	}

	template <typename F, typename Caller, typename Arg0, typename ... Args>
	auto bind_impl(std::true_type /*IsPmf*/, F&& pmf, Caller&& caller, Arg0&& arg0, Args&& ... args)
		-> typename bind_to_function<F, Caller, Arg0, Args...>::type
	{
		return std::bind(pmf, std::forward<Caller>(caller), std::forward<Arg0>(arg0), std::forward<Args>(args)...);
	}

	template <typename F, typename Caller>
	auto bind_impl_pmf_no_placeholder(caller_is_a_pointer, F&& pmf, Caller&& caller)
		-> typename function_traits<F>::stl_function_type
	{
		return [pmf, c = std::forward<Caller>(caller)](auto&& ... args) 
		{ 
			return (c->*pmf)(std::forward<decltype(args)>(args)...); 
		};
	}

	template <typename F, typename Caller>
	auto bind_impl_pmf_no_placeholder(caller_is_a_smart_pointer, F&& pmf, Caller&& caller)
		-> typename function_traits<F>::stl_function_type
	{
		return [pmf, c = std::forward<Caller>(caller)](auto&& ... args) 
		{ 
			return (c.get()->*pmf)(std::forward<decltype(args)>(args)...); 
		};
	}

	template <typename F, typename Caller>
	auto bind_impl_pmf_no_placeholder(caller_is_a_reference, F&& pmf, Caller&& caller)
		-> typename function_traits<F>::stl_function_type
	{
		return [pmf, c = std::forward<Caller>(caller)](auto&& ... args) 
		{
			return (c.*pmf)(std::forward<decltype(args)>(args)...); 
		};
	}

	template <typename F, typename Caller>
	auto bind_impl(std::true_type /*IsPmf*/, F&& pmf, Caller&& caller)
	{
		using caller_category_t = std::conditional_t<std::is_pointer<Caller>::value,
			caller_is_a_pointer, std::conditional_t<is_smart_pointer<Caller>::value,
				caller_is_a_smart_pointer, caller_is_a_reference >> ;

		return bind_impl_pmf_no_placeholder(caller_category_t{}, std::forward<F>(pmf), std::forward<Caller>(caller));
	}

	template <typename F, typename ... Args>
	auto bind(F&& f, Args&& ... args)
	{
		using is_pmf = typename std::is_member_function_pointer<F>::type;

		return bind_impl(is_pmf{}, std::forward<F>(f), forward<Args>::apply(args)...);
	}
}