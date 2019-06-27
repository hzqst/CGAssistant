#include <time.h>
#include <nan.h>
#include "../CGALib/gameinterface.h"

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

void RequestDownloadMap(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be xbottom.");
		return;
	}

	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be ybottom.");
		return;
	}
	
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Arg[2] must be xsize.");
		return;
	}
	
	if (info.Length() < 4) {
		Nan::ThrowTypeError("Arg[3] must be ysize.");
		return;
	}

	int xbottom = (int)info[0]->IntegerValue();
	int ybottom = (int)info[1]->IntegerValue();
	int xsize = (int)info[2]->IntegerValue();
	int ysize = (int)info[3]->IntegerValue();

	if (!g_CGAInterface->RequestDownloadMap(xbottom, ybottom, xsize, ysize))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void FixMapWarpStuck(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be type.");
		return;
	}

	int type = (int)info[0]->IntegerValue();

	if (!g_CGAInterface->FixMapWarpStuck(type))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void GetMapIndex(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	int index1, index2, index3;
	if (!g_CGAInterface->GetMapIndex(index1, index2, index3))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "index1"), Integer::New(isolate, index1));
	obj->Set(String::NewFromUtf8(isolate, "index2"), Integer::New(isolate, index2));
	obj->Set(String::NewFromUtf8(isolate, "index3"), Integer::New(isolate, index3));
	info.GetReturnValue().Set(obj);
}

void GetMapXY(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	int x, y;
	if (!g_CGAInterface->GetMapXY(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "x"), Integer::New(isolate, x));
	obj->Set(String::NewFromUtf8(isolate, "y"), Integer::New(isolate, y));
	info.GetReturnValue().Set(obj);
}

void GetMapXYFloat(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	float x, y;
	if (!g_CGAInterface->GetMapXYFloat(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "x"), Number::New(isolate, x));
	obj->Set(String::NewFromUtf8(isolate, "y"), Number::New(isolate, y));
	info.GetReturnValue().Set(obj);
}

void GetMoveSpeed(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	float x, y;
	if (!g_CGAInterface->GetMoveSpeed(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "x"), Number::New(isolate, x));
	obj->Set(String::NewFromUtf8(isolate, "y"), Number::New(isolate, y));
	info.GetReturnValue().Set(obj);
}

void GetMapName(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

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
		obj->Set(String::NewFromUtf8(isolate, "valid"), Integer::New(isolate, units[i].valid));
		obj->Set(String::NewFromUtf8(isolate, "type"), Integer::New(isolate, units[i].type));
		obj->Set(String::NewFromUtf8(isolate, "model_id"), Integer::New(isolate, units[i].model_id));
		obj->Set(String::NewFromUtf8(isolate, "unit_id"), Integer::New(isolate, units[i].unit_id));
		obj->Set(String::NewFromUtf8(isolate, "xpos"), Integer::New(isolate, units[i].xpos));
		obj->Set(String::NewFromUtf8(isolate, "ypos"), Integer::New(isolate, units[i].ypos));
		obj->Set(String::NewFromUtf8(isolate, "item_count"), Integer::New(isolate, units[i].item_count));
		obj->Set(String::NewFromUtf8(isolate, "injury"), Integer::New(isolate, units[i].injury));
		obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, units[i].level));
		obj->Set(String::NewFromUtf8(isolate, "flags"), Integer::New(isolate, units[i].flags));
		obj->Set(String::NewFromUtf8(isolate, "unit_name"), Nan::New(units[i].unit_name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "nick_name"), Nan::New(units[i].nick_name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "title_name"), Nan::New(units[i].title_name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "item_name"), Nan::New(units[i].item_name).ToLocalChecked());
		arr->Set(i, obj);
	}

	info.GetReturnValue().Set(arr);
}

void GetMapCollisionTable(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be boolean.");
		return;
	}
	
	bool loadall = info[0]->BooleanValue();

	CGA::cga_map_cells_t cells;
	if (!g_CGAInterface->GetMapCollisionTable(loadall, cells))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "x_bottom"), Integer::New(isolate, cells.x_bottom));
	obj->Set(String::NewFromUtf8(isolate, "y_bottom"), Integer::New(isolate, cells.y_bottom));
	obj->Set(String::NewFromUtf8(isolate, "x_size"), Integer::New(isolate, cells.x_size));
	obj->Set(String::NewFromUtf8(isolate, "y_size"), Integer::New(isolate, cells.y_size));
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < cells.cell.size(); ++i)
	{
		arr->Set(i, Integer::New(isolate, cells.cell[i]));
	}
	obj->Set(String::NewFromUtf8(isolate, "cell"), arr);

	info.GetReturnValue().Set(obj);
}

