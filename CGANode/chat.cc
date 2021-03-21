#include <nan.h>
#include "../CGALib/gameinterface.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

extern bool g_PlayerSyncPosition;

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

	if (msg.unitid == -1 && msg.msg.find("\xe9\xad\x94\xe6\x97\x8f\xe7\x9a\x84\xe6\xb0\x94\xe6\x81\xaf") != std::string::npos)
	{
		g_PlayerSyncPosition = true;
	}
}

void ChatMsgAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (ChatMsgNotifyData *)handle->data;

	Local<Value> nullValue = Nan::Null();
	Local<Value> argv[2];
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "unitid").ToLocalChecked(), Integer::New(isolate, data->m_msg.unitid));
		obj->Set(context, String::NewFromUtf8(isolate, "msg").ToLocalChecked(), Nan::New(data->m_msg.msg).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "color").ToLocalChecked(), Integer::New(isolate, data->m_msg.color));
		obj->Set(context, String::NewFromUtf8(isolate, "size").ToLocalChecked(), Integer::New(isolate, data->m_msg.size));
		argv[1] = obj;
	}
	
	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void ChatMsgTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

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
		Local<Value> argv[1];
		argv[0] = Nan::Error("Async callback timeout.");

		Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 1, argv);

		data->m_callback.Reset();

		uv_timer_stop(handle);
		uv_close((uv_handle_t*)data->m_async, FreeUVHandleCallBack);
		uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
		delete handle->data;
	}
}

void AsyncWaitChatMsg(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int timeout = 3000;
	if (info.Length() < 1 || !info[0]->IsFunction()) {
		Nan::ThrowTypeError("Arg[0] must be a function.");
		return;
	}
	if (info.Length() >= 2 && !info[1]->IsUndefined())
	{
		timeout = info[1]->Int32Value(context).ToChecked();
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

//LostConn

void ConnectionStateAsyncCallBack(uv_async_t *handle);

class ConnectionStateNotifyData : public CBaseNotifyData
{
public:
	ConnectionStateNotifyData() : CBaseNotifyData(ConnectionStateAsyncCallBack)
	{

	}

	CGA::cga_conn_state_t m_msg;
};

class ConnectionStateCacheData
{
public:
	ConnectionStateCacheData(CGA::cga_conn_state_t msg, uint64_t time) : m_msg(msg), m_time(time)
	{

	}
	CGA::cga_conn_state_t m_msg;
	uint64_t m_time;
};

boost::shared_mutex g_ConnectionState_Lock;
std::vector<ConnectionStateNotifyData *> g_ConnectionState_Asyncs;
std::vector<ConnectionStateCacheData *> g_ConnectionState_Caches;

void ConnectionStateDoAsyncCall(const CGA::cga_conn_state_t &msg)
{
	if (g_ConnectionState_Asyncs.size())
	{
		for (size_t i = 0; i < g_ConnectionState_Asyncs.size(); ++i)
		{
			const auto data = g_ConnectionState_Asyncs[i];

			data->m_msg = msg;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_ConnectionState_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_ConnectionState_Caches.begin(); p != g_ConnectionState_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_ConnectionState_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_ConnectionState_Caches.emplace_back(new ConnectionStateCacheData(msg, uv_hrtime()));
	}
}

void ConnectionStateNotify(CGA::cga_conn_state_t msg)
{
	boost::unique_lock<boost::shared_mutex> lock(g_ConnectionState_Lock);
	ConnectionStateDoAsyncCall(msg);
}

void ConnectionStateAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (ConnectionStateNotifyData *)handle->data;

	Local<Value> nullValue = Nan::Null();
	Local<Value> argv[2];
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "state").ToLocalChecked(), Integer::New(isolate, data->m_msg.state));
		obj->Set(context, String::NewFromUtf8(isolate, "msg").ToLocalChecked(), Nan::New(data->m_msg.msg).ToLocalChecked());
		argv[1] = obj;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void ConnectionStateTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (ConnectionStateNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_ConnectionState_Lock);
		for (size_t i = 0; i < g_ConnectionState_Asyncs.size(); ++i)
		{
			if (g_ConnectionState_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_ConnectionState_Asyncs.erase(g_ConnectionState_Asyncs.begin() + i);
				break;
			}
		}
	}

	if (asyncNotCalled)
	{
		Local<Value> argv[1];
		argv[0] = Nan::Error("Async callback timeout.");

		Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 1, argv);

		data->m_callback.Reset();

		uv_timer_stop(handle);
		uv_close((uv_handle_t*)data->m_async, FreeUVHandleCallBack);
		uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
		delete handle->data;
	}
}

