#include <nan.h>
#include "../CGALib/gameinterface.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

#include "async.h"

void NPCDialogAsyncCallBack(uv_async_t *handle);

class NPCDialogNotifyData : public CBaseNotifyData
{
public:
	NPCDialogNotifyData() : CBaseNotifyData(NPCDialogAsyncCallBack)
	{
	}

	CGA::cga_npc_dialog_t m_dlg;
};

class NPCDialogCacheData
{
public:
	NPCDialogCacheData(const CGA::cga_npc_dialog_t &dlg, uint64_t time) : m_dlg(dlg), m_time(time)
	{
	
	}
	CGA::cga_npc_dialog_t m_dlg;
	uint64_t m_time;
};

boost::shared_mutex g_NPCDialog_Lock;
std::vector<NPCDialogNotifyData *> g_NPCDialog_Asyncs;
std::vector<NPCDialogCacheData *> g_NPCDialog_Caches;

void NPCDialogNotifyDoAsyncCall(const CGA::cga_npc_dialog_t &dlg)
{
	if (g_NPCDialog_Asyncs.size())
	{
		for (size_t i = 0; i < g_NPCDialog_Asyncs.size(); ++i)
		{
			const auto data = g_NPCDialog_Asyncs[i];
			data->m_dlg = dlg;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_NPCDialog_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_NPCDialog_Caches.begin(); p != g_NPCDialog_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_NPCDialog_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_NPCDialog_Caches.emplace_back(new NPCDialogCacheData(dlg, uv_hrtime()));
	}
}

void NPCDialogNotify(CGA::cga_npc_dialog_t dlg)
{
	boost::unique_lock<boost::shared_mutex> lock(g_NPCDialog_Lock);
	NPCDialogNotifyDoAsyncCall(dlg);
}

void NPCDialogAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (NPCDialogNotifyData *)handle->data;

	Local<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "type").ToLocalChecked(), Integer::New(isolate, data->m_dlg.type));
		obj->Set(context, String::NewFromUtf8(isolate, "options").ToLocalChecked(), Integer::New(isolate, data->m_dlg.options));
		obj->Set(context, String::NewFromUtf8(isolate, "dialog_id").ToLocalChecked(), Integer::New(isolate, data->m_dlg.dialog_id));
		obj->Set(context, String::NewFromUtf8(isolate, "npc_id").ToLocalChecked(), Integer::New(isolate, data->m_dlg.npc_id));
		obj->Set(context, String::NewFromUtf8(isolate, "message").ToLocalChecked(), Nan::New(data->m_dlg.message).ToLocalChecked());
		argv[1] = obj;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void NPCDialogTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (NPCDialogNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_NPCDialog_Lock);
		for (size_t i = 0; i < g_NPCDialog_Asyncs.size(); ++i)
		{
			if (g_NPCDialog_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_NPCDialog_Asyncs.erase(g_NPCDialog_Asyncs.begin() + i);
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

void AsyncWaitNPCDialog(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
	NPCDialogNotifyData *data = new NPCDialogNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_NPCDialog_Lock);

		g_NPCDialog_Asyncs.emplace_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_NPCDialog_Caches.begin(); p != g_NPCDialog_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_NPCDialog_Caches.erase(p);
			}
			else
			{
				NPCDialogNotifyDoAsyncCall((*p)->m_dlg);
				delete *p;
				g_NPCDialog_Caches.erase(p);
				break;
			}
		}
	}

	if(timeout)
		uv_timer_start(data->m_timer, NPCDialogTimerCallBack, timeout, 0);
}

void ClickNPCDialog(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 && !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integr.");
		return;
	}

	if (info.Length() < 2 && !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be intger.");
		return;
	}

	int option = info[0]->Int32Value(context).ToChecked();
	int index = info[1]->Int32Value(context).ToChecked();
	bool bResult = false;

	if (!g_CGAInterface->ClickNPCDialog(option, index, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}

void SellNPCStore(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsArray()) {
		Nan::ThrowTypeError("Arg[0] must be array.");
		return;
	}

	bool bResult = false;
	CGA::cga_sell_items_t items;

	Local<Array> arr = Local<Array>::Cast(info[0]);
	auto length = arr->Length();

	for (uint32_t i = 0; i < length; ++i)
	{
		auto element = arr->Get(context, i);
		if (!element.IsEmpty() && element.ToLocalChecked()->IsObject())
		{
			int itemid = -1;
			int itempos = -1;
			int count = 0;

			auto obj = Local<Object>::Cast(element.ToLocalChecked());

			auto obj_itemid = obj->Get(context, String::NewFromUtf8(isolate, "itemid").ToLocalChecked());

			if (!obj_itemid.IsEmpty() && obj_itemid.ToLocalChecked()->IsInt32())
			{
				itemid = obj_itemid.ToLocalChecked()->Int32Value(context).ToChecked();
			}

			auto obj_itempos = obj->Get(context, String::NewFromUtf8(isolate, "itempos").ToLocalChecked());

			if (!obj_itempos.IsEmpty() && obj_itempos.ToLocalChecked()->IsInt32())
			{
				itempos = obj_itempos.ToLocalChecked()->Int32Value(context).ToChecked();
			}

			auto obj_count = obj->Get(context, String::NewFromUtf8(isolate, "count").ToLocalChecked());

			if (!obj_count.IsEmpty() && obj_count.ToLocalChecked()->IsInt32())
			{
				count = obj_count.ToLocalChecked()->Int32Value(context).ToChecked();
			}

			if (count > 0)
				items.emplace_back(itemid, itempos, count);
		}
	}

	if (!g_CGAInterface->SellNPCStore(items, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}

void BuyNPCStore(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsArray()) {
		Nan::ThrowTypeError("Arg[0] must be array.");
		return;
	}

	bool bResult = false;
	CGA::cga_buy_items_t items;

	Local<Array> arr = Local<Array>::Cast(info[0]);
	auto length = arr->Length();

	for (uint32_t i = 0; i < length; ++i)
	{
		auto element = arr->Get(context, i);
		if (!element.IsEmpty() && element.ToLocalChecked()->IsObject())
		{
			int index = -1;
			int count = 0;

			auto obj = Local<Object>::Cast(element.ToLocalChecked());

			auto obj_index = obj->Get(context, String::NewFromUtf8(isolate, "index").ToLocalChecked());

			if (!obj_index.IsEmpty() && obj_index.ToLocalChecked()->IsInt32())
			{
				index = obj_index.ToLocalChecked()->Int32Value(context).ToChecked();
			}

			auto obj_count = obj->Get(context, String::NewFromUtf8(isolate, "count").ToLocalChecked());

			if (!obj_count.IsEmpty() && obj_count.ToLocalChecked()->IsInt32())
			{
				count = obj_count.ToLocalChecked()->Int32Value(context).ToChecked();
			}

			if (count > 0)
				items.emplace_back(index, count);
		}
	}

	if (!g_CGAInterface->BuyNPCStore(items, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}