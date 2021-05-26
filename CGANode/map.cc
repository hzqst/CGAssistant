#include <time.h>
#include <nan.h>
#include "../CGALib/gameinterface.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace v8;

#include "async.h"

extern CGA::CGAInterface *g_CGAInterface;

bool g_PlayerSyncPosition = false;

void DownloadMapAsyncCallBack(uv_async_t *handle);

class DownloadMapNotifyData : public CBaseNotifyData
{
public:
	DownloadMapNotifyData() : CBaseNotifyData(DownloadMapAsyncCallBack)
	{

	}

	CGA::cga_download_map_t m_msg;
};

class DownloadMapCacheData
{
public:
	DownloadMapCacheData(const CGA::cga_download_map_t &msg, uint64_t time) : m_msg(msg), m_time(time)
	{

	}
	CGA::cga_download_map_t m_msg;
	uint64_t m_time;
};

boost::shared_mutex g_DownloadMap_Lock;
std::vector<DownloadMapNotifyData *> g_DownloadMap_Asyncs;
std::vector<DownloadMapCacheData *> g_DownloadMap_Caches;

void DownloadMapDoAsyncCall(const CGA::cga_download_map_t &msg)
{
	if (g_DownloadMap_Asyncs.size())
	{
		for (size_t i = 0; i < g_DownloadMap_Asyncs.size(); ++i)
		{
			const auto data = g_DownloadMap_Asyncs[i];

			data->m_msg = msg;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_DownloadMap_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_DownloadMap_Caches.begin(); p != g_DownloadMap_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_DownloadMap_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_DownloadMap_Caches.emplace_back(new DownloadMapCacheData(msg, uv_hrtime()));
	}
}

void DownloadMapNotify(CGA::cga_download_map_t msg)
{
	boost::unique_lock<boost::shared_mutex> lock(g_DownloadMap_Lock);
	DownloadMapDoAsyncCall(msg);
}

void DownloadMapAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (DownloadMapNotifyData *)handle->data;

	Local<Value> nullValue = Nan::Null();
	Local<Value> argv[2];
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "index1").ToLocalChecked(), Integer::New(isolate, data->m_msg.index1));
		obj->Set(context, String::NewFromUtf8(isolate, "index3").ToLocalChecked(), Integer::New(isolate, data->m_msg.index3));
		obj->Set(context, String::NewFromUtf8(isolate, "xbase").ToLocalChecked(), Integer::New(isolate, data->m_msg.xbase));
		obj->Set(context, String::NewFromUtf8(isolate, "ybase").ToLocalChecked(), Integer::New(isolate, data->m_msg.ybase));
		obj->Set(context, String::NewFromUtf8(isolate, "xtop").ToLocalChecked(), Integer::New(isolate, data->m_msg.xtop));
		obj->Set(context, String::NewFromUtf8(isolate, "ytop").ToLocalChecked(), Integer::New(isolate, data->m_msg.ytop));
		argv[1] = obj;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void DownloadMapTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (DownloadMapNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_DownloadMap_Lock);
		for (size_t i = 0; i < g_DownloadMap_Asyncs.size(); ++i)
		{
			if (g_DownloadMap_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_DownloadMap_Asyncs.erase(g_DownloadMap_Asyncs.begin() + i);
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

void AsyncWaitDownloadMap(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int timeout = 3000;
	if (info.Length() < 1 || !info[0]->IsFunction()) {
		Nan::ThrowTypeError("Arg[0] must be a function.");
		return;
	}
	if (info.Length() >= 2 && info[1]->IsInt32())
	{
		timeout = info[1]->Int32Value(context).ToChecked();
		if (timeout < 0)
			timeout = 0;
	}

	auto callback = Local<Function>::Cast(info[0]);
	DownloadMapNotifyData *data = new DownloadMapNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_DownloadMap_Lock);
		g_DownloadMap_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_DownloadMap_Caches.begin(); p != g_DownloadMap_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_DownloadMap_Caches.erase(p);
			}
			else
			{
				DownloadMapDoAsyncCall((*p)->m_msg);
				delete *p;
				g_DownloadMap_Caches.erase(p);
				break;
			}
		}
	}

	if(timeout)
		uv_timer_start(data->m_timer, DownloadMapTimerCallBack, timeout, 0);
}

