#include <nan.h>
#include "../CGALib/gameinterface.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

#include "async.h"

void WorkingResultAsyncCallBack(uv_async_t *handle);

class WorkingResultNotifyData : public CBaseNotifyData
{
public:
	WorkingResultNotifyData() : CBaseNotifyData(WorkingResultAsyncCallBack)
	{
	}

	CGA::cga_working_result_t m_results;
};

class WorkingResultCacheData
{
public:
	WorkingResultCacheData(const CGA::cga_working_result_t &result, uint64_t time) : m_result(result), m_time(time)
	{

	}
	CGA::cga_working_result_t m_result;
	uint64_t m_time;
};

boost::shared_mutex g_WorkingResult_Lock;
std::vector<WorkingResultNotifyData *> g_WorkingResult_Asyncs;
std::vector<WorkingResultCacheData *> g_WorkingResult_Caches;

void WorkingResultDoAsyncCall(const CGA::cga_working_result_t &results)
{
	if (g_WorkingResult_Asyncs.size())
	{
		for (size_t i = 0; i < g_WorkingResult_Asyncs.size(); ++i)
		{
			const auto data = g_WorkingResult_Asyncs[i];

			data->m_results = results;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_WorkingResult_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_WorkingResult_Caches.begin(); p != g_WorkingResult_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_WorkingResult_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_WorkingResult_Caches.emplace_back(new WorkingResultCacheData(results, uv_hrtime()));
	}
}

void WorkingResultNotify(CGA::cga_working_result_t results)
{
	boost::unique_lock<boost::shared_mutex> lock(g_WorkingResult_Lock);
	WorkingResultDoAsyncCall(results);
}

void WorkingResultAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (WorkingResultNotifyData *)handle->data;

	Local<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "type").ToLocalChecked(), Integer::New(isolate, data->m_results.type));
		obj->Set(context, String::NewFromUtf8(isolate, "success").ToLocalChecked(), Boolean::New(isolate, data->m_results.success));
		obj->Set(context, String::NewFromUtf8(isolate, "levelup").ToLocalChecked(), Boolean::New(isolate, data->m_results.success));
		obj->Set(context, String::NewFromUtf8(isolate, "xp").ToLocalChecked(), Integer::New(isolate, data->m_results.xp));		
		obj->Set(context, String::NewFromUtf8(isolate, "endurance").ToLocalChecked(), Integer::New(isolate, data->m_results.endurance));
		obj->Set(context, String::NewFromUtf8(isolate, "skillful").ToLocalChecked(), Integer::New(isolate, data->m_results.skillful));
		obj->Set(context, String::NewFromUtf8(isolate, "intelligence").ToLocalChecked(), Integer::New(isolate, data->m_results.intelligence));
		switch (data->m_results.type)
		{
		case WORK_TYPE_GATHERING:
			obj->Set(context, String::NewFromUtf8(isolate, "imgid").ToLocalChecked(), Integer::New(isolate, data->m_results.imgid));
			obj->Set(context, String::NewFromUtf8(isolate, "count").ToLocalChecked(), Integer::New(isolate, data->m_results.count));
			obj->Set(context, String::NewFromUtf8(isolate, "itemname").ToLocalChecked(), Nan::New(data->m_results.name).ToLocalChecked());
			break;
		case WORK_TYPE_HEALING:
			obj->Set(context, String::NewFromUtf8(isolate, "status").ToLocalChecked(), Integer::New(isolate, data->m_results.status));
			break;
		case WORK_TYPE_ASSESSING:
			break;
		case WORK_TYPE_CRAFTING:
			obj->Set(context, String::NewFromUtf8(isolate, "imgid").ToLocalChecked(), Integer::New(isolate, data->m_results.imgid));
			break;
		}
		argv[1] = obj;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void WorkingResultTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (WorkingResultNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_WorkingResult_Lock);
		for (size_t i = 0; i < g_WorkingResult_Asyncs.size(); ++i)
		{
			if (g_WorkingResult_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_WorkingResult_Asyncs.erase(g_WorkingResult_Asyncs.begin() + i);
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

void AsyncWaitWorkingResult(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsFunction()) {
		Nan::ThrowTypeError("Arg[0] must be function.");
		return;
	}
	int timeout = 3000;
	if (info.Length() >= 2 && info[1]->IsInt32())
	{
		timeout = info[1]->Int32Value(context).ToChecked();
		if (timeout < 0)
			timeout = 0;
	}

	auto callback = Local<Function>::Cast(info[0]);
	WorkingResultNotifyData *data = new WorkingResultNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_WorkingResult_Lock);

		g_WorkingResult_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_WorkingResult_Caches.begin(); p != g_WorkingResult_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_WorkingResult_Caches.erase(p);
			}
			else
			{
				WorkingResultDoAsyncCall((*p)->m_result);
				delete *p;
				g_WorkingResult_Caches.erase(p);
				break;
			}
		}
	}

	if (timeout)
		uv_timer_start(data->m_timer, WorkingResultTimerCallBack, timeout, 0);
}

void StartWork(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	int skill_index = info[0]->Int32Value(context).ToChecked();

	int sub_index = info[1]->Int32Value(context).ToChecked() & 0xFF;

	if (info.Length() >= 3 && info[2]->IsInt32())
	{
		int sub = info[2]->Int32Value(context).ToChecked();
		sub_index |= (sub & 0xFF) << 8;
	}
	
	bool result = false;

	if (!g_CGAInterface->StartWork(skill_index, sub_index, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void GetCraftStatus(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int status = 0;
	if (!g_CGAInterface->GetCraftStatus(status))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(status);
}

void GetImmediateDoneWorkState(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int state = 0;
	if (!g_CGAInterface->GetImmediateDoneWorkState(state))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(state);
}

void CraftItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
	if (info.Length() < 4 || !info[3]->IsArray()) {
		Nan::ThrowTypeError("Arg[4] must be array.");
		return;
	}
	bool result = false;
	CGA::cga_craft_item_t craft;

	craft.skill_index = info[0]->Int32Value(context).ToChecked();
	craft.subskill_index = info[1]->Int32Value(context).ToChecked();
	craft.sub_type = info[2]->Int32Value(context).ToChecked();
	
	auto arr = Local<Array>::Cast(info[3]);
	auto arraylength = arr->Length();
	for (uint32_t i = 0; i < min(arraylength, 6); ++i)
	{
		auto v_itempos = arr->Get(context, i);
		if (!v_itempos.IsEmpty() && v_itempos.ToLocalChecked()->IsInt32())
		{
			craft.itempos[i] = v_itempos.ToLocalChecked()->Int32Value(context).ToChecked();
		}
	}

	if (!g_CGAInterface->CraftItem(craft, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void AssessItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be itempos.");
		return;
	}
	CGA::cga_craft_item_t craft;
	int skill_index = info[0]->Int32Value(context).ToChecked();
	int itempos = info[1]->Int32Value(context).ToChecked();

	bool result = false;
	if (!g_CGAInterface->AssessItem(skill_index, itempos, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void SetImmediateDoneWork(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[0] must be boolean.");
		return;
	}
	
	bool enable = info[0]->BooleanValue(isolate);

	if (!g_CGAInterface->SetImmediateDoneWork(enable))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}