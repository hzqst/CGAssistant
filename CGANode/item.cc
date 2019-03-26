#include <nan.h>
#include "../CGALib/gameinterface.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace v8;

#include "async.h"

extern CGA::CGAInterface *g_CGAInterface;

void PlayerMenuAsyncCallBack(uv_async_t *handle);
void UnitMenuAsyncCallBack(uv_async_t *handle);

class PlayerMenuNotifyData : public CBaseNotifyData
{
public:
	PlayerMenuNotifyData() : CBaseNotifyData(PlayerMenuAsyncCallBack)
	{
	}

	CGA::cga_player_menu_items_t m_players;
};

class UnitMenuNotifyData : public CBaseNotifyData
{
public:
	UnitMenuNotifyData() : CBaseNotifyData(UnitMenuAsyncCallBack)
	{
	}

	CGA::cga_unit_menu_items_t m_units;
};

boost::shared_mutex  g_PlayerMenu_Lock;
std::vector<PlayerMenuNotifyData *> g_PlayerMenu_Asyncs;

boost::shared_mutex  g_UnitMenu_Lock;
std::vector<UnitMenuNotifyData *> g_UnitMenu_Asyncs;

void DropItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be itempos.");
		return;
	}

	int itempos = (int)info[0]->IntegerValue();
	bool result = false;
	if (!g_CGAInterface->DropItem(itempos, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void PlayerMenuNotify(CGA::cga_player_menu_items_t players)
{
	boost::unique_lock<boost::shared_mutex> lock(g_PlayerMenu_Lock);
	for (size_t i = 0; i < g_PlayerMenu_Asyncs.size(); ++i)
	{
		const auto data = g_PlayerMenu_Asyncs[i];
		data->m_players = players;
		data->m_result = true;
		uv_async_send(&data->m_async);
	}
	g_PlayerMenu_Asyncs.clear();
}

void PlayerMenuAsyncCallBack(uv_async_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (PlayerMenuNotifyData *)handle->data;

	Handle<Value> argv[1];
	if (data->m_result)
	{
		Local<Array> arr = Array::New(isolate);
		for (size_t i = 0; i < data->m_players.size(); ++i)
		{
			Local<Object> obj = Object::New(isolate);
			obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(data->m_players[i].name).ToLocalChecked());
			obj->Set(String::NewFromUtf8(isolate, "color"), Integer::New(isolate, data->m_players[i].color));
			obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, data->m_players[i].index));
			arr->Set(i, obj);
		}
		argv[0] = arr;
	}
	else
	{
		argv[0] = Nan::New(false);
	}

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(&data->m_timer);
	uv_close((uv_handle_t*)handle, NULL);
	uv_close((uv_handle_t*)&data->m_timer, NULL);
	delete handle->data;
}

void PlayerMenuTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (PlayerMenuNotifyData *)handle->data;

	Handle<Value> argv[1];
	argv[0] = Nan::TypeError("Async callback timeout.");

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

	data->m_callback.Reset();

	{
		boost::unique_lock<boost::shared_mutex> lock(g_PlayerMenu_Lock);
		for (size_t i = 0; i < g_PlayerMenu_Asyncs.size(); ++i)
		{
			if (g_PlayerMenu_Asyncs[i] == data)
			{
				g_PlayerMenu_Asyncs.erase(g_PlayerMenu_Asyncs.begin() + i);
				break;
			}
		}
	}

	uv_timer_stop(handle);
	uv_close((uv_handle_t*)&data->m_async, NULL);
	uv_close((uv_handle_t*)handle, NULL);
	delete handle->data;
}

void UnitMenuNotify(CGA::cga_unit_menu_items_t units)
{
	boost::unique_lock<boost::shared_mutex> lock(g_UnitMenu_Lock);
	for (size_t i = 0; i < g_UnitMenu_Asyncs.size(); ++i)
	{
		const auto data = g_UnitMenu_Asyncs[i];
		data->m_units = units;
		data->m_result = true;
		uv_async_send(&data->m_async);
	}
	g_UnitMenu_Asyncs.clear();
}

