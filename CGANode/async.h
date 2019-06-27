#pragma once 

class CBaseNotifyData
{
public:
	CBaseNotifyData(const uv_async_cb async_cb)
	{
		m_result = false;
		m_async = new uv_async_t;
		m_timer = new uv_timer_t;
		uv_timer_init(uv_default_loop(), m_timer);
		uv_async_init(uv_default_loop(), m_async, async_cb);
	}

	bool m_result;
	Persistent<Function> m_callback;	
	uv_async_t *m_async;
	uv_timer_t *m_timer;
};

void FreeUVHandleCallBack(uv_handle_t *handle);

#define CGA_NOTIFY_MAX_CACHE_TIME 3000