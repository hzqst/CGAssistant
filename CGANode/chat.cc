#include <nan.h>
#include "../CGALib/gameinterface.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

#include "async.h"

void ChatMsgAsyncCallBack(uv_async_t *handle);

class ChatMsgNotifyData : public CBaseNotifyData
{
public:
	ChatMsgNotifyData() : CBaseNotifyData(ChatMsgAsyncCallBack)
	{

	}

	CGA::cga_chat_msg_t m_msg;
};

class ChatMsgCacheData
{
public:
	ChatMsgCacheData(const CGA::cga_chat_msg_t &msg, uint64_t time) : m_msg(msg), m_time(time)
	{

	}
	CGA::cga_chat_msg_t m_msg;
	uint64_t m_time;
};

boost::shared_mutex g_ChatMsg_Lock;
std::vector<ChatMsgNotifyData *> g_ChatMsg_Asyncs;
std::vector<ChatMsgCacheData *> g_ChatMsg_Caches;

void ChatMsgDoAsyncCall(const CGA::cga_chat_msg_t &msg)
{
	if (g_ChatMsg_Asyncs.size())
	{
		for (size_t i = 0; i < g_ChatMsg_Asyncs.size(); ++i)
		{
			const auto data = g_ChatMsg_Asyncs[i];

			data->m_msg = msg;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_ChatMsg_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_ChatMsg_Caches.begin(); p != g_ChatMsg_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_ChatMsg_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_ChatMsg_Caches.emplace_back(new ChatMsgCacheData(msg, uv_hrtime()));
	}
}

void ChatMsgNotify(CGA::cga_chat_msg_t msg)
{
	boost::unique_lock<boost::shared_mutex> lock(g_ChatMsg_Lock);
	ChatMsgDoAsyncCall(msg);
}

void ChatMsgAsyncCallBack(uv_async_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (ChatMsgNotifyData *)handle->data;

	Handle<Value> argv[1];
	if (data->m_result)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(String::NewFromUtf8(isolate, "unitid"), Integer::New(isolate, data->m_msg.unitid));
		obj->Set(String::NewFromUtf8(isolate, "msg"), Nan::New(data->m_msg.msg).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "color"), Integer::New(isolate, data->m_msg.color));
		obj->Set(String::NewFromUtf8(isolate, "size"), Integer::New(isolate, data->m_msg.size));
		argv[0] = obj;
	}
	else
	{
		argv[0] = Nan::New(false);
	}

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void ChatMsgTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (ChatMsgNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_ChatMsg_Lock);
		for (size_t i = 0; i < g_ChatMsg_Asyncs.size(); ++i)
		{
			if (g_ChatMsg_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_ChatMsg_Asyncs.erase(g_ChatMsg_Asyncs.begin() + i);
				break;
			}
		}
	}

	if (asyncNotCalled)
	{
		Handle<Value> argv[1];
		argv[0] = Nan::TypeError("Async callback timeout.");

		Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

		data->m_callback.Reset();

		uv_timer_stop(handle);
		uv_close((uv_handle_t*)data->m_async, FreeUVHandleCallBack);
		uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
		delete handle->data;
	}
}

void AsyncWaitChatMsg(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	int timeout = 3000;
	if (info.Length() < 1 || !info[0]->IsFunction()) {
		Nan::ThrowTypeError("Arg[0] must be a function.");
		return;
	}
	if (info.Length() >= 2 && !info[1]->IsUndefined())
	{
		timeout = (int)info[1]->IntegerValue();
		if (timeout < 0)
			timeout = 0;
	}

	auto callback = Local<Function>::Cast(info[0]);
	ChatMsgNotifyData *data = new ChatMsgNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_ChatMsg_Lock);
		g_ChatMsg_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_ChatMsg_Caches.begin(); p != g_ChatMsg_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_ChatMsg_Caches.erase(p);
			}
			else
			{
				ChatMsgDoAsyncCall((*p)->m_msg);
				delete *p;
				g_ChatMsg_Caches.erase(p);
				break;
			}
		}
	}

	if(timeout)
		uv_timer_start(data->m_timer, ChatMsgTimerCallBack, timeout, 0);
}