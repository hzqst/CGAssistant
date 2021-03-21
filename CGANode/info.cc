#include <nan.h>
#include "../CGALib/gameinterface.h"

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

void IsInGame(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int ingame = 0;
	if (!g_CGAInterface->IsInGame(ingame))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(ingame);
}

void GetWorldStatus(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int status = 0;
	if (!g_CGAInterface->GetWorldStatus(status))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(status);
}

void GetGameStatus(const Nan::FunctionCallbackInfo<v8::Value>& info) 
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int status = 0;
	if (!g_CGAInterface->GetGameStatus(status))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(status);
}

void GetBGMIndex(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int status = 0;
	if (!g_CGAInterface->GetBGMIndex(status))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(status);
}

void GetSysTime(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	CGA::cga_sys_time_t myinfo;
	if (!g_CGAInterface->GetSysTime(myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "years").ToLocalChecked(), Integer::New(isolate, myinfo.years));
	obj->Set(context, String::NewFromUtf8(isolate, "month").ToLocalChecked(), Integer::New(isolate, myinfo.month));
	obj->Set(context, String::NewFromUtf8(isolate, "days").ToLocalChecked(), Integer::New(isolate, myinfo.days));
	obj->Set(context, String::NewFromUtf8(isolate, "hours").ToLocalChecked(), Integer::New(isolate, myinfo.hours));
	obj->Set(context, String::NewFromUtf8(isolate, "mins").ToLocalChecked(), Integer::New(isolate, myinfo.mins));
	obj->Set(context, String::NewFromUtf8(isolate, "secs").ToLocalChecked(), Integer::New(isolate, myinfo.secs));
	obj->Set(context, String::NewFromUtf8(isolate, "local_time").ToLocalChecked(), Integer::New(isolate, myinfo.local_time));
	obj->Set(context, String::NewFromUtf8(isolate, "server_time").ToLocalChecked(), Integer::New(isolate, myinfo.server_time));

	info.GetReturnValue().Set(obj);
}

void GetPlayerInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	CGA::cga_player_info_t myinfo;
	if (!g_CGAInterface->GetPlayerInfo(myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "hp").ToLocalChecked(), Integer::New(isolate, myinfo.hp));
	obj->Set(context, String::NewFromUtf8(isolate, "maxhp").ToLocalChecked(), Integer::New(isolate, myinfo.maxhp));
	obj->Set(context, String::NewFromUtf8(isolate, "mp").ToLocalChecked(), Integer::New(isolate, myinfo.mp));
	obj->Set(context, String::NewFromUtf8(isolate, "maxmp").ToLocalChecked(), Integer::New(isolate, myinfo.maxmp));
	obj->Set(context, String::NewFromUtf8(isolate, "xp").ToLocalChecked(), Integer::New(isolate, myinfo.xp));
	obj->Set(context, String::NewFromUtf8(isolate, "maxxp").ToLocalChecked(), Integer::New(isolate, myinfo.maxxp));
	obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
	obj->Set(context, String::NewFromUtf8(isolate, "health").ToLocalChecked(), Integer::New(isolate, myinfo.health));
	obj->Set(context, String::NewFromUtf8(isolate, "souls").ToLocalChecked(), Integer::New(isolate, myinfo.souls));
	obj->Set(context, String::NewFromUtf8(isolate, "gold").ToLocalChecked(), Integer::New(isolate, myinfo.gold));
	obj->Set(context, String::NewFromUtf8(isolate, "unitid").ToLocalChecked(), Integer::New(isolate, myinfo.unitid));
	obj->Set(context, String::NewFromUtf8(isolate, "petid").ToLocalChecked(), Integer::New(isolate, myinfo.petid));
	obj->Set(context, String::NewFromUtf8(isolate, "direction").ToLocalChecked(), Integer::New(isolate, myinfo.direction));
	obj->Set(context, String::NewFromUtf8(isolate, "battle_position").ToLocalChecked(), Integer::New(isolate, myinfo.battle_position));
	obj->Set(context, String::NewFromUtf8(isolate, "punchclock").ToLocalChecked(), Integer::New(isolate, myinfo.punchclock));
	obj->Set(context, String::NewFromUtf8(isolate, "usingpunchclock").ToLocalChecked(), Boolean::New(isolate, myinfo.usingpunchclock));
	obj->Set(context, String::NewFromUtf8(isolate, "petriding").ToLocalChecked(), Boolean::New(isolate, myinfo.petriding));
	obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "job").ToLocalChecked(), Nan::New(myinfo.job).ToLocalChecked());
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfo.titles.size(); ++i)
	{
		arr->Set(context, i, Nan::New(myinfo.titles[i]).ToLocalChecked());
	}
	obj->Set(context, String::NewFromUtf8(isolate, "titles").ToLocalChecked(), arr);

	Local<Object> objd = Object::New(isolate);
	objd->Set(context, String::NewFromUtf8(isolate, "points_remain").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_remain));
	objd->Set(context, String::NewFromUtf8(isolate, "points_endurance").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_endurance));
	objd->Set(context, String::NewFromUtf8(isolate, "points_strength").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_strength));
	objd->Set(context, String::NewFromUtf8(isolate, "points_defense").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_defense));
	objd->Set(context, String::NewFromUtf8(isolate, "points_agility").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_agility));
	objd->Set(context, String::NewFromUtf8(isolate, "points_magical").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_magical));
	objd->Set(context, String::NewFromUtf8(isolate, "value_attack").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_attack));
	objd->Set(context, String::NewFromUtf8(isolate, "value_defensive").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_defensive));
	objd->Set(context, String::NewFromUtf8(isolate, "value_agility").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_agility));
	objd->Set(context, String::NewFromUtf8(isolate, "value_spirit").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_spirit));
	objd->Set(context, String::NewFromUtf8(isolate, "value_recovery").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_recovery));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_poison").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_poison));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_sleep").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_sleep));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_medusa").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_medusa));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_drunk").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_drunk));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_chaos").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_chaos));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_forget").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_forget));
	objd->Set(context, String::NewFromUtf8(isolate, "fix_critical").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_critical));
	objd->Set(context, String::NewFromUtf8(isolate, "fix_strikeback").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_strikeback));
	objd->Set(context, String::NewFromUtf8(isolate, "fix_accurancy").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_accurancy));
	objd->Set(context, String::NewFromUtf8(isolate, "fix_dodge").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_dodge));
	objd->Set(context, String::NewFromUtf8(isolate, "element_earth").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_earth));
	objd->Set(context, String::NewFromUtf8(isolate, "element_water").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_water));
	objd->Set(context, String::NewFromUtf8(isolate, "element_fire").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_fire));
	objd->Set(context, String::NewFromUtf8(isolate, "element_wind").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_wind));

	objd->Set(context, String::NewFromUtf8(isolate, "manu_endurance").ToLocalChecked(), Integer::New(isolate, myinfo.manu_endurance));
	objd->Set(context, String::NewFromUtf8(isolate, "manu_skillful").ToLocalChecked(), Integer::New(isolate, myinfo.manu_skillful));
	objd->Set(context, String::NewFromUtf8(isolate, "manu_intelligence").ToLocalChecked(), Integer::New(isolate, myinfo.manu_intelligence));
	objd->Set(context, String::NewFromUtf8(isolate, "value_charisma").ToLocalChecked(), Integer::New(isolate, myinfo.value_charisma));

	obj->Set(context, String::NewFromUtf8(isolate, "detail").ToLocalChecked(), objd);

	Local<Object> objp = Object::New(isolate);
	objp->Set(context, String::NewFromUtf8(isolate, "sell_icon").ToLocalChecked(), Integer::New(isolate, myinfo.persdesc.sellIcon));
	objp->Set(context, String::NewFromUtf8(isolate, "sell_string").ToLocalChecked(), Nan::New(myinfo.persdesc.sellString).ToLocalChecked());
	objp->Set(context, String::NewFromUtf8(isolate, "buy_icon").ToLocalChecked(), Integer::New(isolate, myinfo.persdesc.buyIcon));
	objp->Set(context, String::NewFromUtf8(isolate, "buy_string").ToLocalChecked(), Nan::New(myinfo.persdesc.buyString).ToLocalChecked());
	objp->Set(context, String::NewFromUtf8(isolate, "want_icon").ToLocalChecked(), Integer::New(isolate, myinfo.persdesc.wantIcon));
	objp->Set(context, String::NewFromUtf8(isolate, "want_string").ToLocalChecked(), Nan::New(myinfo.persdesc.wantString).ToLocalChecked());
	objp->Set(context, String::NewFromUtf8(isolate, "desc_string").ToLocalChecked(), Nan::New(myinfo.persdesc.descString).ToLocalChecked());

	obj->Set(context, String::NewFromUtf8(isolate, "persdesc").ToLocalChecked(), objp);

	info.GetReturnValue().Set(obj);
}

void SetPlayerFlagEnabled(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	int index = info[0]->Int32Value(context).ToChecked();
	bool enable = info[1]->BooleanValue(isolate);
	if (!g_CGAInterface->SetPlayerFlagEnabled(index, enable))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(true);
}

