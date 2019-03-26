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
		//m_sent = false;
		//m_permanent = false;
		//m_timeout = 0;
	}

	//int m_timeout;
	//bool m_sent;
	//bool m_permanent;
	CGA::cga_chat_msg_t m_msg;
};

boost::shared_mutex  g_ChatMsg_Lock;
std::vector<ChatMsgNotifyData *> g_ChatMsg_Asyncs;
/*
static bool is_removable(ChatMsgNotifyData *data)
{
	return (data->m_sent && !data->m_permanent);
}
*/
void ChatMsgNotify(CGA::cga_chat_msg_t msg)
{
	boost::unique_lock<boost::shared_mutex> lock(g_ChatMsg_Lock);
	for (size_t i = 0; i < g_ChatMsg_Asyncs.size(); ++i)
	{
		const auto data = g_ChatMsg_Asyncs[i];
		//if (!data->m_sent)
		//{
			data->m_msg = msg;
			data->m_result = true;
			//data->m_sent = true;
			uv_async_send(&data->m_async);
		//}
	}
	g_ChatMsg_Asyncs.clear();
	//g_ChatMsg_Asyncs.erase(std::remove_if(std::begin(g_ChatMsg_Asyncs), std::end(g_ChatMsg_Asyncs), is_removable), std::end(g_ChatMsg_Asyncs));
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
	//if (!data->m_permanent)
	//{
		data->m_callback.Reset();

		uv_timer_stop(&data->m_timer);
		uv_close((uv_handle_t*)handle, NULL);
		uv_close((uv_handle_t*)&data->m_timer, NULL);
		delete handle->data;
	/*}
	else
	{
		data->m_sent = false;
	}*/
}

void ChatMsgTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (ChatMsgNotifyData *)handle->data;

	Handle<Value> argv[1];
	argv[0] = Nan::TypeError("Async callback timeout.");

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);
	//if (!data->m_permanent)
	//{
		data->m_callback.Reset();

		{
			boost::unique_lock<boost::shared_mutex> lock(g_ChatMsg_Lock);
			for (size_t i = 0; i < g_ChatMsg_Asyncs.size(); ++i)
			{
				if (g_ChatMsg_Asyncs[i] == data)
				{
					g_ChatMsg_Asyncs.erase(g_ChatMsg_Asyncs.begin() + i);
					break;
				}
			}
		}

		uv_timer_stop(handle);
		uv_close((uv_handle_t*)&data->m_async, NULL);
		uv_close((uv_handle_t*)handle, NULL);
		delete handle->data;
	/*}
	else
	{
		if (data->m_timeout > 0)
		{
			uv_timer_start(&data->m_timer, ChatMsgTimerCallBack, data->m_timeout, 0);
			data->m_sent = false;
		}
	}*/
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
	/*bool permanent = false;
	if (info.Length() >= 3 && info[2]->IsBoolean())
	{
		permanent = info[2]->BooleanValue();
	}*/

	auto callback = Local<Function>::Cast(info[0]);
	ChatMsgNotifyData *data = new ChatMsgNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async.data = data;
	data->m_timer.data = data;
	//data->m_timeout = timeout;
	//data->m_permanent = permanent;

	boost::unique_lock<boost::shared_mutex> lock(g_ChatMsg_Lock);
	g_ChatMsg_Asyncs.push_back(data);

	if(timeout)
		uv_timer_start(&data->m_timer, ChatMsgTimerCallBack, timeout, 0);
}