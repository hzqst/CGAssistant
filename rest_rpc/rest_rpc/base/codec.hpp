#pragma once
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 
#include <sstream> 

namespace boost { namespace serialization {
	/**
	* serialization for tuples
	*/
	template<typename Archive, size_t... I, typename... Args>
	void serialize(Archive & ar, const std::index_sequence<I...>&, std::tuple<Args...> & t, unsigned int version)
	{
		bool arr[] = { (ar & std::get<I>(t), false)... };
		(void*)arr;
	}

	template<typename Archive, typename... Args>
	void serialize(Archive & ar, std::tuple<Args...> & t, unsigned int version)
	{
		serialize(ar, std::make_index_sequence<sizeof... (Args)>{}, t, version);
	}

} // end serialization namespace 
} // end boost namespace

namespace timax { namespace rpc
{
	struct blob_t
	{
		blob_t() : raw_ref_() {}
		blob_t(char const* data, size_t size)
			: raw_ref_(data, static_cast<uint32_t>(size))
		{
		}

		template <typename Packer>
		void msgpack_pack(Packer& pk) const 
		{
			pk.pack_bin(raw_ref_.size);
			pk.pack_bin_body(raw_ref_.ptr, raw_ref_.size);
		}

		void msgpack_unpack(msgpack::object const& o)
		{
			msgpack::operator>>(o, raw_ref_);
		}

		auto data() const
		{
			return raw_ref_.ptr;
		}
		
		size_t size() const
		{
			return raw_ref_.size;
		}

		msgpack::type::raw_ref	raw_ref_;
	};

	struct msgpack_codec
	{
		//using buffer_type = msgpack::sbuffer;
		using buffer_type = std::vector<char>;
		class buffer_t
		{
		public:
			buffer_t()
				: buffer_t(0)
			{ }

			explicit buffer_t(size_t len)
				: buffer_(len, 0)
				, offset_(0)
			{ }

			buffer_t(buffer_t const&) = default;
			buffer_t(buffer_t &&) = default;
			buffer_t& operator= (buffer_t const&) = default;
			buffer_t& operator= (buffer_t &&) = default;

			void write(char const* data, size_t length)
			{
				if (buffer_.size() - offset_ < length)
					buffer_.resize(length + offset_);

				std::memcpy(buffer_.data() + offset_, data, length);
				offset_ += length;
			}

			std::vector<char> release() const noexcept
			{
				return std::move(buffer_);
			}

		private:
			std::vector<char>		buffer_;
			size_t				offset_;
		};

		template <typename ... Args>
		buffer_type pack_args(Args&& ... args) const
		{
			buffer_t buffer;
			auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
			msgpack::pack(buffer, args_tuple);
			return buffer.release();
		}

		template <typename T>
		buffer_type pack(T&& t) const
		{
			buffer_t buffer;
			msgpack::pack(buffer, std::forward<T>(t));
			return buffer.release();
		}

		template <typename T>
		T unpack(char const* data, size_t length)
		{
			try
			{
				msgpack::unpack(&msg_, data, length);
				return msg_.get().as<T>();
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

namespace timax { namespace rpc 
{
	template <typename T>
	struct is_std_tuple : std::false_type {};

	template <typename ... Args>
	struct is_std_tuple<std::tuple<Args...>> : std::true_type {};

	template <typename CodecPolicy, typename Arg>
	auto pack_as_tuple_if_not_impl(std::true_type, CodecPolicy const& cp, Arg&& arg)
	{
		return cp.pack(std::forward<Arg>(arg));
	}

	template <typename CodecPolicy, typename Arg>
	auto pack_as_tuple_if_not_impl(std::false_type, CodecPolicy const& cp, Arg&& arg)
	{
		return cp.pack_args(std::forward<Arg>(arg));
	}

	template <typename CodecPolicy, typename Arg>
	auto pack_as_tuple_if_not(CodecPolicy const& cp, Arg&& arg)
	{
		return pack_as_tuple_if_not_impl(is_std_tuple<std::remove_cv_t<std::remove_reference_t<Arg>>>{},
			cp, std::forward<Arg>(arg));
	}
} }