void IsPlayerFlagEnabled(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int index = info[0]->Int32Value(context).ToChecked();

	bool enable = false;

	if (!g_CGAInterface->IsPlayerFlagEnabled(index, enable))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(enable);
}

void IsUIDialogPresent(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int index = info[0]->Int32Value(context).ToChecked();
	bool present = false;
	if (!g_CGAInterface->IsUIDialogPresent(index, present))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(present);
}

void IsSkillValid(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int skillid = info[0]->Int32Value(context).ToChecked();

	bool valid = false;

	if (!g_CGAInterface->IsSkillValid(skillid, valid))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(valid);
}

void GetSkillInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	CGA::cga_skill_info_t myinfo;

	int skillid = info[0]->Int32Value(context).ToChecked();

	if (!g_CGAInterface->GetSkillInfo(skillid, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "lv").ToLocalChecked(), Integer::New(isolate, myinfo.lv));
	obj->Set(context, String::NewFromUtf8(isolate, "maxlv").ToLocalChecked(), Integer::New(isolate, myinfo.maxlv));
	obj->Set(context, String::NewFromUtf8(isolate, "xp").ToLocalChecked(), Integer::New(isolate, myinfo.xp));
	obj->Set(context, String::NewFromUtf8(isolate, "maxxp").ToLocalChecked(), Integer::New(isolate, myinfo.maxxp));
	obj->Set(context, String::NewFromUtf8(isolate, "skill_id").ToLocalChecked(), Integer::New(isolate, myinfo.skill_id));
	obj->Set(context, String::NewFromUtf8(isolate, "type").ToLocalChecked(), Integer::New(isolate, myinfo.type));
	obj->Set(context, String::NewFromUtf8(isolate, "pos").ToLocalChecked(), Integer::New(isolate, myinfo.pos));
	obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, myinfo.index));
	obj->Set(context, String::NewFromUtf8(isolate, "slotsize").ToLocalChecked(), Integer::New(isolate, myinfo.slotsize));
	info.GetReturnValue().Set(obj);
}

void GetSkillsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	CGA::cga_skills_info_t myinfos;
	if (!g_CGAInterface->GetSkillsInfo(myinfos))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfos.size(); ++i)
	{
		Local<Object> obj = Object::New(isolate);
		const CGA::cga_skill_info_t &myinfo = myinfos.at(i);
;		obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "lv").ToLocalChecked(), Integer::New(isolate, myinfo.lv));
		obj->Set(context, String::NewFromUtf8(isolate, "maxlv").ToLocalChecked(), Integer::New(isolate, myinfo.maxlv));
		obj->Set(context, String::NewFromUtf8(isolate, "xp").ToLocalChecked(), Integer::New(isolate, myinfo.xp));
		obj->Set(context, String::NewFromUtf8(isolate, "maxxp").ToLocalChecked(), Integer::New(isolate, myinfo.maxxp));
		obj->Set(context, String::NewFromUtf8(isolate, "skill_id").ToLocalChecked(), Integer::New(isolate, myinfo.skill_id));
		obj->Set(context, String::NewFromUtf8(isolate, "type").ToLocalChecked(), Integer::New(isolate, myinfo.type));
		obj->Set(context, String::NewFromUtf8(isolate, "pos").ToLocalChecked(), Integer::New(isolate, myinfo.pos));
		obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, myinfo.index));
		obj->Set(context, String::NewFromUtf8(isolate, "slotsize").ToLocalChecked(), Integer::New(isolate, myinfo.slotsize));
		arr->Set(context, i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void GetCraftInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	int skillid = info[0]->Int32Value(context).ToChecked();
	int subskill_index = info[1]->Int32Value(context).ToChecked();

	CGA::cga_craft_info_t myinfo;
	if (!g_CGAInterface->GetCraftInfo(skillid, subskill_index, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Nan::New(myinfo.info).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "itemid").ToLocalChecked(), Integer::New(isolate, myinfo.itemid));
	obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
	obj->Set(context, String::NewFromUtf8(isolate, "cost").ToLocalChecked(), Integer::New(isolate, myinfo.cost));
	obj->Set(context, String::NewFromUtf8(isolate, "id").ToLocalChecked(), Integer::New(isolate, myinfo.id));
	obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, myinfo.index));
	obj->Set(context, String::NewFromUtf8(isolate, "available").ToLocalChecked(), Boolean::New(isolate, myinfo.available));

	Local<Array> mat_arr = Array::New(isolate);
	for (size_t i = 0; i < 5; ++i)
	{
		Local<Object> mat_obj = Object::New(isolate);
		const CGA::cga_craft_material_t &mymat = myinfo.materials[i];
		if (!mymat.name.empty())
		{
			mat_obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(mymat.name).ToLocalChecked());
			mat_obj->Set(context, String::NewFromUtf8(isolate, "itemid").ToLocalChecked(), Integer::New(isolate, mymat.itemid));
			mat_obj->Set(context, String::NewFromUtf8(isolate, "count").ToLocalChecked(), Integer::New(isolate, mymat.count));
			mat_arr->Set(context, i, mat_obj);
		}
	}
	obj->Set(context, String::NewFromUtf8(isolate, "materials").ToLocalChecked(), mat_arr);

	info.GetReturnValue().Set(obj);
}

void GetCraftsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32())
	{
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int skillid = info[0]->Int32Value(context).ToChecked();

	CGA::cga_crafts_info_t myinfos;
	if (!g_CGAInterface->GetCraftsInfo(skillid, myinfos))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfos.size(); ++i)
	{
		const CGA::cga_craft_info_t &myinfo = myinfos.at(i);
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "itemid").ToLocalChecked(), Integer::New(isolate, myinfo.itemid));
		obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
		obj->Set(context, String::NewFromUtf8(isolate, "cost").ToLocalChecked(), Integer::New(isolate, myinfo.cost));
		obj->Set(context, String::NewFromUtf8(isolate, "id").ToLocalChecked(), Integer::New(isolate, myinfo.id));
		obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, myinfo.index));
		obj->Set(context, String::NewFromUtf8(isolate, "available").ToLocalChecked(), Boolean::New(isolate, myinfo.available));

		Local<Array> mat_arr = Array::New(isolate);
		for (size_t j = 0; j < 5; ++j)
		{
			Local<Object> mat_obj = Object::New(isolate);
			const CGA::cga_craft_material_t &mymat = myinfo.materials[j];
			if (!mymat.name.empty())
			{
				mat_obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(mymat.name).ToLocalChecked());
				mat_obj->Set(context, String::NewFromUtf8(isolate, "itemid").ToLocalChecked(), Integer::New(isolate, mymat.itemid));
				mat_obj->Set(context, String::NewFromUtf8(isolate, "count").ToLocalChecked(), Integer::New(isolate, mymat.count));
				mat_arr->Set(context, j, mat_obj);
			}
		}
		obj->Set(context, String::NewFromUtf8(isolate, "materials").ToLocalChecked(), mat_arr);

		arr->Set(context, i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void GetSubSkillInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

	int skillid = info[0]->Int32Value(context).ToChecked();
	int stage = info[1]->Int32Value(context).ToChecked();

	CGA::cga_subskill_info_t myinfo;
	if (!g_CGAInterface->GetSubSkillInfo(skillid, stage, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Nan::New(myinfo.info).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
	obj->Set(context, String::NewFromUtf8(isolate, "cost").ToLocalChecked(), Integer::New(isolate, myinfo.cost));
	obj->Set(context, String::NewFromUtf8(isolate, "flags").ToLocalChecked(), Integer::New(isolate, myinfo.flags));
	obj->Set(context, String::NewFromUtf8(isolate, "available").ToLocalChecked(), Boolean::New(isolate, myinfo.available));
	info.GetReturnValue().Set(obj);
}

void GetSubSkillsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int skillid = info[0]->Int32Value(context).ToChecked();

	CGA::cga_subskills_info_t myinfos;
	if (!g_CGAInterface->GetSubSkillsInfo(skillid, myinfos))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfos.size(); ++i)
	{
		const CGA::cga_subskill_info_t &myinfo = myinfos.at(i);
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
		obj->Set(context, String::NewFromUtf8(isolate, "cost").ToLocalChecked(), Integer::New(isolate, myinfo.cost));
		obj->Set(context, String::NewFromUtf8(isolate, "flags").ToLocalChecked(), Integer::New(isolate, myinfo.flags));
		obj->Set(context, String::NewFromUtf8(isolate, "available").ToLocalChecked(), Boolean::New(isolate, myinfo.available));
		arr->Set(context, i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void IsItemValid(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int itempos = info[0]->Int32Value(context).ToChecked();

	bool valid = false;

	if (!g_CGAInterface->IsItemValid(itempos, valid))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(valid);
}

void GetItemInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int itempos = info[0]->Int32Value(context).ToChecked();

	CGA::cga_item_info_t myinfo;
	if (!g_CGAInterface->GetItemInfo(itempos, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Nan::New(myinfo.info).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "attr").ToLocalChecked(), Nan::New(myinfo.attr).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "count").ToLocalChecked(), Integer::New(isolate, myinfo.count));
	obj->Set(context, String::NewFromUtf8(isolate, "itemid").ToLocalChecked(), Integer::New(isolate, myinfo.itemid));
	obj->Set(context, String::NewFromUtf8(isolate, "pos").ToLocalChecked(), Integer::New(isolate, myinfo.pos));
	obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
	obj->Set(context, String::NewFromUtf8(isolate, "type").ToLocalChecked(), Integer::New(isolate, myinfo.type));
	obj->Set(context, String::NewFromUtf8(isolate, "assessed").ToLocalChecked(), Boolean::New(isolate, myinfo.assessed));
	info.GetReturnValue().Set(obj);
}

void GetItemsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	CGA::cga_items_info_t myinfos;
	if (!g_CGAInterface->GetItemsInfo(myinfos))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfos.size(); ++i)
	{
		const CGA::cga_item_info_t &myinfo = myinfos.at(i);
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "attr").ToLocalChecked(), Nan::New(myinfo.attr).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "count").ToLocalChecked(), Integer::New(isolate, myinfo.count));
		obj->Set(context, String::NewFromUtf8(isolate, "itemid").ToLocalChecked(), Integer::New(isolate, myinfo.itemid));
		obj->Set(context, String::NewFromUtf8(isolate, "pos").ToLocalChecked(), Integer::New(isolate, myinfo.pos));
		obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
		obj->Set(context, String::NewFromUtf8(isolate, "type").ToLocalChecked(), Integer::New(isolate, myinfo.type));
		obj->Set(context, String::NewFromUtf8(isolate, "assessed").ToLocalChecked(), Boolean::New(isolate, myinfo.assessed));
		arr->Set(context, i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void GetBankItemsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	CGA::cga_items_info_t myinfos;
	if (!g_CGAInterface->GetBankItemsInfo(myinfos))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfos.size(); ++i)
	{
		const CGA::cga_item_info_t &myinfo = myinfos.at(i);
		Local<Object> obj = Object::New(isolate);
		obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "attr").ToLocalChecked(), Nan::New(myinfo.attr).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "count").ToLocalChecked(), Integer::New(isolate, myinfo.count));
		obj->Set(context, String::NewFromUtf8(isolate, "itemid").ToLocalChecked(), Integer::New(isolate, myinfo.itemid));
		obj->Set(context, String::NewFromUtf8(isolate, "pos").ToLocalChecked(), Integer::New(isolate, myinfo.pos));
		obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
		obj->Set(context, String::NewFromUtf8(isolate, "type").ToLocalChecked(), Integer::New(isolate, myinfo.type));
		obj->Set(context, String::NewFromUtf8(isolate, "assessed").ToLocalChecked(), Boolean::New(isolate, myinfo.assessed));
		arr->Set(context, i, obj);
	}
	info.GetReturnValue().Set(arr);
}


