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
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (NPCDialogNotifyData *)handle->data;

	Handle<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::TypeError("Unknown exception.");
	if (data->m_result)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(String::NewFromUtf8(isolate, "type"), Integer::New(isolate, data->m_dlg.type));
		obj->Set(String::NewFromUtf8(isolate, "options"), Integer::New(isolate, data->m_dlg.options));
		obj->Set(String::NewFromUtf8(isolate, "dialog_id"), Integer::New(isolate, data->m_dlg.dialog_id));
		obj->Set(String::NewFromUtf8(isolate, "npc_id"), Integer::New(isolate, data->m_dlg.npc_id));
		obj->Set(String::NewFromUtf8(isolate, "message"), Nan::New(data->m_dlg.message).ToLocalChecked());
		argv[1] = obj;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void NPCDialogTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

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

void AsyncWaitNPCDialog(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be option.");
		return;
	}

	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be index.");
		return;
	}

	int option = (int)info[0]->IntegerValue();
	int index = (int)info[1]->IntegerValue();
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsArray()) {
		Nan::ThrowTypeError("Arg[0] must be an array.");
		return;
	}

	Local<Object> objarr = Local<Object>::Cast(info[0]);
	uint32_t length = objarr->Get(Nan::New("length").ToLocalChecked())->Uint32Value();

	Local<Array> arr = Local<Array>::Cast(info[0]);
	bool bResult = false;
	CGA::cga_sell_items_t items;
	for (uint32_t i = 0; i < length; ++i)
	{
		Local<Object> obj = Local<Object>::Cast(arr->Get(i));
		Local<Value> obj_itemid = obj->Get(Nan::New("itemid").ToLocalChecked());
		Local<Value> obj_itempos = obj->Get(Nan::New("itempos").ToLocalChecked());
		Local<Value> obj_count = obj->Get(Nan::New("count").ToLocalChecked());
		int itemid = 0;
		int itempos = 0;
		int count = 0;
		if(!obj_itemid->IsUndefined())
			itemid = (int)obj_itemid->IntegerValue();
		if (!obj_itempos->IsUndefined())
			itempos = (int)obj_itempos->IntegerValue();
		if (!obj_count->IsUndefined())
			count = (int)obj_count->IntegerValue();
		if (count > 0)
			items.emplace_back(itemid, itempos, count);
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsArray()) {
		Nan::ThrowTypeError("Arg[0] must be an array.");
		return;
	}

	Local<Object> objarr = Local<Object>::Cast(info[0]);
	uint32_t length = objarr->Get(Nan::New("length").ToLocalChecked())->Uint32Value();

	Local<Array> arr = Local<Array>::Cast(info[0]);
	bool bResult = false;
	CGA::cga_buy_items_t items;
	for (uint32_t i = 0; i < length; ++i)
	{
		Local<Object> obj = Local<Object>::Cast(arr->Get(i));
		Local<Value> obj_index = obj->Get(Nan::New("index").ToLocalChecked());
		Local<Value> obj_count = obj->Get(Nan::New("count").ToLocalChecked());
		int index = 0;
		int count = 0;
		if (!obj_index->IsUndefined())
			index = (int)obj_index->IntegerValue();
		if (!obj_count->IsUndefined())
			count = (int)obj_count->IntegerValue();
		if (count > 0)
			items.emplace_back(index, count);
	}

	if (!g_CGAInterface->BuyNPCStore(items, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}