void RequestDownloadMap(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be integer.");
		return;
	}
	
	if (info.Length() < 3 || !info[2]->IsInt32()) {
		Nan::ThrowTypeError("Arg[2] must be integer.");
		return;
	}
	
	if (info.Length() < 4 || !info[3]->IsInt32()) {
		Nan::ThrowTypeError("Arg[3] must be integer.");
		return;
	}

	int xbottom = info[0]->Int32Value(context).ToChecked();
	int ybottom = info[1]->Int32Value(context).ToChecked();
	int xsize = info[2]->Int32Value(context).ToChecked();
	int ysize = info[3]->Int32Value(context).ToChecked();

	if (!g_CGAInterface->RequestDownloadMap(xbottom, ybottom, xsize, ysize))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void FixMapWarpStuck(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int type = info[0]->Int32Value(context).ToChecked();

	if (!g_CGAInterface->FixMapWarpStuck(type))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void GetMapIndex(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int index1, index2, index3;
	if (!g_CGAInterface->GetMapIndex(index1, index2, index3))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "index1").ToLocalChecked(), Integer::New(isolate, index1));
	obj->Set(context, String::NewFromUtf8(isolate, "index2").ToLocalChecked(), Integer::New(isolate, index2));
	obj->Set(context, String::NewFromUtf8(isolate, "index3").ToLocalChecked(), Integer::New(isolate, index3));
	info.GetReturnValue().Set(obj);
}

void GetMapXY(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int x, y;
	if (!g_CGAInterface->GetMapXY(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "x").ToLocalChecked(), Integer::New(isolate, x));
	obj->Set(context, String::NewFromUtf8(isolate, "y").ToLocalChecked(), Integer::New(isolate, y));
	info.GetReturnValue().Set(obj);
}

void GetMapXYFloat(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	float x, y;
	if (!g_CGAInterface->GetMapXYFloat(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "x").ToLocalChecked(), Number::New(isolate, x));
	obj->Set(context, String::NewFromUtf8(isolate, "y").ToLocalChecked(), Number::New(isolate, y));
	info.GetReturnValue().Set(obj);
}

void GetMoveSpeed(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	float x, y;
	if (!g_CGAInterface->GetMoveSpeed(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "x").ToLocalChecked(), Number::New(isolate, x));
	obj->Set(context, String::NewFromUtf8(isolate, "y").ToLocalChecked(), Number::New(isolate, y));
	info.GetReturnValue().Set(obj);
}

void GetMapName(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	std::string name;
	if (!g_CGAInterface->GetMapName(name))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
}

void GetMapUnits(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	CGA::cga_map_units_t units;
	if (!g_CGAInterface->GetMapUnits(units))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < units.size(); ++i)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "valid").ToLocalChecked(), Integer::New(isolate, units[i].valid));
		obj->Set(context, String::NewFromUtf8(isolate, "type").ToLocalChecked(), Integer::New(isolate, units[i].type));
		obj->Set(context, String::NewFromUtf8(isolate, "model_id").ToLocalChecked(), Integer::New(isolate, units[i].model_id));
		obj->Set(context, String::NewFromUtf8(isolate, "unit_id").ToLocalChecked(), Integer::New(isolate, units[i].unit_id));
		obj->Set(context, String::NewFromUtf8(isolate, "xpos").ToLocalChecked(), Integer::New(isolate, units[i].xpos));
		obj->Set(context, String::NewFromUtf8(isolate, "ypos").ToLocalChecked(), Integer::New(isolate, units[i].ypos));
		obj->Set(context, String::NewFromUtf8(isolate, "item_count").ToLocalChecked(), Integer::New(isolate, units[i].item_count));
		obj->Set(context, String::NewFromUtf8(isolate, "injury").ToLocalChecked(), Integer::New(isolate, units[i].injury));
		obj->Set(context, String::NewFromUtf8(isolate, "icon").ToLocalChecked(), Integer::New(isolate, units[i].icon));
		obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, units[i].level));
		obj->Set(context, String::NewFromUtf8(isolate, "flags").ToLocalChecked(), Integer::New(isolate, units[i].flags));
		obj->Set(context, String::NewFromUtf8(isolate, "unit_name").ToLocalChecked(), Nan::New(units[i].unit_name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "nick_name").ToLocalChecked(), Nan::New(units[i].nick_name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "title_name").ToLocalChecked(), Nan::New(units[i].title_name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "item_name").ToLocalChecked(), Nan::New(units[i].item_name).ToLocalChecked());
		arr->Set(context, i, obj);
	}

	info.GetReturnValue().Set(arr);
}

