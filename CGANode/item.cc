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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be petpos.");
		return;
	}

	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be state.");
		return;
	}

	int petpos = (int)info[0]->IntegerValue();
	int state = (int)info[1]->IntegerValue();

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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be petpos.");
		return;
	}

	int petpos = (int)info[0]->IntegerValue();
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
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (TradeStateNotifyData *)handle->data;

	Handle<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
		argv[1] = Integer::New(isolate, data->m_info);

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void TradeStateTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

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
		Handle<Value> argv[1];
		argv[0] = Nan::Error("Async callback timeout.");

		Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

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
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (TradeDialogNotifyData *)handle->data;

	Handle<Value> argv[3];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
	if (data->m_result)
	{
		argv[1] = Nan::New(data->m_info.name).ToLocalChecked();
		argv[2] = Integer::New(isolate, data->m_info.level);
	}

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), (data->m_result) ? 3 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void TradeDialogTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

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
		Handle<Value> argv[1];
		argv[0] = Nan::Error("Async callback timeout.");

		Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

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
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (TradeStuffsNotifyData *)handle->data;

	if (data->m_result)
	{
		if(data->m_info.type == TRADE_STUFFS_ITEM)
		{
			Handle<Value> argv[3];
			Local<Value> nullValue = Nan::Null();
			argv[0] = nullValue;
			argv[1] = Nan::New(TRADE_STUFFS_ITEM);
			Local<Array> arr = Array::New(isolate);
			for (size_t i = 0; i < data->m_info.items.size(); ++i)
			{
				Local<Object> obj = Object::New(isolate);				
				obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(data->m_info.items[i].name).ToLocalChecked());
				obj->Set(String::NewFromUtf8(isolate, "attr"), Nan::New(data->m_info.items[i].attr).ToLocalChecked());
				obj->Set(String::NewFromUtf8(isolate, "itemid"), Integer::New(isolate, data->m_info.items[i].itemid));
				obj->Set(String::NewFromUtf8(isolate, "count"), Integer::New(isolate, data->m_info.items[i].count));
				obj->Set(String::NewFromUtf8(isolate, "pos"), Integer::New(isolate, data->m_info.items[i].pos));
				obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, data->m_info.items[i].level));
				obj->Set(String::NewFromUtf8(isolate, "type"), Integer::New(isolate, data->m_info.items[i].type));
				arr->Set(i, obj);
			}
			argv[2] = arr;
			Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 3, argv);
		}
		else if(data->m_info.type == TRADE_STUFFS_PET)
		{
			Handle<Value> argv[2];
			Local<Value> nullValue = Nan::Null();
			argv[0] = nullValue;
			argv[1] = Nan::New(TRADE_STUFFS_PET);
			Local<Object> obj = Object::New(isolate);
			obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, data->m_info.pet.index));
			obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(data->m_info.pet.name).ToLocalChecked());
			obj->Set(String::NewFromUtf8(isolate, "realname"), Nan::New(data->m_info.pet.realname).ToLocalChecked());
			obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, data->m_info.pet.level));
			obj->Set(String::NewFromUtf8(isolate, "race"), Integer::New(isolate, data->m_info.pet.race));
			obj->Set(String::NewFromUtf8(isolate, "maxhp"), Integer::New(isolate, data->m_info.pet.maxhp));
			obj->Set(String::NewFromUtf8(isolate, "maxmp"), Integer::New(isolate, data->m_info.pet.maxmp));
			obj->Set(String::NewFromUtf8(isolate, "loyality"), Integer::New(isolate, data->m_info.pet.loyality));
			obj->Set(String::NewFromUtf8(isolate, "skill_count"), Integer::New(isolate, data->m_info.pet.skill_count));
			obj->Set(String::NewFromUtf8(isolate, "image_id"), Integer::New(isolate, data->m_info.pet.image_id));

			Local<Object> objd = Object::New(isolate);
			objd->Set(String::NewFromUtf8(isolate, "points_remain"), Integer::New(isolate, data->m_info.pet.detail.points_remain));
			objd->Set(String::NewFromUtf8(isolate, "points_endurance"), Integer::New(isolate, data->m_info.pet.detail.points_endurance));
			objd->Set(String::NewFromUtf8(isolate, "points_strength"), Integer::New(isolate, data->m_info.pet.detail.points_strength));
			objd->Set(String::NewFromUtf8(isolate, "points_defense"), Integer::New(isolate, data->m_info.pet.detail.points_defense));
			objd->Set(String::NewFromUtf8(isolate, "points_agility"), Integer::New(isolate, data->m_info.pet.detail.points_agility));
			objd->Set(String::NewFromUtf8(isolate, "points_magical"), Integer::New(isolate, data->m_info.pet.detail.points_magical));
			objd->Set(String::NewFromUtf8(isolate, "value_attack"), Integer::New(isolate, data->m_info.pet.detail.value_attack));
			objd->Set(String::NewFromUtf8(isolate, "value_defensive"), Integer::New(isolate, data->m_info.pet.detail.value_defensive));
			objd->Set(String::NewFromUtf8(isolate, "value_agility"), Integer::New(isolate, data->m_info.pet.detail.value_agility));
			objd->Set(String::NewFromUtf8(isolate, "value_spirit"), Integer::New(isolate, data->m_info.pet.detail.value_spirit));
			objd->Set(String::NewFromUtf8(isolate, "value_recovery"), Integer::New(isolate, data->m_info.pet.detail.value_recovery));
			objd->Set(String::NewFromUtf8(isolate, "resist_poison"), Integer::New(isolate, data->m_info.pet.detail.resist_poison));
			objd->Set(String::NewFromUtf8(isolate, "resist_sleep"), Integer::New(isolate, data->m_info.pet.detail.resist_sleep));
			objd->Set(String::NewFromUtf8(isolate, "resist_medusa"), Integer::New(isolate, data->m_info.pet.detail.resist_medusa));
			objd->Set(String::NewFromUtf8(isolate, "resist_drunk"), Integer::New(isolate, data->m_info.pet.detail.resist_drunk));
			objd->Set(String::NewFromUtf8(isolate, "resist_chaos"), Integer::New(isolate, data->m_info.pet.detail.resist_chaos));
			objd->Set(String::NewFromUtf8(isolate, "resist_forget"), Integer::New(isolate, data->m_info.pet.detail.resist_forget));
			objd->Set(String::NewFromUtf8(isolate, "fix_critical"), Integer::New(isolate, data->m_info.pet.detail.fix_critical));
			objd->Set(String::NewFromUtf8(isolate, "fix_strikeback"), Integer::New(isolate, data->m_info.pet.detail.fix_strikeback));
			objd->Set(String::NewFromUtf8(isolate, "fix_accurancy"), Integer::New(isolate, data->m_info.pet.detail.fix_accurancy));
			objd->Set(String::NewFromUtf8(isolate, "fix_dodge"), Integer::New(isolate, data->m_info.pet.detail.fix_dodge));
			objd->Set(String::NewFromUtf8(isolate, "element_earth"), Integer::New(isolate, data->m_info.pet.detail.element_earth));
			objd->Set(String::NewFromUtf8(isolate, "element_water"), Integer::New(isolate, data->m_info.pet.detail.element_water));
			objd->Set(String::NewFromUtf8(isolate, "element_fire"), Integer::New(isolate, data->m_info.pet.detail.element_fire));
			objd->Set(String::NewFromUtf8(isolate, "element_wind"), Integer::New(isolate, data->m_info.pet.detail.element_wind));
			
			obj->Set(String::NewFromUtf8(isolate, "detail"), objd);

			argv[2] = obj;

			Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 3, argv);
		}
		else if (data->m_info.type == TRADE_STUFFS_PETSKILL) 
		{
			Handle<Value> argv[3];
			Local<Value> nullValue = Nan::Null();
			argv[0] = nullValue;
			argv[1] = Nan::New(TRADE_STUFFS_PETSKILL);

			Local<Object> obj = Object::New(isolate);
			obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, data->m_info.petskills.index));
			Local<Array> arr = Array::New(isolate);
			for (size_t i = 0; i < data->m_info.petskills.skills.size(); ++i)
			{
				arr->Set(i, Nan::New(data->m_info.petskills.skills[i]).ToLocalChecked());
			}
			obj->Set(String::NewFromUtf8(isolate, "skills"), arr);
			argv[2] = obj;

			Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 3, argv);
		}
		else if(data->m_info.type == TRADE_STUFFS_GOLD)
		{
			Handle<Value> argv[3];
			Local<Value> nullValue = Nan::Null();

			argv[0] = nullValue;
			argv[1] = Nan::New(TRADE_STUFFS_GOLD);
			argv[2] = Integer::New(isolate, data->m_info.gold);
			Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 3, argv);
		} else {
			Handle<Value> argv[1];
			argv[0] = Nan::Error("Unknown exception.");
			Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);
		}
	}
	else
	{
		Handle<Value> argv[1];
		argv[0] = Nan::Error("Unknown exception.");
		Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);
	}

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void TradeStuffsTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

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
		Handle<Value> argv[1];
		argv[0] = Nan::Error("Async callback timeout.");

		Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

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
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (PlayerMenuNotifyData *)handle->data;

	Handle<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
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
		argv[1] = arr;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void PlayerMenuTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

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
		Handle<Value> argv[1];
		argv[0] = Nan::Error("Async callback timeout.");

		Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

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
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

	auto data = (UnitMenuNotifyData *)handle->data;

	Handle<Value> argv[2];
	Local<Value> nullValue = Nan::Null();
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");
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
		argv[1] = arr;
	}

	Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), (data->m_result) ? 2 : 1, argv);

	data->m_callback.Reset();

	uv_timer_stop(data->m_timer);
	uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
	uv_close((uv_handle_t*)data->m_timer, FreeUVHandleCallBack);
	delete handle->data;
}