void UnitMenuAsyncCallBack(uv_async_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (UnitMenuNotifyData *)handle->data;

	Handle<Value> argv[1];
	if (data->m_result)
	{
		Local<Array> arr = Array::New(isolate);
		for (size_t i = 0; i < data->m_units.size(); ++i)
		{
			Local<Object> obj = Object::New(isolate);
			obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(data->m_units[i].name).ToLocalChecked());
			obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, data->m_units[i].level));
			obj->Set(String::NewFromUtf8(isolate, "health"), Integer::New(isolate, data->m_units[i].health));
			obj->Set(String::NewFromUtf8(isolate, "hp"), Integer::New(isolate, data->m_units[i].hp));
			obj->Set(String::NewFromUtf8(isolate, "maxhp"), Integer::New(isolate, data->m_units[i].maxhp));
			obj->Set(String::NewFromUtf8(isolate, "mp"), Integer::New(isolate, data->m_units[i].mp));
			obj->Set(String::NewFromUtf8(isolate, "maxmp"), Integer::New(isolate, data->m_units[i].maxmp));
			obj->Set(String::NewFromUtf8(isolate, "color"), Integer::New(isolate, data->m_units[i].color));
			obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, data->m_units[i].index));
			arr->Set(i, obj);
		}
		argv[0] = arr;
	}
	else
	{
		argv[0] = Nan::New(false);
	}

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(&data->m_timer);
	uv_close((uv_handle_t*)handle, NULL);
	uv_close((uv_handle_t*)&data->m_timer, NULL);
	delete handle->data;
}

void UnitMenuTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (UnitMenuNotifyData *)handle->data;

	Handle<Value> argv[1];
	argv[0] = Nan::TypeError("Async callback timeout.");

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

	data->m_callback.Reset();

	{
		boost::unique_lock<boost::shared_mutex> lock(g_UnitMenu_Lock);
		for (size_t i = 0; i < g_UnitMenu_Asyncs.size(); ++i)
		{
			if (g_UnitMenu_Asyncs[i] == data)
			{
				g_UnitMenu_Asyncs.erase(g_UnitMenu_Asyncs.begin() + i);
				break;
			}
		}
	}

	uv_timer_stop(handle);
	uv_close((uv_handle_t*)&data->m_async, NULL);
	uv_close((uv_handle_t*)handle, NULL);
	delete handle->data;
}

void UseItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be itempos.");
		return;
	}

	int itempos = (int)info[0]->IntegerValue();
	bool bResult = false;
	if (!g_CGAInterface->UseItem(itempos, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void MoveItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be itempos.");
		return;
	}
	
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be dstpos.");
		return;
	}
	
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Arg[2] must be count.");
		return;
	}

	int itempos = (int)info[0]->IntegerValue();
	int dstpos = (int)info[1]->IntegerValue();
	int count = (int)info[2]->IntegerValue();
	
	bool bResult = false;
	if (!g_CGAInterface->MoveItem(itempos, dstpos, count, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void AsyncWaitPlayerMenu(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	boost::unique_lock<boost::shared_mutex> lock(g_PlayerMenu_Lock);

	auto callback = Local<Function>::Cast(info[0]);
	PlayerMenuNotifyData *data = new PlayerMenuNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async.data = data;
	data->m_timer.data = data;

	g_PlayerMenu_Asyncs.push_back(data);
	if(timeout)
		uv_timer_start(&data->m_timer, PlayerMenuTimerCallBack, timeout, 0);
}

void PlayerMenuSelect(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be menu index.");
		return;
	}

	int menuindex = (int)info[0]->IntegerValue();
	bool bResult = false;
	if (!g_CGAInterface->PlayerMenuSelect(menuindex, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}

void AsyncWaitUnitMenu(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	boost::unique_lock<boost::shared_mutex> lock(g_UnitMenu_Lock);

	auto callback = Local<Function>::Cast(info[0]);
	UnitMenuNotifyData *data = new UnitMenuNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async.data = data;
	data->m_timer.data = data;

	g_UnitMenu_Asyncs.push_back(data);
	if(timeout)
		uv_timer_start(&data->m_timer, UnitMenuTimerCallBack, timeout, 0);
}

void UnitMenuSelect(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be menu index.");
		return;
	}

	int menuindex = (int)info[0]->IntegerValue();
	bool bResult = false;
	if (!g_CGAInterface->UnitMenuSelect(menuindex, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}