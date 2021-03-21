#include <nan.h>
#include "../CGALib/gameinterface.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace v8;

#include "async.h"

extern CGA::CGAInterface *g_CGAInterface;

void TradeStateAsyncCallBack(uv_async_t *handle);
void TradeDialogAsyncCallBack(uv_async_t *handle);
void TradeStuffsAsyncCallBack(uv_async_t *handle);
void PlayerMenuAsyncCallBack(uv_async_t *handle);
void UnitMenuAsyncCallBack(uv_async_t *handle);

class TradeStateNotifyData : public CBaseNotifyData
{
public:
	TradeStateNotifyData() : CBaseNotifyData(TradeStateAsyncCallBack)
	{
	}

	int m_info;
};

class TradeStateCacheData
{
public:
	TradeStateCacheData(int info, uint64_t time) : m_info(info), m_time(time)
	{

	}
	int m_info;
	uint64_t m_time;
};

class TradeDialogNotifyData : public CBaseNotifyData
{
public:
	TradeDialogNotifyData() : CBaseNotifyData(TradeDialogAsyncCallBack)
	{
	}

	CGA::cga_trade_dialog_t m_info;
};

class TradeDialogCacheData
{
public:
	TradeDialogCacheData(const CGA::cga_trade_dialog_t &info, uint64_t time) : m_info(info), m_time(time)
	{

	}
	CGA::cga_trade_dialog_t m_info;
	uint64_t m_time;
};

class TradeStuffsNotifyData : public CBaseNotifyData
{
public:
	TradeStuffsNotifyData() : CBaseNotifyData(TradeStuffsAsyncCallBack)
	{
	}

	CGA::cga_trade_stuff_info_t m_info;
};

class TradeStuffsCacheData
{
public:
	TradeStuffsCacheData(const CGA::cga_trade_stuff_info_t &info, uint64_t time) : m_info(info), m_time(time)
	{

	}
	CGA::cga_trade_stuff_info_t m_info;
	uint64_t m_time;
};

class PlayerMenuNotifyData : public CBaseNotifyData
{
public:
	PlayerMenuNotifyData() : CBaseNotifyData(PlayerMenuAsyncCallBack)
	{
	}

	CGA::cga_player_menu_items_t m_players;
};

class PlayerMenuCacheData
{
public:
	PlayerMenuCacheData(const CGA::cga_player_menu_items_t &info, uint64_t time) : m_info(info), m_time(time)
	{

	}
	CGA::cga_player_menu_items_t m_info;
	uint64_t m_time;
};

class UnitMenuNotifyData : public CBaseNotifyData
{
public:
	UnitMenuNotifyData() : CBaseNotifyData(UnitMenuAsyncCallBack)
	{
	}

	CGA::cga_unit_menu_items_t m_units;
};

class UnitMenuCacheData
{
public:
	UnitMenuCacheData(const CGA::cga_unit_menu_items_t &info, uint64_t time) : m_info(info), m_time(time)
	{

	}
	CGA::cga_unit_menu_items_t m_info;
	uint64_t m_time;
};

boost::shared_mutex g_TradeState_Lock;
std::vector<TradeStateNotifyData *> g_TradeState_Asyncs;
std::vector<TradeStateCacheData *> g_TradeState_Caches;

boost::shared_mutex g_TradeDialog_Lock;
std::vector<TradeDialogNotifyData *> g_TradeDialog_Asyncs;
std::vector<TradeDialogCacheData *> g_TradeDialog_Caches;

boost::shared_mutex  g_TradeStuffs_Lock;
std::vector<TradeStuffsNotifyData *> g_TradeStuffs_Asyncs;
std::vector<TradeStuffsCacheData *> g_TradeStuffs_Caches;

boost::shared_mutex  g_PlayerMenu_Lock;
std::vector<PlayerMenuNotifyData *> g_PlayerMenu_Asyncs;
std::vector<PlayerMenuCacheData *> g_PlayerMenu_Caches;