void UnitMenuTimerCallBack(uv_timer_t *handle)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);

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
		Handle<Value> argv[1];
		argv[0] = Nan::Error("Async callback timeout.");

		Local<Function>::New(isolate, data->m_callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

		data->m_callback.Reset();

		uv_timer_stop(handle);
		uv_close((uv_handle_t*)data->m_async, FreeUVHandleCallBack);
		uv_close((uv_handle_t*)handle, FreeUVHandleCallBack);
		delete handle->data;
	}
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

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be srcpos.");
		return;
	}
	
	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be dstpos.");
		return;
	}
	
	if (info.Length() < 3 || !info[2]->IsInt32()) {
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

void MovePet(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be srcpos.");
		return;
	}

	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be dstpos.");
		return;
	}

	int srcpos = (int)info[0]->IntegerValue();
	int dstpos = (int)info[1]->IntegerValue();

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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be gold.");
		return;
	}

	if (info.Length() < 2 || !info[1]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be opt.");
		return;
	}

	int gold = (int)info[0]->IntegerValue();
	int opt = (int)info[1]->IntegerValue();

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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be menu index.");
		return;
	}

	int menuindex = (int)info[0]->IntegerValue();

	std::string menustring;

	if (info.Length() >= 2) {
		if (info[1]->IsString())
		{
			v8::String::Utf8Value str(info[1]->ToString());
			menustring.assign(*str);
		}
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

void UpgradePlayer(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be attr index.");
		return;
	}

	int attr = (int)info[0]->IntegerValue();
	if (!g_CGAInterface->UpgradePlayer(attr))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(true);
}