void GetMapObjectTable(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be boolean.");
		return;
	}
	
	bool loadall = info[0]->BooleanValue();

	CGA::cga_map_cells_t cells;
	if (!g_CGAInterface->GetMapObjectTable(loadall, cells))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "x_bottom"), Integer::New(isolate, cells.x_bottom));
	obj->Set(String::NewFromUtf8(isolate, "y_bottom"), Integer::New(isolate, cells.y_bottom));
	obj->Set(String::NewFromUtf8(isolate, "x_size"), Integer::New(isolate, cells.x_size));
	obj->Set(String::NewFromUtf8(isolate, "y_size"), Integer::New(isolate, cells.y_size));
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < cells.cell.size(); ++i)
	{
		arr->Set(i, Integer::New(isolate, cells.cell[i]));
	}
	obj->Set(String::NewFromUtf8(isolate, "cell"), arr);

	info.GetReturnValue().Set(obj);
}

void GetMoveHistory(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	std::vector<DWORD> v;
	if (!g_CGAInterface->GetMoveHistory(v))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < v.size(); ++i)
	{
		Local<Array> arr2 = Array::New(isolate);
		arr2->Set(0, Integer::New(isolate, (v[i] >> 16) & 0xFFFF ));
		arr2->Set(1, Integer::New(isolate, v[i] & 0xFFFF));
		arr->Set(i, arr2);
	}

	info.GetReturnValue().Set(arr);
}

void WalkTo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be x.");
		return;
	}
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be y.");
		return;
	}

	int x = (int)info[0]->IntegerValue();
	int y = (int)info[1]->IntegerValue();

	if (!g_CGAInterface->WalkTo(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void TurnTo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be x.");
		return;
	}
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be y.");
		return;
	}

	int x = (int)info[0]->IntegerValue();
	int y = (int)info[1]->IntegerValue();

	if (!g_CGAInterface->TurnTo(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void IsMapCellPassable(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || info[0]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[0] must be x.");
		return;
	}
	if (info.Length() < 2 || info[1]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[1] must be y.");
		return;
	}

	int x = (int)info[0]->IntegerValue();
	int y = (int)info[1]->IntegerValue();
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || info[0]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[0] must be dir.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[1] must be boolean.");
		return;
	}

	int dir = (int)info[0]->IntegerValue();
	bool show = info[1]->BooleanValue();
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || info[0]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[0] must be x.");
		return;
	}
	if (info.Length() < 2 || info[1]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[0] must be y.");
		return;
	}
	if (info.Length() < 3 || !info[2]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[1] must be boolean.");
		return;
	}

	int x = (int)info[0]->IntegerValue();
	int y = (int)info[1]->IntegerValue();
	bool show = info[2]->BooleanValue();
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

	__time64_t lastStay = 0;
	__time64_t lastStay2 = 0;
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
			auto cur = _time64(NULL);
			if(fx != lastX || fy != lastY)
			{
				lastX = fx;
				lastY = fy;
				lastStay = cur;
				lastStay2 = cur;
			}
			else if (lastStay2 > 0 && cur - lastStay2 > 30)
			{
				data->m_reason = 3;
				return;
			}
			else if(lastStay > 0 && cur - lastStay > 5)
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
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	//enable map warp entrance, that is, restore it to default...
	g_CGAInterface->FixMapWarpStuck(2);

	auto data = (WalkToWorkerData *)req->data;

	Handle<Value> argv[2];
	argv[0] = Boolean::New(isolate, data->m_result);
	argv[1] = Integer::New(isolate, data->m_reason);

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 2, argv);
	data->m_callback.Reset();

	delete data;
}

