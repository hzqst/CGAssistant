#include <nan.h>
#include "../CGALib/gameinterface.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

#include "async.h"

void BattleActionAsyncCallBack(uv_async_t *handle);

class BattleActionNotifyData : public CBaseNotifyData
{
public:
	BattleActionNotifyData() : CBaseNotifyData(BattleActionAsyncCallBack)
	{

	}

	int m_flags;
};

class BattleActionCacheData
{
public:
	BattleActionCacheData(int flags, uint64_t time) : m_flags(flags), m_time(time)
	{

	}
	int m_flags;
	uint64_t m_time;
};

boost::shared_mutex  g_BattleAction_Lock;
std::vector<BattleActionNotifyData *> g_BattleAction_Asyncs;
std::vector<BattleActionCacheData *> g_BattleAction_Caches;

void BattleActionDoAsyncCall(int flags)
{
	if (g_BattleAction_Asyncs.size())
	{
		for (size_t i = 0; i < g_BattleAction_Asyncs.size(); ++i)
		{
			const auto data = g_BattleAction_Asyncs[i];

			data->m_flags = flags;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_BattleAction_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_BattleAction_Caches.begin(); p != g_BattleAction_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_BattleAction_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_BattleAction_Caches.emplace_back(new BattleActionCacheData(flags, uv_hrtime()));
	}
}

void BattleActionNotify(int flags)
{
	boost::unique_lock<boost::shared_mutex> lock(g_BattleAction_Lock);
	BattleActionDoAsyncCall(flags);
}

void BattleActionAsyncCallBack(uv_async_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (BattleActionNotifyData *)handle->data;

	Local<Value> nullValue = Nan::Null();
	Handle<Value> argv[2];
	argv[0] = data->m_result ? nullValue : Nan::TypeError("Unknown exception.");
	if (data->m_result)
		argv[1] = Integer::New(isolate, data->m_flags);

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void BattleActionTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (BattleActionNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_BattleAction_Lock);
		for (size_t i = 0; i < g_BattleAction_Asyncs.size(); ++i)
		{
			if (g_BattleAction_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_BattleAction_Asyncs.erase(g_BattleAction_Asyncs.begin() + i);
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

void AsyncWaitBattleAction(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
	BattleActionNotifyData *data = new BattleActionNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_BattleAction_Lock);
		g_BattleAction_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_BattleAction_Caches.begin(); p != g_BattleAction_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_BattleAction_Caches.erase(p);
			}
			else
			{
				BattleActionDoAsyncCall((*p)->m_flags);
				delete *p;
				g_BattleAction_Caches.erase(p);
				break;
			}
		}
	}

	if(timeout)
		uv_timer_start(data->m_timer, BattleActionTimerCallBack, timeout, 0);
}

void GetBattleUnits(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	CGA::cga_battle_units_t myinfos;
	if (!g_CGAInterface->GetBattleUnits(myinfos))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfos.size(); ++i)
	{
		Local<Object> obj = Object::New(isolate);
		const CGA::cga_battle_unit_t &myinfo = myinfos.at(i);
;		obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
		obj->Set(String::NewFromUtf8(isolate, "modelid"), Integer::New(isolate, myinfo.modelid));
		obj->Set(String::NewFromUtf8(isolate, "curhp"), Integer::New(isolate, myinfo.curhp));
		obj->Set(String::NewFromUtf8(isolate, "maxhp"), Integer::New(isolate, myinfo.maxhp));
		obj->Set(String::NewFromUtf8(isolate, "curmp"), Integer::New(isolate, myinfo.curmp));
		obj->Set(String::NewFromUtf8(isolate, "maxmp"), Integer::New(isolate, myinfo.maxmp));
		obj->Set(String::NewFromUtf8(isolate, "pos"), Integer::New(isolate, myinfo.pos));
		obj->Set(String::NewFromUtf8(isolate, "flags"), Integer::New(isolate, myinfo.flags));
		arr->Set(i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void GetBattleContext(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	CGA::cga_battle_context_t ctx;
	if (!g_CGAInterface->GetBattleContext(ctx))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "round_count"), Integer::New(isolate, ctx.round_count));
	obj->Set(String::NewFromUtf8(isolate, "player_pos"), Integer::New(isolate, ctx.player_pos));
	obj->Set(String::NewFromUtf8(isolate, "player_status"), Integer::New(isolate, ctx.player_status));
	obj->Set(String::NewFromUtf8(isolate, "skill_performed"), Integer::New(isolate, ctx.skill_performed));
	obj->Set(String::NewFromUtf8(isolate, "skill_allowbit"), Integer::New(isolate, ctx.skill_allowbit));
	obj->Set(String::NewFromUtf8(isolate, "petskill_allowbit"), Integer::New(isolate, ctx.petskill_allowbit));
	obj->Set(String::NewFromUtf8(isolate, "weapon_allowbit"), Integer::New(isolate, ctx.weapon_allowbit));
	obj->Set(String::NewFromUtf8(isolate, "petid"), Integer::New(isolate, ctx.petid));
	obj->Set(String::NewFromUtf8(isolate, "effect_flags"), Integer::New(isolate, ctx.effect_flags));

	info.GetReturnValue().Set(obj);
}

void BattleNormalAttack(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be target.");
		return;
	}
	
	int target = (int)info[0]->IntegerValue();
	
	bool bResult = false;
	if (!g_CGAInterface->BattleNormalAttack(target, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void BattleSkillAttack(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be skill pos.");
		return;
	}
	
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be skill level.");
		return;
	}
	
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Arg[2] must be target.");
		return;
	}
	
	int skillpos = (int)info[0]->IntegerValue();
	int skilllevel = (int)info[1]->IntegerValue();
	int target = (int)info[2]->IntegerValue();

	bool packetOnly = (info.Length() >= 4 && info[3]->IsBoolean()) ? (int)info[3]->BooleanValue() : false;

	bool bResult = false;
	if (!g_CGAInterface->BattleSkillAttack(skillpos, skilllevel, target, packetOnly, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void BattleRebirth(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	bool bResult = false;
	if (!g_CGAInterface->BattleRebirth(bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void BattleGuard(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	bool bResult = false;
	if (!g_CGAInterface->BattleGuard(bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void BattleEscape(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	bool bResult = false;
	if (!g_CGAInterface->BattleEscape(bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void BattleDoNothing(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	bool bResult = false;
	if (!g_CGAInterface->BattleDoNothing(bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void BattleExchangePosition(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	bool bResult = false;
	if (!g_CGAInterface->BattleExchangePosition(bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void BattleChangePet(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be pet id.");
		return;
	}
	
	int petid = (int)info[0]->IntegerValue();

	bool bResult = false;
	if (!g_CGAInterface->BattleChangePet(petid, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void BattleUseItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be item pos.");
		return;
	}
	
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[1] must be target.");
		return;
	}
	
	int itempos = (int)info[0]->IntegerValue();
	int target = (int)info[1]->IntegerValue();

	bool bResult = false;
	if (!g_CGAInterface->BattleUseItem(itempos, target, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void BattlePetSkillAttack(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be pet skill pos.");
		return;
	}
	
	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be target.");
		return;
	}

	int skillpos = (int)info[0]->IntegerValue();
	int target = (int)info[1]->IntegerValue();
	bool packetOnly = (info.Length() >= 3 && info[2]->IsBoolean()) ?  (int)info[2]->BooleanValue() : false;

	bool bResult = false;
	if (!g_CGAInterface->BattlePetSkillAttack(skillpos, target, packetOnly, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}