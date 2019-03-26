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
		//m_permanent = false;
		//m_sent = false;
		//m_timeout = 0;
	}

	//int m_timeout;
	//bool m_sent;
	//bool m_permanent;
	CGA::cga_working_result_t m_results;
};

boost::shared_mutex  g_WorkingResult_Lock;
std::vector<WorkingResultNotifyData *> g_WorkingResult_Asyncs;
/*
static bool is_removable(WorkingResultNotifyData *data)
{
	return (data->m_sent && !data->m_permanent);
}
*/
void WorkingResultNotify(CGA::cga_working_result_t results)
{
	boost::unique_lock<boost::shared_mutex> lock(g_WorkingResult_Lock);
	for (size_t i = 0; i < g_WorkingResult_Asyncs.size(); ++i)
	{
		const auto data = g_WorkingResult_Asyncs[i];
		//if (!data->m_sent) 
		//{
			data->m_results = results;
			data->m_result = true;
			//data->m_sent = true;
			uv_async_send(&data->m_async);
		//}
	}
	g_WorkingResult_Asyncs.clear();
	//g_WorkingResult_Asyncs.erase(std::remove_if(std::begin(g_WorkingResult_Asyncs), std::end(g_WorkingResult_Asyncs), is_removable), std::end(g_WorkingResult_Asyncs));
}

void WorkingResultAsyncCallBack(uv_async_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (WorkingResultNotifyData *)handle->data;

	Handle<Value> argv[1];
	if (data->m_result)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(String::NewFromUtf8(isolate, "type"), Integer::New(isolate, data->m_results.type));
		obj->Set(String::NewFromUtf8(isolate, "success"), Boolean::New(isolate, data->m_results.success));
		obj->Set(String::NewFromUtf8(isolate, "levelup"), Boolean::New(isolate, data->m_results.success));
		obj->Set(String::NewFromUtf8(isolate, "xp"), Integer::New(isolate, data->m_results.xp));		
		obj->Set(String::NewFromUtf8(isolate, "endurance"), Integer::New(isolate, data->m_results.endurance));
		obj->Set(String::NewFromUtf8(isolate, "skillful"), Integer::New(isolate, data->m_results.skillful));
		obj->Set(String::NewFromUtf8(isolate, "intelligence"), Integer::New(isolate, data->m_results.intelligence));
		switch (data->m_results.type)
		{
		case WORK_TYPE_GATHERING:
			obj->Set(String::NewFromUtf8(isolate, "imgid"), Integer::New(isolate, data->m_results.imgid));
			obj->Set(String::NewFromUtf8(isolate, "count"), Integer::New(isolate, data->m_results.count));
			obj->Set(String::NewFromUtf8(isolate, "itemname"), Nan::New(data->m_results.name).ToLocalChecked());
			break;
		case WORK_TYPE_HEALING:
			obj->Set(String::NewFromUtf8(isolate, "status"), Integer::New(isolate, data->m_results.status));
			break;
		case WORK_TYPE_ASSESSING:
			break;
		case WORK_TYPE_CRAFTING:
			obj->Set(String::NewFromUtf8(isolate, "imgid"), Integer::New(isolate, data->m_results.imgid));
			break;
		}
		argv[0] = obj;
	}
	else
	{
		argv[0] = Nan::New(false);
	}

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

//	if (!data->m_permanent)
//	{
		data->m_callback.Reset();

		uv_timer_stop(&data->m_timer);
		uv_close((uv_handle_t*)handle, NULL);
		uv_close((uv_handle_t*)&data->m_timer, NULL);
		delete handle->data;
/*	}
	else
	{
		data->m_sent = false;
	}*/
}

void WorkingResultTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (WorkingResultNotifyData *)handle->data;

	Handle<Value> argv[1];
	argv[0] = Nan::TypeError("Async callback timeout.");

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

	//if (!data->m_permanent)
	//{
		data->m_callback.Reset();

		{
			boost::unique_lock<boost::shared_mutex> lock(g_WorkingResult_Lock);
			for (size_t i = 0; i < g_WorkingResult_Asyncs.size(); ++i)
			{
				if (g_WorkingResult_Asyncs[i] == data)
				{
					g_WorkingResult_Asyncs.erase(g_WorkingResult_Asyncs.begin() + i);
					break;
				}
			}
		}

		uv_timer_stop(handle);
		uv_close((uv_handle_t*)&data->m_async, NULL);		
		uv_close((uv_handle_t*)handle, NULL);
		delete handle->data;
/*	}
	else
	{
		if (data->m_timeout > 0)
		{
			uv_timer_start(&data->m_timer, WorkingResultTimerCallBack, data->m_timeout, 0);
			data->m_sent = false;
		}
	}*/
}

void AsyncWaitWorkingResult(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsFunction()) {
		Nan::ThrowTypeError("Arg[0] must be a function.");
		return;
	}
	int timeout = 3000;
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

	boost::unique_lock<boost::shared_mutex> lock(g_WorkingResult_Lock);

	auto callback = Local<Function>::Cast(info[0]);
	WorkingResultNotifyData *data = new WorkingResultNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async.data = data;
	data->m_timer.data = data;
	//data->m_timeout = timeout;
	//data->m_permanent = permanent;

	g_WorkingResult_Asyncs.push_back(data);
	if (timeout)
		uv_timer_start(&data->m_timer, WorkingResultTimerCallBack, timeout, 0);
}

void StartWork(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be skill_index.");
		return;
	}
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be sub_index.");
		return;
	}
	int skill_index = (int)info[0]->IntegerValue();
	int sub_index = (int)info[1]->IntegerValue() & 0xFF;
	if (info.Length() >= 3 && !info[2]->IsUndefined()) {
		sub_index |= (info[2]->IntegerValue() & 0xFF) << 8;
	}
	
	bool result = false;
	if (!g_CGAInterface->StartWork(skill_index, sub_index, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void CraftItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || info[0]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[0] must be skill_index.");
		return;
	}
	if (info.Length() < 2 || info[1]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[1] must be sub_index.");
		return;
	}
	if (info.Length() < 3 || info[2]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[2] must be sub_index.");
		return;
	}
	if (info.Length() < 4 || !info[3]->IsArray()) {
		Nan::ThrowTypeError("Arg[4] must be itempos array.");
		return;
	}
	CGA::cga_craft_item_t craft;
	craft.skill_index = (int)info[0]->IntegerValue();
	craft.subskill_index = (int)info[1]->IntegerValue();
	craft.sub_type = (int)info[2]->IntegerValue();
	
	Local<Array> arr = Local<Array>::Cast(info[3]);
	for (uint32_t i = 0; i < arr->Length(); ++i)
	{
		Local<Value> v_itempos = arr->Get(i);
		craft.itempos[i] = (int)v_itempos->IntegerValue();
	}

	bool result = false;
	if (!g_CGAInterface->CraftItem(craft, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void AssessItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || info[0]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[0] must be skill_index.");
		return;
	}
	if (info.Length() < 2 || info[1]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[1] must be itempos.");
		return;
	}
	CGA::cga_craft_item_t craft;
	int skill_index = (int)info[0]->IntegerValue();
	int itempos = (int)info[1]->IntegerValue();

	bool result = false;
	if (!g_CGAInterface->AssessItem(skill_index, itempos, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}