void AsyncWalkTo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be x.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be y.");
		return;
	}
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Arg[2] must be desired map name(string) or mapindex (array) or null.");
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
	int x = (int)info[0]->IntegerValue();
	int y = (int)info[1]->IntegerValue();

	auto callback = Local<Function>::Cast(info[5]);

	auto data = new WalkToWorkerData(x, y);
	
	if(info[2]->IsString())
	{
		v8::String::Utf8Value str(info[2]->ToString());
		data->m_map = std::string(*str);
		data->m_waitmapname = true;
	}
	else if(info[2]->IsInt32())
	{
		data->m_mapindex = (int)info[2]->IntegerValue();
		data->m_waitmapindex = true;
	}
	if(info[3]->IsInt32() && info[4]->IsInt32())
	{
		data->m_dfx = (int)info[3]->IntegerValue() * 64.0f;
		data->m_dfy = (int)info[4]->IntegerValue() * 64.0f;
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

	bool m_result;
	int m_reason;
	bool m_waitmapname;
	bool m_waitmapindex;
	bool m_waitxy;
	__time64_t m_timestart;
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

	data->m_timestart = _time64(NULL);

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

		auto curtime = _time64(NULL);

		if (data->m_timeout > 0 && curtime > data->m_timestart + data->m_timeout) {
			data->m_reason = 3;
			return;
		}

		Sleep(100);
	}
}

void WaitMoveAfterWorker(uv_work_t* req, int status)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (WaitMoveWorkerData *)req->data;

	Handle<Value> argv[2];
	argv[0] = Boolean::New(isolate, data->m_result);
	argv[1] = Integer::New(isolate, data->m_reason);

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 2, argv);
	data->m_callback.Reset();

	delete data;
}

void AsyncWaitMovement(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsObject()) {
		Nan::ThrowTypeError("Arg[0] must be an object.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsFunction()) {
		Nan::ThrowTypeError("Arg[1] must be a function.");
		return;
	}

	Local<Object> obj = Local<Object>::Cast(info[0]);
	
	auto callback = Local<Function>::Cast(info[1]);

	auto data = new WaitMoveWorkerData();
	data->m_callback.Reset(isolate, callback);

	Local<Value> v_map = obj->Get(String::NewFromUtf8(isolate, "map"));
	if (v_map->IsArray())
	{
		Local<Array> arr = Local<Array>::Cast(v_map);
		for (uint32_t i = 0; i < arr->Length(); ++i)
		{
			Local<Value> v_map_name = arr->Get(i);
			if (v_map_name->IsString()) {
				v8::String::Utf8Value str(v_map_name->ToString());
				std::string mapname(*str);
				data->AddMapName(mapname);
			}
			else if (v_map_name->IsInt32()) {
				int mapindex  = (int)v_map_name->IntegerValue();
				data->AddMapIndex(mapindex);
			}
		}
	}
	else if (v_map->IsString())
	{
		v8::String::Utf8Value str(v_map->ToString());
		std::string mapname(*str);
		data->AddMapName(mapname);
	}
	else if (v_map->IsInt32()) {
		int mapindex  = (int)v_map->IntegerValue();
		data->AddMapIndex(mapindex);
	}
			
	Local<Value> v_x = obj->Get(String::NewFromUtf8(isolate, "x"));
	Local<Value> v_y = obj->Get(String::NewFromUtf8(isolate, "y"));
	if (v_x->IsInt32() && v_y->IsInt32())
	{
		int x = (int)v_x->IntegerValue();
		int y = (int)v_y->IntegerValue();
		data->SetMapXY(x, y);
	}

	Local<Value> v_delay = obj->Get(String::NewFromUtf8(isolate, "delay"));
	if (v_delay->IsInt32())
		data->SetDelay((int)v_delay->IntegerValue());

	Local<Value> v_timeout = obj->Get(String::NewFromUtf8(isolate, "timeout"));
	if (v_timeout->IsInt32())
		data->SetTimeout((int)v_timeout->IntegerValue());

	uv_queue_work(uv_default_loop(), &data->m_worker, WaitMoveWorker, WaitMoveAfterWorker);
}

void GetTeamPlayerInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	CGA::cga_team_players_t plinfo;
	if (!g_CGAInterface->GetTeamPlayerInfo(plinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < plinfo.size(); ++i)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(String::NewFromUtf8(isolate, "unit_id"), Integer::New(isolate, plinfo[i].unit_id));
		obj->Set(String::NewFromUtf8(isolate, "hp"), Integer::New(isolate, plinfo[i].hp));
		obj->Set(String::NewFromUtf8(isolate, "mp"), Integer::New(isolate, plinfo[i].mp));
		obj->Set(String::NewFromUtf8(isolate, "maxhp"), Integer::New(isolate, plinfo[i].maxhp));
		arr->Set(i, obj);
	}

	info.GetReturnValue().Set(arr);
}