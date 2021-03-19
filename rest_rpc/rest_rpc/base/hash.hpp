#pragma once

#include <cstdint>

namespace timax
{
	template <typename HashPolicy, typename T>
	struct hash
	{
		using hash_policy = HashPolicy;
		using result_type = typename T;
		using argument_type = typename hash_policy::argument_type;

		result_type operator() (argument_type const& arg) const
		{
			return hash_policy::hash<T>(arg);
		}
	};

	template <typename T>
	struct bkdr_hash
	{
		using argument_type = T;

		template <typename ResultT>
		static auto hash(argument_type const& arg)
		{
			return hash_impl<ResultT>(reinterpret_cast<uint8_t const*>(arg.c_str()),
				arg.size() * sizeof(typename argument_type::value_type));
		}

	private:

		template <typename ResultT>
		static auto hash_impl(uint8_t const* first, size_t count)
		{
			ResultT seed = 131;
			ResultT val = 0;

			for (size_t loop = 0; loop < count; ++loop)
			{
				val = val * seed + first[loop];
			}

			return val;
		}
	};
}