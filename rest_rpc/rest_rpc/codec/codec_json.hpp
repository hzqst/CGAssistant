#pragma once

#include <iguana/json.hpp>

namespace timax { namespace rpc 
{
	struct json_codec
	{
		using buffer_type = std::string;

		template <typename ... Args>
		buffer_type pack_args(Args&& ... args) const
		{
			iguana::string_stream ss;
			auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
			iguana::json::to_json(ss, args_tuple);
			return ss.str();
		}

		template <typename T>
		buffer_type pack(T&& t) const
		{
			iguana::string_stream ss;
			iguana::json::to_json(ss, std::forward<T>(t));
			return ss.str();
		}

		template <typename T>
		T unpack(char const* data, size_t length)
		{
			try
			{
				T t;
				iguana::json::from_json(t, data, length);
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