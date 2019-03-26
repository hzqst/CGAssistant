#pragma once 

class CBaseNotifyData
{
public:
	CBaseNotifyData(const uv_async_cb async_cb)
	{
		m_result = false;
		uv_timer_init(uv_default_loop(), &m_timer);
		uv_async_init(uv_default_loop(), &m_async, async_cb);
	}

	bool m_result;
	Persistent<Function> m_callback;	
	uv_async_t m_async;
	uv_timer_t m_timer;
};