void GetBankGold(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	int gold = 0;

	if (!g_CGAInterface->GetBankGold(gold))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(gold);
}

void IsPetValid(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int petid = info[0]->Int32Value(context).ToChecked();
	bool valid = false;
	if (!g_CGAInterface->IsPetValid(petid, valid))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(valid);
}

void GetPetInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int petid = info[0]->Int32Value(context).ToChecked();
	CGA::cga_pet_info_t myinfo;
	if (!g_CGAInterface->GetPetInfo(petid, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "realname").ToLocalChecked(), Nan::New(myinfo.realname).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
	obj->Set(context, String::NewFromUtf8(isolate, "race").ToLocalChecked(), Integer::New(isolate, myinfo.race));
	obj->Set(context, String::NewFromUtf8(isolate, "loyality").ToLocalChecked(), Integer::New(isolate, myinfo.loyality));
	obj->Set(context, String::NewFromUtf8(isolate, "health").ToLocalChecked(), Integer::New(isolate, myinfo.health));
	obj->Set(context, String::NewFromUtf8(isolate, "hp").ToLocalChecked(), Integer::New(isolate, myinfo.hp));
	obj->Set(context, String::NewFromUtf8(isolate, "maxhp").ToLocalChecked(), Integer::New(isolate, myinfo.maxhp));
	obj->Set(context, String::NewFromUtf8(isolate, "mp").ToLocalChecked(), Integer::New(isolate, myinfo.mp));
	obj->Set(context, String::NewFromUtf8(isolate, "maxmp").ToLocalChecked(), Integer::New(isolate, myinfo.maxmp));
	obj->Set(context, String::NewFromUtf8(isolate, "xp").ToLocalChecked(), Integer::New(isolate, myinfo.xp));
	obj->Set(context, String::NewFromUtf8(isolate, "maxxp").ToLocalChecked(), Integer::New(isolate, myinfo.maxxp));
	obj->Set(context, String::NewFromUtf8(isolate, "flags").ToLocalChecked(), Integer::New(isolate, myinfo.flags));
	obj->Set(context, String::NewFromUtf8(isolate, "battle_flags").ToLocalChecked(), Integer::New(isolate, myinfo.battle_flags));
	obj->Set(context, String::NewFromUtf8(isolate, "state").ToLocalChecked(), Integer::New(isolate, myinfo.state));
	obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, myinfo.index));

	Local<Object> objd = Object::New(isolate);
	objd->Set(context, String::NewFromUtf8(isolate, "points_remain").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_remain));
	objd->Set(context, String::NewFromUtf8(isolate, "points_endurance").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_endurance));
	objd->Set(context, String::NewFromUtf8(isolate, "points_strength").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_strength));
	objd->Set(context, String::NewFromUtf8(isolate, "points_defense").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_defense));
	objd->Set(context, String::NewFromUtf8(isolate, "points_agility").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_agility));
	objd->Set(context, String::NewFromUtf8(isolate, "points_magical").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_magical));
	objd->Set(context, String::NewFromUtf8(isolate, "value_attack").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_attack));
	objd->Set(context, String::NewFromUtf8(isolate, "value_defensive").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_defensive));
	objd->Set(context, String::NewFromUtf8(isolate, "value_agility").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_agility));
	objd->Set(context, String::NewFromUtf8(isolate, "value_spirit").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_spirit));
	objd->Set(context, String::NewFromUtf8(isolate, "value_recovery").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_recovery));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_poison").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_poison));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_sleep").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_sleep));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_medusa").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_medusa));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_drunk").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_drunk));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_chaos").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_chaos));
	objd->Set(context, String::NewFromUtf8(isolate, "resist_forget").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_forget));
	objd->Set(context, String::NewFromUtf8(isolate, "fix_critical").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_critical));
	objd->Set(context, String::NewFromUtf8(isolate, "fix_strikeback").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_strikeback));
	objd->Set(context, String::NewFromUtf8(isolate, "fix_accurancy").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_accurancy));
	objd->Set(context, String::NewFromUtf8(isolate, "fix_dodge").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_dodge));
	objd->Set(context, String::NewFromUtf8(isolate, "element_earth").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_earth));
	objd->Set(context, String::NewFromUtf8(isolate, "element_water").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_water));
	objd->Set(context, String::NewFromUtf8(isolate, "element_fire").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_fire));
	objd->Set(context, String::NewFromUtf8(isolate, "element_wind").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_wind));
	obj->Set(context, String::NewFromUtf8(isolate, "detail").ToLocalChecked(), objd);

	info.GetReturnValue().Set(obj);
}

void GetPetsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	CGA::cga_pets_info_t myinfos;
	if (!g_CGAInterface->GetPetsInfo(myinfos))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfos.size(); ++i)
	{
		Local<Object> obj = Object::New(isolate);
		const CGA::cga_pet_info_t &myinfo = myinfos.at(i);
		obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "realname").ToLocalChecked(), Nan::New(myinfo.realname).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
		obj->Set(context, String::NewFromUtf8(isolate, "race").ToLocalChecked(), Integer::New(isolate, myinfo.race));
		obj->Set(context, String::NewFromUtf8(isolate, "loyality").ToLocalChecked(), Integer::New(isolate, myinfo.loyality));
		obj->Set(context, String::NewFromUtf8(isolate, "health").ToLocalChecked(), Integer::New(isolate, myinfo.health));
		obj->Set(context, String::NewFromUtf8(isolate, "hp").ToLocalChecked(), Integer::New(isolate, myinfo.hp));
		obj->Set(context, String::NewFromUtf8(isolate, "maxhp").ToLocalChecked(), Integer::New(isolate, myinfo.maxhp));
		obj->Set(context, String::NewFromUtf8(isolate, "mp").ToLocalChecked(), Integer::New(isolate, myinfo.mp));
		obj->Set(context, String::NewFromUtf8(isolate, "maxmp").ToLocalChecked(), Integer::New(isolate, myinfo.maxmp));
		obj->Set(context, String::NewFromUtf8(isolate, "xp").ToLocalChecked(), Integer::New(isolate, myinfo.xp));
		obj->Set(context, String::NewFromUtf8(isolate, "maxxp").ToLocalChecked(), Integer::New(isolate, myinfo.maxxp));
		obj->Set(context, String::NewFromUtf8(isolate, "flags").ToLocalChecked(), Integer::New(isolate, myinfo.flags));
		obj->Set(context, String::NewFromUtf8(isolate, "battle_flags").ToLocalChecked(), Integer::New(isolate, myinfo.battle_flags));
		obj->Set(context, String::NewFromUtf8(isolate, "state").ToLocalChecked(), Integer::New(isolate, myinfo.state));
		obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, myinfo.index));

		Local<Object> objd = Object::New(isolate);
		objd->Set(context, String::NewFromUtf8(isolate, "points_remain").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_remain));
		objd->Set(context, String::NewFromUtf8(isolate, "points_endurance").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_endurance));
		objd->Set(context, String::NewFromUtf8(isolate, "points_strength").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_strength));
		objd->Set(context, String::NewFromUtf8(isolate, "points_defense").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_defense));
		objd->Set(context, String::NewFromUtf8(isolate, "points_agility").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_agility));
		objd->Set(context, String::NewFromUtf8(isolate, "points_magical").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_magical));
		objd->Set(context, String::NewFromUtf8(isolate, "value_attack").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_attack));
		objd->Set(context, String::NewFromUtf8(isolate, "value_defensive").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_defensive));
		objd->Set(context, String::NewFromUtf8(isolate, "value_agility").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_agility));
		objd->Set(context, String::NewFromUtf8(isolate, "value_spirit").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_spirit));
		objd->Set(context, String::NewFromUtf8(isolate, "value_recovery").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_recovery));
		objd->Set(context, String::NewFromUtf8(isolate, "resist_poison").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_poison));
		objd->Set(context, String::NewFromUtf8(isolate, "resist_sleep").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_sleep));
		objd->Set(context, String::NewFromUtf8(isolate, "resist_medusa").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_medusa));
		objd->Set(context, String::NewFromUtf8(isolate, "resist_drunk").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_drunk));
		objd->Set(context, String::NewFromUtf8(isolate, "resist_chaos").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_chaos));
		objd->Set(context, String::NewFromUtf8(isolate, "resist_forget").ToLocalChecked(), Integer::New(isolate, myinfo.detail.resist_forget));
		objd->Set(context, String::NewFromUtf8(isolate, "fix_critical").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_critical));
		objd->Set(context, String::NewFromUtf8(isolate, "fix_strikeback").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_strikeback));
		objd->Set(context, String::NewFromUtf8(isolate, "fix_accurancy").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_accurancy));
		objd->Set(context, String::NewFromUtf8(isolate, "fix_dodge").ToLocalChecked(), Integer::New(isolate, myinfo.detail.fix_dodge));
		objd->Set(context, String::NewFromUtf8(isolate, "element_earth").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_earth));
		objd->Set(context, String::NewFromUtf8(isolate, "element_water").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_water));
		objd->Set(context, String::NewFromUtf8(isolate, "element_fire").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_fire));
		objd->Set(context, String::NewFromUtf8(isolate, "element_wind").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_wind));
		obj->Set(context, String::NewFromUtf8(isolate, "detail").ToLocalChecked(), objd);

		arr->Set(context, i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void GetBankPetsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	CGA::cga_pets_info_t myinfos;
	if (!g_CGAInterface->GetBankPetsInfo(myinfos))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfos.size(); ++i)
	{
		Local<Object> obj = Object::New(isolate);
		const CGA::cga_pet_info_t &myinfo = myinfos.at(i);
		obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "realname").ToLocalChecked(), Nan::New(myinfo.realname).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "level").ToLocalChecked(), Integer::New(isolate, myinfo.level));
		obj->Set(context, String::NewFromUtf8(isolate, "race").ToLocalChecked(), Integer::New(isolate, myinfo.race));
		obj->Set(context, String::NewFromUtf8(isolate, "loyality").ToLocalChecked(), Integer::New(isolate, myinfo.loyality));
		obj->Set(context, String::NewFromUtf8(isolate, "maxhp").ToLocalChecked(), Integer::New(isolate, myinfo.maxhp));
		obj->Set(context, String::NewFromUtf8(isolate, "maxmp").ToLocalChecked(), Integer::New(isolate, myinfo.maxmp));
		obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, myinfo.index));

		Local<Object> objd = Object::New(isolate);
		objd->Set(context, String::NewFromUtf8(isolate, "points_endurance").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_endurance));
		objd->Set(context, String::NewFromUtf8(isolate, "points_strength").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_strength));
		objd->Set(context, String::NewFromUtf8(isolate, "points_defense").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_defense));
		objd->Set(context, String::NewFromUtf8(isolate, "points_agility").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_agility));
		objd->Set(context, String::NewFromUtf8(isolate, "points_magical").ToLocalChecked(), Integer::New(isolate, myinfo.detail.points_magical));
		objd->Set(context, String::NewFromUtf8(isolate, "value_attack").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_attack));
		objd->Set(context, String::NewFromUtf8(isolate, "value_defensive").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_defensive));
		objd->Set(context, String::NewFromUtf8(isolate, "value_agility").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_agility));
		objd->Set(context, String::NewFromUtf8(isolate, "value_spirit").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_spirit));
		objd->Set(context, String::NewFromUtf8(isolate, "value_recovery").ToLocalChecked(), Integer::New(isolate, myinfo.detail.value_recovery));
		objd->Set(context, String::NewFromUtf8(isolate, "element_earth").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_earth));
		objd->Set(context, String::NewFromUtf8(isolate, "element_water").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_water));
		objd->Set(context, String::NewFromUtf8(isolate, "element_fire").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_fire));
		objd->Set(context, String::NewFromUtf8(isolate, "element_wind").ToLocalChecked(), Integer::New(isolate, myinfo.detail.element_wind));
		obj->Set(context, String::NewFromUtf8(isolate, "detail").ToLocalChecked(), objd);

		arr->Set(context, i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void IsPetSkillValid(const Nan::FunctionCallbackInfo<v8::Value>& info)
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
	int skillid = info[1]->Int32Value(context).ToChecked();
	bool valid = false;
	if (!g_CGAInterface->IsPetSkillValid(petid, skillid, valid))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(valid);
}

void GetPetSkillInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}
	if (info.Length() < 2 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[1] must be integer.");
		return;
	}

	int petid = info[0]->Int32Value(context).ToChecked();
	int skillid = info[1]->Int32Value(context).ToChecked();
	CGA::cga_pet_skill_info_t myinfo;
	if (!g_CGAInterface->GetPetSkillInfo(petid, skillid, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Nan::New(myinfo.info).ToLocalChecked());
	obj->Set(context, String::NewFromUtf8(isolate, "cost").ToLocalChecked(), Integer::New(isolate, myinfo.cost));
	obj->Set(context, String::NewFromUtf8(isolate, "flags").ToLocalChecked(), Integer::New(isolate, myinfo.flags));
	obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, myinfo.index));
	info.GetReturnValue().Set(obj);
}

void GetPetSkillsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int petid = info[0]->Int32Value(context).ToChecked();
	CGA::cga_pet_skills_info_t myinfos;
	if (!g_CGAInterface->GetPetSkillsInfo(petid, myinfos))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfos.size(); ++i)
	{
		Local<Object> obj = Object::New(isolate);
		const CGA::cga_pet_skill_info_t &myinfo = myinfos.at(i);
		obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "info").ToLocalChecked(), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(context, String::NewFromUtf8(isolate, "cost").ToLocalChecked(), Integer::New(isolate, myinfo.cost));
		obj->Set(context, String::NewFromUtf8(isolate, "flags").ToLocalChecked(), Integer::New(isolate, myinfo.flags));
		obj->Set(context, String::NewFromUtf8(isolate, "index").ToLocalChecked(), Integer::New(isolate, myinfo.index));
		arr->Set(context, i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void GetTeamPlayerInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

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
		obj->Set(context, String::NewFromUtf8(isolate, "unit_id").ToLocalChecked(), Integer::New(isolate, plinfo[i].unit_id));
		obj->Set(context, String::NewFromUtf8(isolate, "hp").ToLocalChecked(), Integer::New(isolate, plinfo[i].hp));
		obj->Set(context, String::NewFromUtf8(isolate, "maxhp").ToLocalChecked(), Integer::New(isolate, plinfo[i].maxhp));
		obj->Set(context, String::NewFromUtf8(isolate, "mp").ToLocalChecked(), Integer::New(isolate, plinfo[i].mp));
		obj->Set(context, String::NewFromUtf8(isolate, "maxmp").ToLocalChecked(), Integer::New(isolate, plinfo[i].maxmp));
		obj->Set(context, String::NewFromUtf8(isolate, "xpos").ToLocalChecked(), Integer::New(isolate, plinfo[i].xpos));
		obj->Set(context, String::NewFromUtf8(isolate, "ypos").ToLocalChecked(), Integer::New(isolate, plinfo[i].ypos));
		obj->Set(context, String::NewFromUtf8(isolate, "name").ToLocalChecked(), Nan::New(plinfo[i].name).ToLocalChecked());
		arr->Set(context, i, obj);
	}

	info.GetReturnValue().Set(arr);
}