boost::shared_mutex g_UnitMenu_Lock;
std::vector<UnitMenuNotifyData *> g_UnitMenu_Asyncs;
std::vector<UnitMenuCacheData *> g_UnitMenu_Caches;

void ChangePetState(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	int petpos = info[0]->Int32Value(context).ToChecked();
	int state = info[1]->Int32Value(context).ToChecked();

	bool result = false;
	if (!g_CGAInterface->ChangePetState(petpos, state, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void DropPet(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int petpos = info[0]->Int32Value(context).ToChecked();

	bool result = false;

	if (!g_CGAInterface->DropPet(petpos, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void DropItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int itempos = info[0]->Int32Value(context).ToChecked();

	bool result = false;

	if (!g_CGAInterface->DropItem(itempos, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void TradeStateDoAsyncCall(int state)
{
	if (g_TradeState_Asyncs.size())
	{
		for (size_t i = 0; i < g_TradeState_Asyncs.size(); ++i)
		{
			const auto data = g_TradeState_Asyncs[i];
			data->m_info = state;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_TradeState_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_TradeState_Caches.begin(); p != g_TradeState_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_TradeState_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_TradeState_Caches.emplace_back(new TradeStateCacheData(state, uv_hrtime()));
	}
}

void TradeStateNotify(int state)
{
	boost::unique_lock<boost::shared_mutex> lock(g_TradeState_Lock);
	TradeStateDoAsyncCall(state);
}

void TradeStateAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (TradeStateNotifyData *)handle->data;

	Local<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
		argv[1] = Integer::New(isolate, data->m_info);

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void TradeStateTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (TradeStateNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_TradeState_Lock);
		for (size_t i = 0; i < g_TradeState_Asyncs.size(); ++i)
		{
			if (g_TradeState_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_TradeState_Asyncs.erase(g_TradeState_Asyncs.begin() + i);
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

void TradeDialogDoAsyncCall(const CGA::cga_trade_dialog_t &info)
{
	if (g_TradeDialog_Asyncs.size())
	{
		for (size_t i = 0; i < g_TradeDialog_Asyncs.size(); ++i)
		{
			const auto data = g_TradeDialog_Asyncs[i];
			data->m_info = info;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_TradeDialog_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_TradeDialog_Caches.begin(); p != g_TradeDialog_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_TradeDialog_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_TradeDialog_Caches.emplace_back(new TradeDialogCacheData(info, uv_hrtime()));
	}
}

void TradeDialogNotify(CGA::cga_trade_dialog_t info)
{
	boost::unique_lock<boost::shared_mutex> lock(g_TradeDialog_Lock);
	TradeDialogDoAsyncCall(info);
}

void TradeDialogAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (TradeDialogNotifyData *)handle->data;

	Local<Value> argv[3];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		argv[1] = Nan::New(data->m_info.name).ToLocalChecked();
		argv[2] = Integer::New(isolate, data->m_info.level);
	}

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 3 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void TradeDialogTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (TradeDialogNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_TradeDialog_Lock);
		for (size_t i = 0; i < g_TradeDialog_Asyncs.size(); ++i)
		{
			if (g_TradeDialog_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_TradeDialog_Asyncs.erase(g_TradeDialog_Asyncs.begin() + i);
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

void TradeStuffsDoAsyncCall(const CGA::cga_trade_stuff_info_t &info)
{
	if (g_TradeStuffs_Asyncs.size())
	{
		for (size_t i = 0; i < g_TradeStuffs_Asyncs.size(); ++i)
		{
			const auto data = g_TradeStuffs_Asyncs[i];
			data->m_info = info;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_TradeStuffs_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_TradeStuffs_Caches.begin(); p != g_TradeStuffs_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_TradeStuffs_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_TradeStuffs_Caches.emplace_back(new TradeStuffsCacheData(info, uv_hrtime()));
	}
}

void TradeStuffsNotify(CGA::cga_trade_stuff_info_t info)
{
	boost::unique_lock<boost::shared_mutex> lock(g_TradeStuffs_Lock);
	TradeStuffsDoAsyncCall(info);
}

void TradeStuffsAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (TradeStuffsNotifyData *)handle->data;

	if (data->m_result)
	{
		if(data->m_info.type == TRADE_STUFFS_ITEM)
		{
			Local<Value> argv[3];
			Local<Value> nullValue = Nan::Null();
			argv[0] = nullValue;
			argv[1] = Nan::New(TRADE_STUFFS_ITEM);
			Local<Array> arr = Array::New(isolate);
			for (size_t i = 0; i < data->m_info.items.size(); ++i)
			{
				Local<Object> obj = Object::New(isolate);				
				obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(data->m_info.items[i].name).ToLocalChecked());
				obj->Set(context, String::NewFromUtf8(isolate, "attr").ToLocalChecked(), Nan::New(data->m_info.items[i].attr).ToLocalChecked());
				obj->Set(context, String::NewFromUtf8(isolate, "itemid").ToLocalChecked(), Integer::New(isolate, data->m_info.items[i].itemid));
				obj->Set(context, String::NewFromUtf8(isolate, "count").ToLocalChecked(), Integer::New(isolate, data->m_info.items[i].count));
				obj->Set(context, String::NewFromUtf8(isolate, "pos").ToLocalChecked(), Integer::New(isolate, data->m_info.items[i].pos));
				obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, data->m_info.items[i].level));
				obj->Set(context, String::NewFromUtf8(isolate, "type").ToLocalChecked(), Integer::New(isolate, data->m_info.items[i].type));
				arr->Set(context, i, obj);
			}
			argv[2] = arr;
			Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 3, argv);
		}
		else if(data->m_info.type == TRADE_STUFFS_PET)
		{
			Local<Value> argv[2];
			Local<Value> nullValue = Nan::Null();
			argv[0] = nullValue;
			argv[1] = Nan::New(TRADE_STUFFS_PET);
			Local<Object> obj = Object::New(isolate);
			obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.index));
			obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(data->m_info.pet.name).ToLocalChecked());
			obj->Set(context, String::NewFromUtf8(isolate, "realname").ToLocalChecked(), Nan::New(data->m_info.pet.realname).ToLocalChecked());
			obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.level));
			obj->Set(context, String::NewFromUtf8(isolate, "race").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.race));
			obj->Set(context, String::NewFromUtf8(isolate, "maxhp").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.maxhp));
			obj->Set(context, String::NewFromUtf8(isolate, "maxmp").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.maxmp));
			obj->Set(context, String::NewFromUtf8(isolate, "loyality").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.loyality));
			obj->Set(context, String::NewFromUtf8(isolate, "skill_count").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.skill_count));
			obj->Set(context, String::NewFromUtf8(isolate, "image_id").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.image_id));

			Local<Object> objd = Object::New(isolate);
			objd->Set(context, String::NewFromUtf8(isolate, "points_remain").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.points_remain));
			objd->Set(context, String::NewFromUtf8(isolate, "points_endurance").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.points_endurance));
			objd->Set(context, String::NewFromUtf8(isolate, "points_strength").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.points_strength));
			objd->Set(context, String::NewFromUtf8(isolate, "points_defense").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.points_defense));
			objd->Set(context, String::NewFromUtf8(isolate, "points_agility").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.points_agility));
			objd->Set(context, String::NewFromUtf8(isolate, "points_magical").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.points_magical));
			objd->Set(context, String::NewFromUtf8(isolate, "value_attack").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.value_attack));
			objd->Set(context, String::NewFromUtf8(isolate, "value_defensive").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.value_defensive));
			objd->Set(context, String::NewFromUtf8(isolate, "value_agility").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.value_agility));
			objd->Set(context, String::NewFromUtf8(isolate, "value_spirit").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.value_spirit));
			objd->Set(context, String::NewFromUtf8(isolate, "value_recovery").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.value_recovery));
			objd->Set(context, String::NewFromUtf8(isolate, "resist_poison").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.resist_poison));
			objd->Set(context, String::NewFromUtf8(isolate, "resist_sleep").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.resist_sleep));
			objd->Set(context, String::NewFromUtf8(isolate, "resist_medusa").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.resist_medusa));
			objd->Set(context, String::NewFromUtf8(isolate, "resist_drunk").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.resist_drunk));
			objd->Set(context, String::NewFromUtf8(isolate, "resist_chaos").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.resist_chaos));
			objd->Set(context, String::NewFromUtf8(isolate, "resist_forget").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.resist_forget));
			objd->Set(context, String::NewFromUtf8(isolate, "fix_critical").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.fix_critical));
			objd->Set(context, String::NewFromUtf8(isolate, "fix_strikeback").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.fix_strikeback));
			objd->Set(context, String::NewFromUtf8(isolate, "fix_accurancy").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.fix_accurancy));
			objd->Set(context, String::NewFromUtf8(isolate, "fix_dodge").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.fix_dodge));
			objd->Set(context, String::NewFromUtf8(isolate, "element_earth").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.element_earth));
			objd->Set(context, String::NewFromUtf8(isolate, "element_water").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.element_water));
			objd->Set(context, String::NewFromUtf8(isolate, "element_fire").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.element_fire));
			objd->Set(context, String::NewFromUtf8(isolate, "element_wind").ToLocalChecked(), Integer::New(isolate, data->m_info.pet.detail.element_wind));
			
			obj->Set(context, String::NewFromUtf8(isolate, "detail").ToLocalChecked(), objd);

			argv[2] = obj;

			Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 3, argv);
		}
		else if (data->m_info.type == TRADE_STUFFS_PETSKILL) 
		{
			Local<Value> argv[3];
			Local<Value> nullValue = Nan::Null();
			argv[0] = nullValue;
			argv[1] = Nan::New(TRADE_STUFFS_PETSKILL);

			Local<Object> obj = Object::New(isolate);
			obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, data->m_info.petskills.index));
			Local<Array> arr = Array::New(isolate);
			for (size_t i = 0; i < data->m_info.petskills.skills.size(); ++i)
			{
				arr->Set(context, i, Nan::New(data->m_info.petskills.skills[i]).ToLocalChecked());
			}
			obj->Set(context, String::NewFromUtf8(isolate, "skills").ToLocalChecked(), arr);
			argv[2] = obj;

			Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 3, argv);
		}
		else if(data->m_info.type == TRADE_STUFFS_GOLD)
		{
			Local<Value> argv[3];
			Local<Value> nullValue = Nan::Null();

			argv[0] = nullValue;
			argv[1] = Nan::New(TRADE_STUFFS_GOLD);
			argv[2] = Integer::New(isolate, data->m_info.gold);
			Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 3, argv);
		} else {
			Local<Value> argv[1];
			argv[0] = Nan::Error("Unknown exception.");
			Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 1, argv);
		}
	}
	else
	{
		Local<Value> argv[1];
		argv[0] = Nan::Error("Unknown exception.");
		Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 1, argv);
	}

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void TradeStuffsTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (TradeStuffsNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_TradeStuffs_Lock);
		for (size_t i = 0; i < g_TradeStuffs_Asyncs.size(); ++i)
		{
			if (g_TradeStuffs_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_TradeStuffs_Asyncs.erase(g_TradeStuffs_Asyncs.begin() + i);
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

void PlayerMenuDoAsyncCall(const CGA::cga_player_menu_items_t &players)
{
	if (g_PlayerMenu_Asyncs.size())
	{
		for (size_t i = 0; i < g_PlayerMenu_Asyncs.size(); ++i)
		{
			const auto data = g_PlayerMenu_Asyncs[i];
			data->m_players = players;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_PlayerMenu_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_PlayerMenu_Caches.begin(); p != g_PlayerMenu_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_PlayerMenu_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_PlayerMenu_Caches.emplace_back(new PlayerMenuCacheData(players, uv_hrtime()));
	}
}

void PlayerMenuNotify(CGA::cga_player_menu_items_t players)
{
	boost::unique_lock<boost::shared_mutex> lock(g_PlayerMenu_Lock);
	PlayerMenuDoAsyncCall(players);
}

void PlayerMenuAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (PlayerMenuNotifyData *)handle->data;

	Local<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		Local<Array> arr = Array::New(isolate);
		for (size_t i = 0; i < data->m_players.size(); ++i)
		{
			Local<Object> obj = Object::New(isolate);
			obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(data->m_players[i].name).ToLocalChecked());
			obj->Set(context, String::NewFromUtf8(isolate, "color").ToLocalChecked(), Integer::New(isolate, data->m_players[i].color));
			obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, data->m_players[i].index));
			arr->Set(context, i, obj);
		}
		argv[1] = arr;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void PlayerMenuTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (PlayerMenuNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_PlayerMenu_Lock);
		for (size_t i = 0; i < g_PlayerMenu_Asyncs.size(); ++i)
		{
			if (g_PlayerMenu_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_PlayerMenu_Asyncs.erase(g_PlayerMenu_Asyncs.begin() + i);
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

void UnitMenuDoAsyncCall(const CGA::cga_unit_menu_items_t &units)
{
	if (g_UnitMenu_Asyncs.size())
	{
		for (size_t i = 0; i < g_UnitMenu_Asyncs.size(); ++i)
		{
			const auto data = g_UnitMenu_Asyncs[i];
			data->m_units = units;
			data->m_result = true;
			uv_async_send(data->m_async);
		}
		g_UnitMenu_Asyncs.clear();
	}
	else
	{
		auto hrtime = uv_hrtime();
		for (auto &p = g_UnitMenu_Caches.begin(); p != g_UnitMenu_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_UnitMenu_Caches.erase(p);
			}
			else
			{
				p++;
			}
		}
		g_UnitMenu_Caches.emplace_back(new UnitMenuCacheData(units, uv_hrtime()));
	}
}

void UnitMenuNotify(CGA::cga_unit_menu_items_t units)
{
	boost::unique_lock<boost::shared_mutex> lock(g_UnitMenu_Lock);
	UnitMenuDoAsyncCall(units);
}

void UnitMenuAsyncCallBack(uv_async_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (UnitMenuNotifyData *)handle->data;

	Local<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		Local<Array> arr = Array::New(isolate);
		for (size_t i = 0; i < data->m_units.size(); ++i)
		{
			Local<Object> obj = Object::New(isolate);
			obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(data->m_units[i].name).ToLocalChecked());
			obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, data->m_units[i].level));
			obj->Set(context, String::NewFromUtf8(isolate, "health").ToLocalChecked(), Integer::New(isolate, data->m_units[i].health));
			obj->Set(context, String::NewFromUtf8(isolate, "hp").ToLocalChecked(), Integer::New(isolate, data->m_units[i].hp));
			obj->Set(context, String::NewFromUtf8(isolate, "maxhp").ToLocalChecked(), Integer::New(isolate, data->m_units[i].maxhp));
			obj->Set(context, String::NewFromUtf8(isolate, "mp").ToLocalChecked(), Integer::New(isolate, data->m_units[i].mp));
			obj->Set(context, String::NewFromUtf8(isolate, "maxmp").ToLocalChecked(), Integer::New(isolate, data->m_units[i].maxmp));
			obj->Set(context, String::NewFromUtf8(isolate, "color").ToLocalChecked(), Integer::New(isolate, data->m_units[i].color));
			obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, data->m_units[i].index));
			arr->Set(context, i, obj);
		}
		argv[1] = arr;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void UnitMenuTimerCallBack(uv_timer_t *handle)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (UnitMenuNotifyData *)handle->data;

	bool asyncNotCalled = false;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_UnitMenu_Lock);
		for (size_t i = 0; i < g_UnitMenu_Asyncs.size(); ++i)
		{
			if (g_UnitMenu_Asyncs[i] == data)
			{
				asyncNotCalled = true;
				g_UnitMenu_Asyncs.erase(g_UnitMenu_Asyncs.begin() + i);
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

void UseItem(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int itempos = (int)info[0]->Int32Value(context).ToChecked();

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

	int itempos = info[0]->Int32Value(context).ToChecked();

	int dstpos = info[1]->Int32Value(context).ToChecked();

	int count = info[2]->Int32Value(context).ToChecked();
	
	bool bResult = false;

	if (!g_CGAInterface->MoveItem(itempos, dstpos, count, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void MovePet(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	int srcpos = info[0]->Int32Value(context).ToChecked();
	int dstpos = info[1]->Int32Value(context).ToChecked();

	bool bResult = false;
	if (!g_CGAInterface->MovePet(srcpos, dstpos, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void MoveGold(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	int gold = info[0]->Int32Value(context).ToChecked();
	int opt = info[1]->Int32Value(context).ToChecked();

	bool bResult = false;
	if (!g_CGAInterface->MoveGold(gold, opt, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	info.GetReturnValue().Set(bResult);
}

void AsyncWaitPlayerMenu(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
	PlayerMenuNotifyData *data = new PlayerMenuNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_PlayerMenu_Lock);
		g_PlayerMenu_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_PlayerMenu_Caches.begin(); p != g_PlayerMenu_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_PlayerMenu_Caches.erase(p);
			}
			else
			{
				PlayerMenuDoAsyncCall((*p)->m_info);
				delete *p;
				g_PlayerMenu_Caches.erase(p);
				break;
			}
		}
	}

	if(timeout)
		uv_timer_start(data->m_timer, PlayerMenuTimerCallBack, timeout, 0);
}

void PlayerMenuSelect(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int menuindex = info[0]->Int32Value(context).ToChecked();

	std::string menustring;

	if (info.Length() >= 2 && info[1]->IsString())
	{
		v8::String::Utf8Value str(isolate, info[1]->ToString(context).ToLocalChecked());
		menustring = *str;
	}

	bool bResult = false;
	if (!g_CGAInterface->PlayerMenuSelect(menuindex, menustring, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}

void AsyncWaitUnitMenu(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
	UnitMenuNotifyData *data = new UnitMenuNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_UnitMenu_Lock);
		g_UnitMenu_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_UnitMenu_Caches.begin(); p != g_UnitMenu_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_UnitMenu_Caches.erase(p);
			}
			else
			{
				UnitMenuDoAsyncCall((*p)->m_info);
				delete *p;
				g_UnitMenu_Caches.erase(p);
				break;
			}
		}
	}

	if(timeout)
		uv_timer_start(data->m_timer, UnitMenuTimerCallBack, timeout, 0);
}

void UnitMenuSelect(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int menuindex = info[0]->Int32Value(context).ToChecked();

	bool bResult = false;

	if (!g_CGAInterface->UnitMenuSelect(menuindex, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}

void UpgradePlayer(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int attr = (int)info[0]->Int32Value(context).ToChecked();

	if (!g_CGAInterface->UpgradePlayer(attr))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(true);
}

void UpgradePet(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	int petid = info[0]->Int32Value(context).ToChecked();

	int attr = info[1]->Int32Value(context).ToChecked();

	if (!g_CGAInterface->UpgradePet(petid, attr))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(true);
}

void DoRequest(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int request_type = info[0]->Int32Value(context).ToChecked();

	bool result = false;

	if (!g_CGAInterface->DoRequest(request_type, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void EnableFlags(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	int type = info[0]->Int32Value(context).ToChecked();

	bool enable = info[1]->BooleanValue(isolate);

	bool result = false;

	if (!g_CGAInterface->EnableFlags(type, enable, result))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(result);
}

void TradeAddStuffs(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsArray()) {
		Nan::ThrowTypeError("Arg[0] must be array.");
		return;
	}
	
	if (info.Length() < 2 || !info[1]->IsArray()) {
		Nan::ThrowTypeError("Arg[1] must be array.");
		return;
	}
	
	if (info.Length() < 3 || !info[2]->IsInt32()) {
		Nan::ThrowTypeError("Arg[2] must be integer.");
		return;
	}

	CGA::cga_sell_items_t items;
	CGA::cga_sell_pets_t pets;

	{
		Local<Array> arr = Local<Array>::Cast(info[0]);
		uint32_t length = arr->Length();

		for (uint32_t i = 0; i < length; ++i)
		{
			auto element = arr->Get(context, i);
			if (!element.IsEmpty() && element.ToLocalChecked()->IsObject())
			{
				int itemid = -1;
				int itempos = -1;
				int count = 0;

				Local<Object> obj = Local<Object>::Cast(element.ToLocalChecked());

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
	}

	{

		Local<Array> arr = Local<Array>::Cast(info[1]);
		uint32_t length = arr->Length();
		for (uint32_t i = 0; i < length; ++i)
		{
			auto element = arr->Get(context, i);
			if (!element.IsEmpty() && element.ToLocalChecked()->IsInt32())
			{
				int petid = element.ToLocalChecked()->Int32Value(context).ToChecked();
				pets.emplace_back(petid);
			}
		}
	}
	
	int gold = info[2]->Int32Value(context).ToChecked();

	if (!g_CGAInterface->TradeAddStuffs(items, pets, gold))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void AsyncWaitTradeStuffs(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
	TradeStuffsNotifyData *data = new TradeStuffsNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_TradeStuffs_Lock);
		g_TradeStuffs_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_TradeStuffs_Caches.begin(); p != g_TradeStuffs_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_TradeStuffs_Caches.erase(p);
			}
			else
			{
				TradeStuffsDoAsyncCall((*p)->m_info);
				delete *p;
				g_TradeStuffs_Caches.erase(p);
				break;
			}
		}
	}

	if(timeout)
		uv_timer_start(data->m_timer, TradeStuffsTimerCallBack, timeout, 0);
}

void AsyncWaitTradeDialog(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
		timeout = (int)info[1]->Int32Value(context).ToChecked();
		if (timeout < 0)
			timeout = 0;
	}

	auto callback = Local<Function>::Cast(info[0]);
	TradeDialogNotifyData *data = new TradeDialogNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_TradeDialog_Lock);

		g_TradeDialog_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_TradeDialog_Caches.begin(); p != g_TradeDialog_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_TradeDialog_Caches.erase(p);
			}
			else
			{
				TradeDialogDoAsyncCall((*p)->m_info);
				delete *p;
				g_TradeDialog_Caches.erase(p);
				break;
			}
		}
	}

	if (timeout)
		uv_timer_start(data->m_timer, TradeDialogTimerCallBack, timeout, 0);
}

void AsyncWaitTradeState(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
	TradeStateNotifyData *data = new TradeStateNotifyData();
	data->m_callback.Reset(isolate, callback);
	data->m_async->data = data;
	data->m_timer->data = data;

	{
		boost::unique_lock<boost::shared_mutex> lock(g_TradeState_Lock);

		g_TradeState_Asyncs.push_back(data);

		auto hrtime = uv_hrtime();
		for (auto &p = g_TradeState_Caches.begin(); p != g_TradeState_Caches.end(); )
		{
			if (hrtime - (*p)->m_time > CGA_NOTIFY_MAX_CACHE_TIME)
			{
				delete *p;
				p = g_TradeState_Caches.erase(p);
			}
			else
			{
				TradeStateDoAsyncCall((*p)->m_info);
				delete *p;
				g_TradeState_Caches.erase(p);
				break;
			}
		}
	}

	if (timeout)
		uv_timer_start(data->m_timer, TradeStateTimerCallBack, timeout, 0);
}