void GetMapCollisionTableRaw(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[0] must be boolean.");
		return;
	}
	
	bool loadall = info[0]->BooleanValue(isolate);

	CGA::cga_map_cells_t cells;
	if (!g_CGAInterface->GetMapCollisionTableRaw(loadall, cells))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "x_bottom").ToLocalChecked(), Integer::New(isolate, cells.x_bottom));
	obj->Set(context, String::NewFromUtf8(isolate, "y_bottom").ToLocalChecked(), Integer::New(isolate, cells.y_bottom));
	obj->Set(context, String::NewFromUtf8(isolate, "x_size").ToLocalChecked(), Integer::New(isolate, cells.x_size));
	obj->Set(context, String::NewFromUtf8(isolate, "y_size").ToLocalChecked(), Integer::New(isolate, cells.y_size));
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < cells.cell.size(); ++i)
	{
		arr->Set(context, i, Integer::New(isolate, cells.cell[i]));
	}
	obj->Set(context, String::NewFromUtf8(isolate, "cell").ToLocalChecked(), arr);

	info.GetReturnValue().Set(obj);
}

void GetMapCollisionTable(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be boolean.");
		return;
	}
	
	bool loadall = info[0]->BooleanValue(isolate);

	CGA::cga_map_cells_t cells;
	if (!g_CGAInterface->GetMapCollisionTable(loadall, cells))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "x_bottom").ToLocalChecked(), Integer::New(isolate, cells.x_bottom));
	obj->Set(context, String::NewFromUtf8(isolate, "y_bottom").ToLocalChecked(), Integer::New(isolate, cells.y_bottom));
	obj->Set(context, String::NewFromUtf8(isolate, "x_size").ToLocalChecked(), Integer::New(isolate, cells.x_size));
	obj->Set(context, String::NewFromUtf8(isolate, "y_size").ToLocalChecked(), Integer::New(isolate, cells.y_size));
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < cells.cell.size(); ++i)
	{
		arr->Set(context, i, Integer::New(isolate, cells.cell[i]));
	}
	obj->Set(context, String::NewFromUtf8(isolate, "cell").ToLocalChecked(), arr);

	info.GetReturnValue().Set(obj);
}

void GetMapObjectTable(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[0] must be boolean.");
		return;
	}
	
	bool loadall = info[0]->BooleanValue(isolate);

	CGA::cga_map_cells_t cells;
	if (!g_CGAInterface->GetMapObjectTable(loadall, cells))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "x_bottom").ToLocalChecked(), Integer::New(isolate, cells.x_bottom));
	obj->Set(context, String::NewFromUtf8(isolate, "y_bottom").ToLocalChecked(), Integer::New(isolate, cells.y_bottom));
	obj->Set(context, String::NewFromUtf8(isolate, "x_size").ToLocalChecked(), Integer::New(isolate, cells.x_size));
	obj->Set(context, String::NewFromUtf8(isolate, "y_size").ToLocalChecked(), Integer::New(isolate, cells.y_size));
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < cells.cell.size(); ++i)
	{
		arr->Set(context, i, Integer::New(isolate, cells.cell[i]));
	}
	obj->Set(context, String::NewFromUtf8(isolate, "cell").ToLocalChecked(), arr);

	info.GetReturnValue().Set(obj);
}

void GetMapTileTable(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[0] must be boolean.");
		return;
	}
	
	bool loadall = info[0]->BooleanValue(isolate);

	CGA::cga_map_cells_t cells;
	if (!g_CGAInterface->GetMapTileTable(loadall, cells))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "x_bottom").ToLocalChecked(), Integer::New(isolate, cells.x_bottom));
	obj->Set(context, String::NewFromUtf8(isolate, "y_bottom").ToLocalChecked(), Integer::New(isolate, cells.y_bottom));
	obj->Set(context, String::NewFromUtf8(isolate, "x_size").ToLocalChecked(), Integer::New(isolate, cells.x_size));
	obj->Set(context, String::NewFromUtf8(isolate, "y_size").ToLocalChecked(), Integer::New(isolate, cells.y_size));
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < cells.cell.size(); ++i)
	{
		arr->Set(context, i, Integer::New(isolate, cells.cell[i]));
	}
	obj->Set(context, String::NewFromUtf8(isolate, "cell").ToLocalChecked(), arr);

	info.GetReturnValue().Set(obj);
}

