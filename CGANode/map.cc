#include <nan.h>
#include "../CGALib/gameinterface.h"

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

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
		obj->Set(String::NewFromUtf8(isolate, "unit_name"), Nan::New(units[i].unit_name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "nick_name"), Nan::New(units[i].nick_name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "item_name"), Nan::New(units[i].item_name).ToLocalChecked());
		arr->Set(i, obj);
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
	if (info.Length() < 2 || !info[1]->IsUndefined()) {
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
		m_result = false;
		m_reason = 0;
		m_worker.data = this;
	}

	bool m_result;
	int m_reason;
	int m_x, m_y;
	float m_fx, m_fy;
	Persistent<Function> m_callback;
	uv_work_t m_worker;
};

void WalkToWorker(uv_work_t* req)
{
	auto data = (WalkToWorkerData *)req->data;

	std::string map, curmap;
	if (!g_CGAInterface->GetMapName(map))
		return;

	while (1)
	{
		if (!g_CGAInterface->GetMapName(curmap)) {
			data->m_reason = -1;
			return;
		}

		if (curmap != map)
		{
			data->m_result = true;
			return;
		}

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

		float fx, fy;
		if (!g_CGAInterface->GetMapXYFloat(fx, fy)) {
			data->m_reason = -5;
			return;
		}

		if (worldStatus == 9 && gameStatus == 3)
		{
			if (sx == 0.0f && sy == 0.0f && fabs(fx - data->m_fx) < 0.001f && fabs(fy - data->m_fy) < 0.001f)
			{
				data->m_result = true;
				return;
			}
			if (!g_CGAInterface->WalkTo(data->m_x, data->m_y)) {
				data->m_reason = -6;
				return;
			}
		}

		Sleep(100);
	}
}

void WalkToAfterWorker(uv_work_t* req, int status)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

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

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be x.");
		return;
	}
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be y.");
		return;
	}
	if (info.Length() < 3 || !info[2]->IsFunction()) {
		Nan::ThrowTypeError("Arg[2] must be a function.");
		return;
	}
	int x = (int)info[0]->IntegerValue();
	int y = (int)info[1]->IntegerValue();

	if (!g_CGAInterface->WalkTo(x, y))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	auto callback = Local<Function>::Cast(info[2]);

	auto data = new WalkToWorkerData(x, y);
	data->m_callback.Reset(isolate, callback);

	uv_queue_work(uv_default_loop(), &data->m_worker, WalkToWorker, WalkToAfterWorker);	
}

class WaitMoveWorkerData
{
public:
	WaitMoveWorkerData()
	{
		m_flags = 0;
		m_fx = 0;
		m_fy = 0;
		m_retry = 0;
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
		m_flags |= 2;
	}

	void AddMapName(const std::string &mapname)
	{
		m_mapnames.push_back(mapname);
		m_flags |= 1;
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
	int m_retry;
	int m_reason;
	int m_flags;
	int m_timeout, m_delay;
	float m_fx, m_fy;
	std::vector<std::string> m_mapnames;
	Persistent<Function> m_callback;
	uv_work_t m_worker;
};

void WaitMoveWorker(uv_work_t* req)
{
	auto data = (WaitMoveWorkerData *)req->data;

	std::string map, curmap;
	if (!g_CGAInterface->GetMapName(map)) {
		data->m_reason = -1;
		return;
	}

	float curfx, curfy;
	if (!g_CGAInterface->GetMapXYFloat(curfx, curfy)) {
		data->m_reason = -2;
		return;
	}

	if(data->m_delay > 0)
		Sleep(data->m_delay);

	while (1)
	{
		int flags = 0;
		if (!g_CGAInterface->GetMapName(curmap)) {
			data->m_reason = -3;
			return;
		}

		if (data->m_flags & 1)
		{
			for (size_t j = 0; j < data->m_mapnames.size(); ++j)
			{
				if (data->m_mapnames[j] == curmap)
				{
					flags |= 1;
					break;
				}
			}
		}
		else
		{
			if (curmap != map)
				flags |= 1;
		}

		float fx, fy;
		if (!g_CGAInterface->GetMapXYFloat(fx, fy)) {
			data->m_reason = -4;
			return;
		}

		if (data->m_flags & 2)
		{
			if (fabs(fx - data->m_fx) < 0.01f && fabs(fy - data->m_fy) < 0.01f)
				flags |= 2;
		}
		else
		{
			if (curfx != fx || curfy != fy)
				flags |= 2;
		}

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
			if ((data->m_flags > 0 && (flags & data->m_flags) == data->m_flags))
			{
				data->m_result = true;
				return;
			}
			else if (data->m_flags == 0 && flags > 0) 
			{
				data->m_result = true;
				return;
			}
		}

		data->m_retry++;
		if (data->m_timeout > 0 && data->m_retry * 100 > data->m_timeout) {
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
	
	int delay = 0, timeout = 0;

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
		}
	}

	Local<Value> v_x = obj->Get(String::NewFromUtf8(isolate, "x"));
	Local<Value> v_y = obj->Get(String::NewFromUtf8(isolate, "y"));
	if (!v_x->IsUndefined() && !v_x->IsUndefined())
	{
		int x = (int)v_x->IntegerValue();
		int y = (int)v_y->IntegerValue();
		data->SetMapXY(x, y);
	}

	Local<Value> v_delay = obj->Get(String::NewFromUtf8(isolate, "delay"));
	if (!v_delay->IsUndefined())
		data->SetDelay((int)v_delay->IntegerValue());

	Local<Value> v_timeout = obj->Get(String::NewFromUtf8(isolate, "timeout"));
	if (!v_timeout->IsUndefined())
		data->SetTimeout((int)v_timeout->IntegerValue());

	uv_queue_work(uv_default_loop(), &data->m_worker, WaitMoveWorker, WaitMoveAfterWorker);
}