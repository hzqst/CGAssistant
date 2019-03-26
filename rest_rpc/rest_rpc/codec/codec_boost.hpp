#pragma once

namespace boost { namespace serialization {
	/**
	* serialization for tuples
	*/
	template<typename Archive, size_t... I, typename... Args>
	void serialize(Archive & ar, const std::index_sequence<I...>&, std::tuple<Args...> & t, unsigned int version)
	{
		bool swallow[] = { (ar & std::get<I>(t), false)... };
		(void*)swallow;
	}

	template<typename Archive, typename... Args>
	void serialize(Archive & ar, std::tuple<Args...> & t, unsigned int version)
	{
		serialize(ar, std::make_index_sequence<sizeof... (Args)>{}, t, version);
	}

	template <typename Archive, typename Tuple, size_t ... Is>
	void serialize_meta(Archive& ar, Tuple& tuple, std::index_sequence<Is...>)
	{
		bool swallow[] = { (ar & std::get<Is>(tuple).second, false)... };
		(void*)swallow;
	}

	template <typename Archive, typename Tuple>
	void serialize_meta(Archive & ar, Tuple& tuple)
	{
		serialize_meta(ar, tuple, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
	}

	template <typename Archive, typename T, typename = typename T::meta_tag>
	auto serialize(Archive& ar, T& t, unsigned int version) -> std::enable_if_t<timax::rpc::has_meta_macro<T>::value>
	{
		serialize_meta(ar, t.Meta());
	}

} // end serialization namespace 
} // end boost namespace

namespace timax { namespace rpc 
{
	struct boost_codec
	{
		template<typename T>
		T unpack(char const* data, size_t length)
		{
			std::stringstream ss;
			ss.write(data, length);
			boost::archive::text_iarchive ia(ss);
			T t;
			ia >> t;
			return t;
		}

		using buffer_type = std::vector<char>;

		template <typename ... Args>
		buffer_type pack_args(Args&& ... args) const
		{
			auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
			std::stringstream ss;
			boost::archive::text_oarchive oa(ss);
			oa << args_tuple;

			return assign(ss);
		}

		template <typename T>
		buffer_type pack(T&& t)
		{
			std::stringstream ss;
			boost::archive::text_oarchive oa(ss);
			oa << std::forward<T>(t);

			return assign(ss);
		}

		std::vector<char> assign(std::stringstream& ss) const
		{
			std::vector<char> vec;
			std::streampos beg = ss.tellg();
			ss.seekg(0, std::ios_base::end);
			std::streampos end = ss.tellg();
			ss.seekg(0, std::ios_base::beg);
			vec.reserve(end - beg);

			vec.assign(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>());
			return vec;
		}
	};
} }