void WalkTo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be integer.");
		return;
	}

	int x = info[0]->Int32Value(context).ToChecked();
	int y = info[1]->Int32Value(context).ToChecked();

	if (!g_CGAInterface->WalkTo(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void TurnTo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be integer.");
		return;
	}

	int x = info[0]->Int32Value(context).ToChecked();
	int y = info[1]->Int32Value(context).ToChecked();

	if (!g_CGAInterface->TurnTo(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void IsMapCellPassable(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be integer.");
		return;
	}

	int x = info[0]->Int32Value(context).ToChecked();
	int y = info[1]->Int32Value(context).ToChecked();
	bool bResult = false;
	if (!g_CGAInterface->IsMapCellPassable(x, y, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void ForceMove(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	if (info.Length() < 2 || !info[1]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[1] must be boolean.");
		return;
	}

	int dir = info[0]->Int32Value(context).ToChecked();
	bool show = info[1]->BooleanValue(isolate);
	bool bResult = false;
	if (!g_CGAInterface->ForceMove(dir, show, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void ForceMoveTo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be integer.");
		return;
	}
	if (info.Length() < 3 || !info[2]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[2] must be boolean.");
		return;
	}

	int x = info[0]->Int32Value(context).ToChecked();
	int y = info[1]->Int32Value(context).ToChecked();
	bool show = info[2]->BooleanValue(isolate);
	bool bResult = false;
	if (!g_CGAInterface->ForceMoveTo(x, y, show, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

class WalkToWorkerData
{
public:
	WalkToWorkerData(int x, int y)
	{
		m_x = x;
		m_y = y;
		m_fx = x * 64.0f;
		m_fy = y * 64.0f;
		m_dfx = x * 64.0f;
		m_dfy = y * 64.0f;
		m_mapindex = 0;
		m_waitmapname = false;
		m_waitmapindex = false;
		m_waitxy = false;
		m_result = false;		
		m_reason = 0;
		m_worker.data = this;
	}

	bool m_result;
	int m_reason;
	int m_x, m_y;
	float m_fx, m_fy;
	float m_dfx, m_dfy;
	std::string m_map;
	int m_mapindex;
	bool m_waitmapname;
	bool m_waitmapindex;
	bool m_waitxy;
	Persistent<Function> m_callback;
	uv_work_t m_worker;
};

void WalkToWorker(uv_work_t* req)
{
	auto data = (WalkToWorkerData *)req->data;

	std::string initmap, curmap;

	uint64_t lastStay = 0;
	uint64_t lastStay2 = 0;
	bool prevEnableWarpState = true;
	float lastX = 0, lastY = 0;
	float fx = 0, fy = 0;
	
	if (!g_CGAInterface->GetMapName(initmap)) {
		data->m_reason = -1;
		return;
	}

	int initmap_index1 = 0, initmap_index2 = 0, initmap_index3 = 0;
	int curmap_index1 = 0, curmap_index2 = 0, curmap_index3 = 0;

	if (!g_CGAInterface->GetMapIndex(initmap_index1, initmap_index2, initmap_index3)) {
		data->m_reason = -1;
		return;
	}

	while (1)
	{
		float sx, sy;
		if (!g_CGAInterface->GetMoveSpeed(sx, sy)) {
			data->m_reason = -2;
			return;
		}

		int gameStatus = 0;
		if (!g_CGAInterface->GetGameStatus(gameStatus)) {
			data->m_reason = -3;
			return;
		}

		int worldStatus = 0;
		if (!g_CGAInterface->GetWorldStatus(worldStatus)) {
			data->m_reason = -4;
			return;
		}

		//We are in battle status, quit and let script walk the last one
		if (worldStatus == 10) {
			data->m_reason = 2;
			return;
		}

		//Position are syncronized by server
		if (g_PlayerSyncPosition) {
			g_PlayerSyncPosition = false;
			data->m_reason = 5;
			return;
		}

		if(data->m_waitmapname || data->m_waitmapindex)
		{
			//enable map warp entrance so that we can get in it
			if (!prevEnableWarpState && !g_CGAInterface->FixMapWarpStuck(2)) {
				data->m_reason = -8;
				return;
			}
			prevEnableWarpState = true;
		}
		else
		{
			//disable map warp entrance so that we will never get in wrong map
			if (prevEnableWarpState && !g_CGAInterface->FixMapWarpStuck(3)) {
				data->m_reason = -8;
				return;
			}
			prevEnableWarpState = false;
		}
		
		//We are switching map
		if (gameStatus != 3) {
			Sleep(100);
			continue;
		}
		
		if (worldStatus == 9 && gameStatus == 3 && sx == 0.0f && sy == 0.0f)
		{
			if (!g_CGAInterface->GetMapXYFloat(fx, fy)) {
				data->m_reason = -5;
				return;
			}
			
			if (!g_CGAInterface->GetMapName(curmap)) {
				data->m_reason = -1;
				return;
			}

			if (!g_CGAInterface->GetMapIndex(curmap_index1, curmap_index2, curmap_index3)) {
				data->m_reason = -1;
				return;
			}

			bool waiting_for_new_map = false;

			if(data->m_waitmapname)
			{
				waiting_for_new_map = true;
				if (!data->m_map.empty() && curmap == data->m_map && !data->m_waitxy)
				{
					data->m_result = true;
					return;
				}
				//for empty mapname, any new map is ok
				else if (data->m_map.empty() && !(curmap_index3 == initmap_index3))
				{
					data->m_result = true;
					return;
				}
			}
			else if (data->m_waitmapindex)
			{
				waiting_for_new_map = true;
				if (curmap_index3 == data->m_mapindex)
				{
					data->m_result = true;
					return;
				}
			}

			//we are not waiting for any new map, but the map has been changed, wtf...
			if (!(curmap_index1 == initmap_index1 && curmap_index2 == initmap_index2 && curmap_index3 == initmap_index3))
			{
				data->m_reason = 4;
				return;
			}

			if (fabs(fx - data->m_dfx) < 0.001f && fabs(fy - data->m_dfy) < 0.001f && (!waiting_for_new_map || data->m_waitxy))
			{
				data->m_result = true;
				return;
			}
			if (!g_CGAInterface->WalkTo(data->m_x, data->m_y)) {
				data->m_reason = -6;
				return;
			}
			auto cur = uv_hrtime();
			if(fx != lastX || fy != lastY)
			{
				lastX = fx;
				lastY = fy;
				lastStay = cur;
				lastStay2 = cur;
			}
			else if (lastStay2 > 0 && cur - lastStay2 > 30000 * NANO_SECONDS_TO_MS)
			{
				data->m_reason = 3;
				return;
			}
			else if(lastStay > 0 && cur - lastStay > 5000 * NANO_SECONDS_TO_MS)
			{
				lastStay = cur;
				if (!g_CGAInterface->FixMapWarpStuck(1)) {
					data->m_reason = -7;
					return;
				}
			}
		}

		Sleep(100);
	}
}

void WalkToAfterWorker(uv_work_t* req, int status)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	//enable map warp entrance, that is, restore it to default...
	g_CGAInterface->FixMapWarpStuck(2);

	auto data = (WalkToWorkerData *)req->data;

	auto reasonString = "Unknown exception.";

	if (data->m_reason == 2)
		reasonString = "Battle status.";
	else if (data->m_reason == 3)
		reasonString = "Stucked for over 30s.";
	else if (data->m_reason == 4)
		reasonString = "Unexcepted map changed.";
	else if (data->m_reason == 5)
		reasonString = "Position are syncronized by server.";

	Local<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error(reasonString);
	argv[1] = Integer::New(isolate, data->m_reason);

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 2, argv);
	data->m_callback.Reset();

	delete data;
}

void AsyncWalkTo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be integer.");
		return;
	}
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Arg[2] must be string or array(integer) or null.");
		return;
	}
	if (info.Length() < 4) {
		Nan::ThrowTypeError("Arg[3] must be desired target x or null .");
		return;
	}
	if (info.Length() < 5) {
		Nan::ThrowTypeError("Arg[4] must be desired target x or null.");
		return;
	}
	if (info.Length() < 6 || !info[5]->IsFunction()) {
		Nan::ThrowTypeError("Arg[5] must be a function.");
		return;
	}
	int x = info[0]->Int32Value(context).ToChecked();
	int y = info[1]->Int32Value(context).ToChecked();

	auto callback = Local<Function>::Cast(info[5]);

	auto data = new WalkToWorkerData(x, y);
	
	if(info[2]->IsString())
	{
		v8::String::Utf8Value str(isolate, info[2]->ToString(context).ToLocalChecked());
		data->m_map = std::string(*str);
		data->m_waitmapname = true;
	}
	else if(info[2]->IsInt32())
	{
		data->m_mapindex = info[2]->Int32Value(context).ToChecked();
		data->m_waitmapindex = true;
	}
	if(info[3]->IsInt32() && info[4]->IsInt32())
	{
		data->m_dfx = info[3]->Int32Value(context).ToChecked() * 64.0f;
		data->m_dfy = info[4]->Int32Value(context).ToChecked() * 64.0f;
		data->m_waitxy = true;
	}

	data->m_callback.Reset(isolate, callback);

	uv_queue_work(uv_default_loop(), &data->m_worker, WalkToWorker, WalkToAfterWorker);	
}

class WaitMoveWorkerData
{
public:
	WaitMoveWorkerData()
	{
		m_waitmapname = false;
		m_waitmapindex = false;
		m_waitxy = false;
		
		m_fx = 0;
		m_fy = 0;
		m_timestart = 0;
		m_timeout = 0;
		m_delay = 0;
		m_result = false;
		m_reason = 0;
		m_worker.data = this;
	}
	void SetMapXY(int x, int y)
	{
		m_fx = x * 64.0f;
		m_fy = y * 64.0f;
		m_waitxy = true;
	}

	void AddMapName(const std::string &mapname)
	{
		m_mapnames.push_back(mapname);
		m_waitmapname = true;
	}
	
	void AddMapIndex(int mapindex)
	{
		m_mapindices.push_back(mapindex);
		m_waitmapindex = true;
	}

	void SetTimeout(int ms)
	{
		m_timeout = ms;
	}

	void SetDelay(int ms)
	{
		m_delay = ms;
	}

	int m_reason;
	bool m_result;
	bool m_waitmapname;
	bool m_waitmapindex;
	bool m_waitxy;
	uint64_t m_timestart;
	int m_timeout, m_delay;
	float m_fx, m_fy;
	std::vector<std::string> m_mapnames;
	std::vector<int> m_mapindices;
	Persistent<Function> m_callback;
	uv_work_t m_worker;
};

void WaitMoveWorker(uv_work_t* req)
{
	auto data = (WaitMoveWorkerData *)req->data;

	float fx = 0, fy = 0;
	std::string curmap;
	int curmap_index1, curmap_index2, curmap_index3;

	data->m_timestart = uv_hrtime();

	if(data->m_delay > 0)
		Sleep(data->m_delay);

	while (1)
	{
		float sx, sy;
		if (!g_CGAInterface->GetMoveSpeed(sx, sy)) {
			data->m_reason = -5;
			return;
		}
		
		int worldStatus = 0;
		if (!g_CGAInterface->GetWorldStatus(worldStatus)) {
			data->m_reason = -6;
			return;
		}

		int gameStatus = 0;
		if (!g_CGAInterface->GetGameStatus(gameStatus)) {
			data->m_reason = -7;
			return;
		}

		if (worldStatus == 10) {
			data->m_reason = 2;
			return;
		}

		//Position are syncronized by server
		if (g_PlayerSyncPosition) {
			g_PlayerSyncPosition = false;
			data->m_reason = 5;
			return;
		}
		
		if (worldStatus == 9 && gameStatus == 3 && sx == 0.0f && sy == 0.0f)
		{
			if (!g_CGAInterface->GetMapXYFloat(fx, fy)) {
				data->m_reason = -5;
				return;
			}
			
			if (!g_CGAInterface->GetMapName(curmap)) {
				data->m_reason = -1;
				return;
			}

			if (!g_CGAInterface->GetMapIndex(curmap_index1, curmap_index2, curmap_index3)) {
				data->m_reason = -1;
				return;
			}
			
			if(data->m_waitmapname)
			{
				for (size_t j = 0; j < data->m_mapnames.size(); ++j)
				{
					if (data->m_mapnames[j] == curmap)
					{
						data->m_result = true;
						return;
					}
				}
			}
			else if (data->m_waitmapindex)
			{
				for (size_t j = 0; j < data->m_mapindices.size(); ++j)
				{
					if (data->m_mapindices[j] == curmap_index3)
					{
						data->m_result = true;
						return;
					}
				}
			}
			else if (data->m_waitxy)
			{
				if (fabs(fx - data->m_fx) < 0.001f && fabs(fy - data->m_fy) < 0.001f)
				{
					data->m_result = true;
					return;
				}
			}
		}

		auto curtime = uv_hrtime();

		if (data->m_timeout > 0 && curtime > data->m_timestart + (data->m_timeout * NANO_SECONDS_TO_MS)) {
			data->m_reason = 3;
			return;
		}

		Sleep(100);
	}
}

void WaitMoveAfterWorker(uv_work_t* req, int status)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (WaitMoveWorkerData *)req->data;

	auto reasonString = "Unknown exception.";

	if (data->m_reason == 2)
		reasonString = "Battle status.";
	else if (data->m_reason == 3)
		reasonString = "Async callback timeout.";
	else if (data->m_reason == 4)
		reasonString = "Unexcepted map changed.";
	else if (data->m_reason == 5)
		reasonString = "Position are syncronized by server.";

	Local<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error(reasonString);
	argv[1] = Integer::New(isolate, data->m_reason);

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 2, argv);
	data->m_callback.Reset();

	delete data;
}

void AsyncWaitMovement(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsObject()) {
		Nan::ThrowTypeError("Arg[0] must be object.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsFunction()) {
		Nan::ThrowTypeError("Arg[1] must be function.");
		return;
	}

	Local<Object> obj = Local<Object>::Cast(info[0]);
	
	auto callback = Local<Function>::Cast(info[1]);

	auto data = new WaitMoveWorkerData();
	data->m_callback.Reset(isolate, callback);

	auto v_map = obj->Get(context, String::NewFromUtf8(isolate, "map").ToLocalChecked());
	if (!v_map.IsEmpty() && v_map.ToLocalChecked()->IsArray())
	{
		Local<Array> arr = Local<Array>::Cast(v_map.ToLocalChecked());
		for (uint32_t i = 0; i < arr->Length(); ++i)
		{
			auto v_map_name = arr->Get(context, i);
			if (!v_map_name.IsEmpty() && v_map_name.ToLocalChecked()->IsString())
			{
				v8::String::Utf8Value str(isolate, v_map_name.ToLocalChecked()->ToString(context).ToLocalChecked());
				std::string mapname(*str);
				data->AddMapName(mapname);
			}
			else if (!v_map_name.IsEmpty() && v_map_name.ToLocalChecked()->IsInt32())
			{
				int mapindex = v_map_name.ToLocalChecked()->Int32Value(context).ToChecked();
				data->AddMapIndex(mapindex);
			}
		}
	}
	else if (!v_map.IsEmpty() && v_map.ToLocalChecked()->IsString())
	{
		v8::String::Utf8Value str(isolate, v_map.ToLocalChecked()->ToString(context).ToLocalChecked());
		std::string mapname(*str);
		data->AddMapName(mapname);
	}
	else if (!v_map.IsEmpty() && v_map.ToLocalChecked()->IsInt32()) 
	{
		int mapindex = v_map.ToLocalChecked()->Int32Value(context).ToChecked();
		data->AddMapIndex(mapindex);
	}
			
	auto v_x = obj->Get(context, String::NewFromUtf8(isolate, "x").ToLocalChecked());
	auto v_y = obj->Get(context, String::NewFromUtf8(isolate, "y").ToLocalChecked());
	if (!v_x.IsEmpty() && v_x.ToLocalChecked()->IsInt32())
	{
		if (!v_y.IsEmpty() && v_y.ToLocalChecked()->IsInt32())
		{
			int x = v_x.ToLocalChecked()->Int32Value(context).ToChecked();
			int y = v_y.ToLocalChecked()->Int32Value(context).ToChecked();
			data->SetMapXY(x, y);
		}
	}

	auto v_delay = obj->Get(context, String::NewFromUtf8(isolate, "delay").ToLocalChecked());
	if (!v_delay.IsEmpty() && v_delay.ToLocalChecked()->IsInt32())
	{
		int delay = v_delay.ToLocalChecked()->Int32Value(context).ToChecked();
		data->SetDelay(delay);
	}

	auto v_timeout = obj->Get(context, String::NewFromUtf8(isolate, "timeout").ToLocalChecked());
	if (!v_timeout.IsEmpty() && v_timeout.ToLocalChecked()->IsInt32())
	{
		int timeout = v_timeout.ToLocalChecked()->Int32Value(context).ToChecked();
		data->SetTimeout(timeout);
	}

	uv_queue_work(uv_default_loop(), &data->m_worker, WaitMoveWorker, WaitMoveAfterWorker);
}