void AsyncWaitConnectionState(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int timeout = 3000;
	if (info.Length() < 1 || !info[0]->IsFunction()) {
		Nan::ThrowTypeError("Arg[0] must be a function.");
		return;
	}
	if (info.Length() >= 2 && !info[1]->IsUndefined())
	{
		timeout = info[1]->Int32Value(context).ToChecked();
		if (timeout < 0)
			timeout = 0;
	}

	auto callback = Local<Function>::Cast(info[0]);
	ConnectionStateNotifyData *data = new ConnectionStateNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_ConnectionState_Lock);
		g_ConnectionState_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_ConnectionState_Caches.begin(); p != g_ConnectionState_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_ConnectionState_Caches.erase(p);
			}
			else
			{
				ConnectionStateDoAsyncCall((*p)->m_msg);
				delete *p;
				g_ConnectionState_Caches.erase(p);
				break;
			}
		}
	}

	if (timeout)
		uv_timer_start(data->m_timer, ConnectionStateTimerCallBack, timeout, 0);
}

//MessageBox

void MessageBoxAsyncCallBack(uv_async_t *handle);

class MessageBoxNotifyData : public CBaseNotifyData
{
public:
	MessageBoxNotifyData() : CBaseNotifyData(MessageBoxAsyncCallBack)
	{

	}

	std::string m_msg;
};

class MessageBoxCacheData
{
public:
	MessageBoxCacheData(const std::string &msg, uint64_t time) : m_msg(msg), m_time(time)
	{

	}
	std::string m_msg;
	uint64_t m_time;
};

boost::shared_mutex g_MessageBox_Lock;
std::vector<MessageBoxNotifyData *> g_MessageBox_Asyncs;
std::vector<MessageBoxCacheData *> g_MessageBox_Caches;

void MessageBoxDoAsyncCall(const std::string &msg)
{
	if (g_MessageBox_Asyncs.size())
	{
		for (size_t i = 0; i < g_MessageBox_Asyncs.size(); ++i)
		{
			const auto data = g_MessageBox_Asyncs[i];

			data->m_msg = msg;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_MessageBox_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_MessageBox_Caches.begin(); p != g_MessageBox_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_MessageBox_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_MessageBox_Caches.emplace_back(new MessageBoxCacheData(msg, uv_hrtime()));
	}
}

void MessageBoxNotify(std::string msg)
{
	boost::unique_lock<boost::shared_mutex> lock(g_MessageBox_Lock);
	MessageBoxDoAsyncCall(msg);
}

void MessageBoxAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (MessageBoxNotifyData *)handle->data;

	Local<Value> nullValue = Nan::Null();
	Local<Value> argv[2];
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "msg").ToLocalChecked(), Nan::New(data->m_msg).ToLocalChecked());
		argv[1] = obj;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void MessageBoxTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (MessageBoxNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_MessageBox_Lock);
		for (size_t i = 0; i < g_MessageBox_Asyncs.size(); ++i)
		{
			if (g_MessageBox_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_MessageBox_Asyncs.erase(g_MessageBox_Asyncs.begin() + i);
				break;
			}
		}
	}

	if (asyncNotCalled)
	{
		Local<Value> argv[1];
		argv[0] = Nan::Error("Async callback timeout.");

		Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 1, argv);

		data->m_callback.Reset();

		uv_timer_stop(handle);
		uv_close((uv_handle_t*)data->m_async, FreeUVHandleCallBack);
		uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
		delete handle->data;
	}
}

void AsyncWaitMessageBox(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int timeout = 3000;
	if (info.Length() < 1 || !info[0]->IsFunction()) {
		Nan::ThrowTypeError("Arg[0] must be function.");
		return;
	}
	if (info.Length() >= 2 && info[1]->IsInt32())
	{
		timeout = info[1]->Int32Value(context).ToChecked();
		if (timeout < 0)
			timeout = 0;
	}

	auto callback = Local<Function>::Cast(info[0]);
	MessageBoxNotifyData *data = new MessageBoxNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_MessageBox_Lock);
		g_MessageBox_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_MessageBox_Caches.begin(); p != g_MessageBox_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_MessageBox_Caches.erase(p);
			}
			else
			{
				MessageBoxDoAsyncCall((*p)->m_msg);
				delete *p;
				g_MessageBox_Caches.erase(p);
				break;
			}
		}
	}

	if (timeout)
		uv_timer_start(data->m_timer, MessageBoxTimerCallBack, timeout, 0);
}