void UpgradePet(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be petid.");
		return;
	}

	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be attr index.");
		return;
	}

	int petid = (int)info[0]->IntegerValue();
	int attr = (int)info[1]->IntegerValue();
	if (!g_CGAInterface->UpgradePet(petid, attr))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(true);
}

void DoRequest(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be request type.");
		return;
	}

	int request_type = (int)info[0]->IntegerValue();
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be type.");
		return;
	}
	
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be boolean.");
		return;
	}

	int type = (int)info[0]->IntegerValue();

	bool enable = info[1]->BooleanValue();

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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsArray()) {
		Nan::ThrowTypeError("Arg[0] must be item object array.");
		return;
	}
	
	if (info.Length() < 2 || !info[0]->IsArray()) {
		Nan::ThrowTypeError("Arg[1] must be pet id array.");
		return;
	}
	
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Arg[2] must be gold.");
		return;
	}

	Local<Object> objarr = Local<Object>::Cast(info[0]);
	uint32_t length = objarr->Get(Nan::New("length").ToLocalChecked())->Uint32Value();

	Local<Array> arr = Local<Array>::Cast(info[0]);
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
	
	Local<Object> objarr2 = Local<Object>::Cast(info[1]);
	uint32_t length2 = objarr2->Get(Nan::New("length").ToLocalChecked())->Uint32Value();

	Local<Array> arr2 = Local<Array>::Cast(info[1]);
	CGA::cga_sell_pets_t pets;
	for (uint32_t i = 0; i < length2; ++i)
	{
		auto val = (int)Local<Value>::Cast(arr2->Get(i))->IntegerValue();
		pets.emplace_back(val);
	}
	
	int gold = (int)info[2]->IntegerValue();
	if (!g_CGAInterface->TradeAddStuffs(items, pets, gold))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void AsyncWaitTradeStuffs(const Nan::FunctionCallbackInfo<v8::Value>& info)
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