#pragma once

#include <iguana/xml.hpp>

namespace timax { namespace rpc
{
	struct xml_codec
	{
		using buffer_type = std::string;

		template <typename ... Args>
		buffer_type pack_args(Args&& ... args) const
		{
			iguana::string_stream ss;
			auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
			iguana::xml::to_xml(ss, args_tuple);
			return ss.str();
		}

		template <typename T>
		buffer_type pack(T&& t) const
		{
			iguana::string_stream ss;
			iguana::xml::to_xml(ss, std::forward<T>(t));
			return ss.str();
		}

		template <typename T>
		T unpack(char const* data, size_t length)
		{
			try
			{
				T t;
				iguana::xml::from_xml(t, data, length);
				return t;
			}
			catch (...)
			{
				using namespace std::string_literals;
				exception error{ error_code::FAIL, "Args not match!"s };
				throw error;
			}
		}
	};
} }
