#pragma once

#include "codec_msgpack.hpp"
#include "codec_json.hpp"
#include "codec_xml.hpp"

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