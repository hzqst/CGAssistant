#pragma once

namespace timax{ namespace rpc 
{
	struct req_header
	{
		req_header() = default;

		req_header(int16_t code, uint32_t id, uint32_t len, uint64_t hash)
			: code(code)
			, id(id)
			, len(len)
			, hash(hash)
		{
		}

		int16_t		code = 0;
		uint32_t		id = 0;
		uint32_t		len = 0;
		uint64_t		hash = 0;
	};

	struct rep_header
	{
		rep_header() = default;

		rep_header(int16_t code, uint32_t id, uint32_t len)
			: code(code)
			, id(id)
			, len(len)
		{
		}

		rep_header(req_header const& header)
			: code(header.code)
			, id(header.id)
			, len(header.len)
		{
		}

		int16_t		code = 0;
		uint32_t		id = 0;
		uint32_t		len = 0;
	};

	enum class result_code : int16_t
	{
		OK = 0,
		FAIL = 1,
	};

	enum class error_code 
	{
		OK,
		UNKNOWN,	
		FAIL,
		TIMEOUT,
		CANCEL,					
		BADCONNECTION,
	};
} }

