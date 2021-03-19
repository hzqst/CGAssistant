#pragma once

namespace timax { namespace rpc 
{
	static const char* SUB_TOPIC = "sub_topic";
	static const char* PUB = "pub";
	static const char* SUB_CONFIRM = "sub_confirm";
	static const char* HEART_BEAT = "heart_beat";
	static const char* RESULT = "result";
	static const char* CODE = "code";

	static const size_t MAX_BUF_LEN = 1048576*10;
	static const size_t HEAD_LEN = 12;
	static const size_t PAGE_SIZE = 1024;
	static const size_t MAX_QUEUE_SIZE = 10240;
} }