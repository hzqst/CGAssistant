#pragma once

#include <iguana/msgpack.hpp>

namespace timax { namespace rpc 
{
	struct msgpack_codec
	{
		using buffer_type = std::vector<char>;

		template <typename ... Args>
		buffer_type pack_args(Args&& ... args) const
		{
			iguana::memory_buffer buffer;
			auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
			iguana::msgpack::to_msgpack(buffer, args_tuple);
			return buffer.release();
		}

		template <typename T>
		buffer_type pack(T&& t) const
		{
			iguana::memory_buffer buffer;
			iguana::msgpack::to_msgpack(buffer, std::forward<T>(t));
			return buffer.release();
		}

		template <typename T>
		T unpack(char const* data, size_t length)
		{
			try
			{
				T t;
				iguana::msgpack::from_msgpack(t, msg_, data, length);
				return t;
			}
			catch (...)
			{
				using namespace std::string_literals;
				exception error{ error_code::FAIL, "Args not match!"s };
				throw error;
			}
		}

	private:
		msgpack::unpacked msg_;
	};
} }
