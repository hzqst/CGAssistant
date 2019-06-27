#include <nan.h>
#include "../CGALib/gameinterface.h"

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

void IsInGame(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	int status = 0;
	if (!g_CGAInterface->GetGameStatus(status))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(status);
}

void GetPlayerInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	CGA::cga_player_info_t myinfo;
	if (!g_CGAInterface->GetPlayerInfo(myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "hp"), Integer::New(isolate, myinfo.hp));
	obj->Set(String::NewFromUtf8(isolate, "maxhp"), Integer::New(isolate, myinfo.maxhp));
	obj->Set(String::NewFromUtf8(isolate, "mp"), Integer::New(isolate, myinfo.mp));
	obj->Set(String::NewFromUtf8(isolate, "maxmp"), Integer::New(isolate, myinfo.maxmp));
	obj->Set(String::NewFromUtf8(isolate, "xp"), Integer::New(isolate, myinfo.xp));
	obj->Set(String::NewFromUtf8(isolate, "maxxp"), Integer::New(isolate, myinfo.maxxp));
	obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
	obj->Set(String::NewFromUtf8(isolate, "health"), Integer::New(isolate, myinfo.health));
	obj->Set(String::NewFromUtf8(isolate, "souls"), Integer::New(isolate, myinfo.souls));
	obj->Set(String::NewFromUtf8(isolate, "gold"), Integer::New(isolate, myinfo.gold));
	obj->Set(String::NewFromUtf8(isolate, "unitid"), Integer::New(isolate, myinfo.unitid));
	obj->Set(String::NewFromUtf8(isolate, "petid"), Integer::New(isolate, myinfo.petid));
	obj->Set(String::NewFromUtf8(isolate, "punchclock"), Integer::New(isolate, myinfo.punchclock));
	obj->Set(String::NewFromUtf8(isolate, "usingpunchclock"), Boolean::New(isolate, myinfo.usingpunchclock));
	obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "job"), Nan::New(myinfo.job).ToLocalChecked());
	Local<Array> arr = Array::New(isolate);
	for (size_t i = 0; i < myinfo.titles.size(); ++i)
	{
		arr->Set(i, Nan::New(myinfo.titles[i]).ToLocalChecked());
	}
	obj->Set(String::NewFromUtf8(isolate, "titles"), arr);

	Local<Object> objd = Object::New(isolate);
	objd->Set(String::NewFromUtf8(isolate, "points_remain"), Integer::New(isolate, myinfo.detail.points_remain));
	objd->Set(String::NewFromUtf8(isolate, "points_endurance"), Integer::New(isolate, myinfo.detail.points_endurance));
	objd->Set(String::NewFromUtf8(isolate, "points_strength"), Integer::New(isolate, myinfo.detail.points_strength));
	objd->Set(String::NewFromUtf8(isolate, "points_defense"), Integer::New(isolate, myinfo.detail.points_defense));
	objd->Set(String::NewFromUtf8(isolate, "points_agility"), Integer::New(isolate, myinfo.detail.points_agility));
	objd->Set(String::NewFromUtf8(isolate, "points_magical"), Integer::New(isolate, myinfo.detail.points_magical));
	objd->Set(String::NewFromUtf8(isolate, "value_attack"), Integer::New(isolate, myinfo.detail.value_attack));
	objd->Set(String::NewFromUtf8(isolate, "value_defensive"), Integer::New(isolate, myinfo.detail.value_defensive));
	objd->Set(String::NewFromUtf8(isolate, "value_agility"), Integer::New(isolate, myinfo.detail.value_agility));
	objd->Set(String::NewFromUtf8(isolate, "value_spirit"), Integer::New(isolate, myinfo.detail.value_spirit));
	objd->Set(String::NewFromUtf8(isolate, "value_recovery"), Integer::New(isolate, myinfo.detail.value_recovery));
	objd->Set(String::NewFromUtf8(isolate, "resist_poison"), Integer::New(isolate, myinfo.detail.resist_poison));
	objd->Set(String::NewFromUtf8(isolate, "resist_sleep"), Integer::New(isolate, myinfo.detail.resist_sleep));
	objd->Set(String::NewFromUtf8(isolate, "resist_medusa"), Integer::New(isolate, myinfo.detail.resist_medusa));
	objd->Set(String::NewFromUtf8(isolate, "resist_drunk"), Integer::New(isolate, myinfo.detail.resist_drunk));
	objd->Set(String::NewFromUtf8(isolate, "resist_chaos"), Integer::New(isolate, myinfo.detail.resist_chaos));
	objd->Set(String::NewFromUtf8(isolate, "resist_forget"), Integer::New(isolate, myinfo.detail.resist_forget));
	objd->Set(String::NewFromUtf8(isolate, "fix_critical"), Integer::New(isolate, myinfo.detail.fix_critical));
	objd->Set(String::NewFromUtf8(isolate, "fix_strikeback"), Integer::New(isolate, myinfo.detail.fix_strikeback));
	objd->Set(String::NewFromUtf8(isolate, "fix_accurancy"), Integer::New(isolate, myinfo.detail.fix_accurancy));
	objd->Set(String::NewFromUtf8(isolate, "fix_dodge"), Integer::New(isolate, myinfo.detail.fix_dodge));
	objd->Set(String::NewFromUtf8(isolate, "element_earth"), Integer::New(isolate, myinfo.detail.element_earth));
	objd->Set(String::NewFromUtf8(isolate, "element_water"), Integer::New(isolate, myinfo.detail.element_water));
	objd->Set(String::NewFromUtf8(isolate, "element_fire"), Integer::New(isolate, myinfo.detail.element_fire));
	objd->Set(String::NewFromUtf8(isolate, "element_wind"), Integer::New(isolate, myinfo.detail.element_wind));

	objd->Set(String::NewFromUtf8(isolate, "manu_endurance"), Integer::New(isolate, myinfo.manu_endurance));
	objd->Set(String::NewFromUtf8(isolate, "manu_skillful"), Integer::New(isolate, myinfo.manu_skillful));
	objd->Set(String::NewFromUtf8(isolate, "manu_intelligence"), Integer::New(isolate, myinfo.manu_intelligence));

	obj->Set(String::NewFromUtf8(isolate, "detail"), objd);

	info.GetReturnValue().Set(obj);
}

void SetPlayerFlagEnabled(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || info[0]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[0] must be index.");
		return;
	}

	if (info.Length() < 2 || info[1]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[1] must be boolean.");
		return;
	}

	int index = (int)info[0]->IntegerValue();
	bool enable = info[1]->BooleanValue();
	if (!g_CGAInterface->SetPlayerFlagEnabled(index, enable))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(true);
}

void IsPlayerFlagEnabled(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || info[0]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[0] must be index.");
		return;
	}

	if (info.Length() < 2 || info[1]->IsBoolean()) {
		Nan::ThrowTypeError("Arg[1] must be boolean.");
		return;
	}

	int index = (int)info[0]->IntegerValue();
	bool enable = false;
	if (!g_CGAInterface->IsPlayerFlagEnabled(index, enable))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(enable);
}

void IsSkillValid(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be skillid.");
		return;
	}

	int skillid = (int)info[0]->IntegerValue();
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be skillid.");
		return;
	}

	int skillid = (int)info[0]->IntegerValue();
	CGA::cga_skill_info_t myinfo;
	if (!g_CGAInterface->GetSkillInfo(skillid, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "lv"), Integer::New(isolate, myinfo.lv));
	obj->Set(String::NewFromUtf8(isolate, "maxlv"), Integer::New(isolate, myinfo.maxlv));
	obj->Set(String::NewFromUtf8(isolate, "xp"), Integer::New(isolate, myinfo.xp));
	obj->Set(String::NewFromUtf8(isolate, "maxxp"), Integer::New(isolate, myinfo.maxxp));
	obj->Set(String::NewFromUtf8(isolate, "skill_id"), Integer::New(isolate, myinfo.skill_id));
	obj->Set(String::NewFromUtf8(isolate, "type"), Integer::New(isolate, myinfo.type));
	obj->Set(String::NewFromUtf8(isolate, "pos"), Integer::New(isolate, myinfo.pos));
	obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, myinfo.index));
	obj->Set(String::NewFromUtf8(isolate, "slotsize"), Integer::New(isolate, myinfo.slotsize));
	info.GetReturnValue().Set(obj);
}

void GetSkillsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

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
;		obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "lv"), Integer::New(isolate, myinfo.lv));
		obj->Set(String::NewFromUtf8(isolate, "maxlv"), Integer::New(isolate, myinfo.maxlv));
		obj->Set(String::NewFromUtf8(isolate, "xp"), Integer::New(isolate, myinfo.xp));
		obj->Set(String::NewFromUtf8(isolate, "maxxp"), Integer::New(isolate, myinfo.maxxp));
		obj->Set(String::NewFromUtf8(isolate, "skill_id"), Integer::New(isolate, myinfo.skill_id));
		obj->Set(String::NewFromUtf8(isolate, "type"), Integer::New(isolate, myinfo.type));
		obj->Set(String::NewFromUtf8(isolate, "pos"), Integer::New(isolate, myinfo.pos));
		obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, myinfo.index));
		obj->Set(String::NewFromUtf8(isolate, "slotsize"), Integer::New(isolate, myinfo.slotsize));
		arr->Set(i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void GetCraftInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || info[0]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[0] must be skillid.");
		return;
	}

	if (info.Length() < 2 || info[1]->IsUndefined()) {
		Nan::ThrowTypeError("Arg[2] must be subskill_index.");
		return;
	}

	int skillid = (int)info[0]->IntegerValue();
	int subskill_index = (int)info[1]->IntegerValue();

	CGA::cga_craft_info_t myinfo;
	if (!g_CGAInterface->GetCraftInfo(skillid, subskill_index, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "info"), Nan::New(myinfo.info).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "itemid"), Integer::New(isolate, myinfo.itemid));
	obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
	obj->Set(String::NewFromUtf8(isolate, "cost"), Integer::New(isolate, myinfo.cost));
	obj->Set(String::NewFromUtf8(isolate, "id"), Integer::New(isolate, myinfo.id));
	obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, myinfo.index));

	Local<Array> mat_arr = Array::New(isolate);
	for (size_t i = 0; i < 5; ++i)
	{
		Local<Object> mat_obj = Object::New(isolate);
		const CGA::cga_craft_material_t &mymat = myinfo.materials[i];
		if (!mymat.name.empty())
		{
			mat_obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(mymat.name).ToLocalChecked());
			mat_obj->Set(String::NewFromUtf8(isolate, "itemid"), Integer::New(isolate, mymat.itemid));
			mat_obj->Set(String::NewFromUtf8(isolate, "count"), Integer::New(isolate, mymat.count));
			mat_arr->Set(i, mat_obj);
		}
	}
	obj->Set(String::NewFromUtf8(isolate, "materials"), mat_arr);

	info.GetReturnValue().Set(obj);
}

void GetCraftsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || info[0]->IsUndefined())
	{
		Nan::ThrowTypeError("Arg[0] must be skillid.");
		return;
	}

	int skillid = (int)info[0]->IntegerValue();

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
		obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "info"), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "itemid"), Integer::New(isolate, myinfo.itemid));
		obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
		obj->Set(String::NewFromUtf8(isolate, "cost"), Integer::New(isolate, myinfo.cost));
		obj->Set(String::NewFromUtf8(isolate, "id"), Integer::New(isolate, myinfo.id));
		obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, myinfo.index));

		Local<Array> mat_arr = Array::New(isolate);
		for (size_t j = 0; j < 5; ++j)
		{
			Local<Object> mat_obj = Object::New(isolate);
			const CGA::cga_craft_material_t &mymat = myinfo.materials[j];
			if (!mymat.name.empty())
			{
				mat_obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(mymat.name).ToLocalChecked());
				mat_obj->Set(String::NewFromUtf8(isolate, "itemid"), Integer::New(isolate, mymat.itemid));
				mat_obj->Set(String::NewFromUtf8(isolate, "count"), Integer::New(isolate, mymat.count));
				mat_arr->Set(j, mat_obj);
			}
		}
		obj->Set(String::NewFromUtf8(isolate, "materials"), mat_arr);

		arr->Set(i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void GetSubSkillInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be skillid.");
		return;
	}
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[0] must be stage.");
		return;
	}

	int skillid = (int)info[0]->IntegerValue();
	int stage = (int)info[1]->IntegerValue();

	CGA::cga_subskill_info_t myinfo;
	if (!g_CGAInterface->GetSubSkillInfo(skillid, stage, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "info"), Nan::New(myinfo.info).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
	obj->Set(String::NewFromUtf8(isolate, "cost"), Integer::New(isolate, myinfo.cost));
	obj->Set(String::NewFromUtf8(isolate, "flags"), Integer::New(isolate, myinfo.flags));
	obj->Set(String::NewFromUtf8(isolate, "available"), Boolean::New(isolate, myinfo.available));
	info.GetReturnValue().Set(obj);
}

void GetSubSkillsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be skillid.");
		return;
	}

	int skillid = (int)info[0]->IntegerValue();

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
		obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "info"), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
		obj->Set(String::NewFromUtf8(isolate, "cost"), Integer::New(isolate, myinfo.cost));
		obj->Set(String::NewFromUtf8(isolate, "flags"), Integer::New(isolate, myinfo.flags));
		obj->Set(String::NewFromUtf8(isolate, "available"), Boolean::New(isolate, myinfo.available));
		arr->Set(i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void IsItemValid(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be itempos.");
		return;
	}

	int itempos = (int)info[0]->IntegerValue();
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be itempos.");
		return;
	}

	int itempos = (int)info[0]->IntegerValue();

	CGA::cga_item_info_t myinfo;
	if (!g_CGAInterface->GetItemInfo(itempos, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "info"), Nan::New(myinfo.info).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "info2"), Nan::New(myinfo.info2).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "attr"), Nan::New(myinfo.attr).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "attr2"), Nan::New(myinfo.attr2).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "count"), Integer::New(isolate, myinfo.count));
	obj->Set(String::NewFromUtf8(isolate, "itemid"), Integer::New(isolate, myinfo.itemid));
	obj->Set(String::NewFromUtf8(isolate, "pos"), Integer::New(isolate, myinfo.pos));
	obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
	obj->Set(String::NewFromUtf8(isolate, "type"), Integer::New(isolate, myinfo.type));
	obj->Set(String::NewFromUtf8(isolate, "assessed"), Boolean::New(isolate, myinfo.assessed));
	info.GetReturnValue().Set(obj);
}

void GetItemsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

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
		obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "info"), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "info2"), Nan::New(myinfo.info2).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "attr"), Nan::New(myinfo.attr).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "attr2"), Nan::New(myinfo.attr2).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "count"), Integer::New(isolate, myinfo.count));
		obj->Set(String::NewFromUtf8(isolate, "itemid"), Integer::New(isolate, myinfo.itemid));
		obj->Set(String::NewFromUtf8(isolate, "pos"), Integer::New(isolate, myinfo.pos));
		obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
		obj->Set(String::NewFromUtf8(isolate, "type"), Integer::New(isolate, myinfo.type));
		obj->Set(String::NewFromUtf8(isolate, "assessed"), Boolean::New(isolate, myinfo.assessed));
		arr->Set(i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void GetBankItemsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

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
		obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "info"), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "info2"), Nan::New(myinfo.info2).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "attr"), Nan::New(myinfo.attr).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "attr2"), Nan::New(myinfo.attr2).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "count"), Integer::New(isolate, myinfo.count));
		obj->Set(String::NewFromUtf8(isolate, "itemid"), Integer::New(isolate, myinfo.itemid));
		obj->Set(String::NewFromUtf8(isolate, "pos"), Integer::New(isolate, myinfo.pos));
		obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
		obj->Set(String::NewFromUtf8(isolate, "type"), Integer::New(isolate, myinfo.type));
		obj->Set(String::NewFromUtf8(isolate, "assessed"), Boolean::New(isolate, myinfo.assessed));
		arr->Set(i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void IsPetValid(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be petid.");
		return;
	}

	int petid = (int)info[0]->IntegerValue();
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be petid.");
		return;
	}

	int petid = (int)info[0]->IntegerValue();
	CGA::cga_pet_info_t myinfo;
	if (!g_CGAInterface->GetPetInfo(petid, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "realname"), Nan::New(myinfo.realname).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
	obj->Set(String::NewFromUtf8(isolate, "race"), Integer::New(isolate, myinfo.race));
	obj->Set(String::NewFromUtf8(isolate, "loyality"), Integer::New(isolate, myinfo.loyality));
	obj->Set(String::NewFromUtf8(isolate, "health"), Integer::New(isolate, myinfo.health));
	obj->Set(String::NewFromUtf8(isolate, "hp"), Integer::New(isolate, myinfo.hp));
	obj->Set(String::NewFromUtf8(isolate, "maxhp"), Integer::New(isolate, myinfo.maxhp));
	obj->Set(String::NewFromUtf8(isolate, "mp"), Integer::New(isolate, myinfo.mp));
	obj->Set(String::NewFromUtf8(isolate, "maxmp"), Integer::New(isolate, myinfo.maxmp));
	obj->Set(String::NewFromUtf8(isolate, "xp"), Integer::New(isolate, myinfo.xp));
	obj->Set(String::NewFromUtf8(isolate, "maxxp"), Integer::New(isolate, myinfo.maxxp));
	obj->Set(String::NewFromUtf8(isolate, "flags"), Integer::New(isolate, myinfo.flags));
	obj->Set(String::NewFromUtf8(isolate, "battle_flags"), Integer::New(isolate, myinfo.battle_flags));
	obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, myinfo.index));

	Local<Object> objd = Object::New(isolate);
	objd->Set(String::NewFromUtf8(isolate, "points_remain"), Integer::New(isolate, myinfo.detail.points_remain));
	objd->Set(String::NewFromUtf8(isolate, "points_endurance"), Integer::New(isolate, myinfo.detail.points_endurance));
	objd->Set(String::NewFromUtf8(isolate, "points_strength"), Integer::New(isolate, myinfo.detail.points_strength));
	objd->Set(String::NewFromUtf8(isolate, "points_defense"), Integer::New(isolate, myinfo.detail.points_defense));
	objd->Set(String::NewFromUtf8(isolate, "points_agility"), Integer::New(isolate, myinfo.detail.points_agility));
	objd->Set(String::NewFromUtf8(isolate, "points_magical"), Integer::New(isolate, myinfo.detail.points_magical));
	objd->Set(String::NewFromUtf8(isolate, "value_attack"), Integer::New(isolate, myinfo.detail.value_attack));
	objd->Set(String::NewFromUtf8(isolate, "value_defensive"), Integer::New(isolate, myinfo.detail.value_defensive));
	objd->Set(String::NewFromUtf8(isolate, "value_agility"), Integer::New(isolate, myinfo.detail.value_agility));
	objd->Set(String::NewFromUtf8(isolate, "value_spirit"), Integer::New(isolate, myinfo.detail.value_spirit));
	objd->Set(String::NewFromUtf8(isolate, "value_recovery"), Integer::New(isolate, myinfo.detail.value_recovery));
	objd->Set(String::NewFromUtf8(isolate, "resist_poison"), Integer::New(isolate, myinfo.detail.resist_poison));
	objd->Set(String::NewFromUtf8(isolate, "resist_sleep"), Integer::New(isolate, myinfo.detail.resist_sleep));
	objd->Set(String::NewFromUtf8(isolate, "resist_medusa"), Integer::New(isolate, myinfo.detail.resist_medusa));
	objd->Set(String::NewFromUtf8(isolate, "resist_drunk"), Integer::New(isolate, myinfo.detail.resist_drunk));
	objd->Set(String::NewFromUtf8(isolate, "resist_chaos"), Integer::New(isolate, myinfo.detail.resist_chaos));
	objd->Set(String::NewFromUtf8(isolate, "resist_forget"), Integer::New(isolate, myinfo.detail.resist_forget));
	objd->Set(String::NewFromUtf8(isolate, "fix_critical"), Integer::New(isolate, myinfo.detail.fix_critical));
	objd->Set(String::NewFromUtf8(isolate, "fix_strikeback"), Integer::New(isolate, myinfo.detail.fix_strikeback));
	objd->Set(String::NewFromUtf8(isolate, "fix_accurancy"), Integer::New(isolate, myinfo.detail.fix_accurancy));
	objd->Set(String::NewFromUtf8(isolate, "fix_dodge"), Integer::New(isolate, myinfo.detail.fix_dodge));
	objd->Set(String::NewFromUtf8(isolate, "element_earth"), Integer::New(isolate, myinfo.detail.element_earth));
	objd->Set(String::NewFromUtf8(isolate, "element_water"), Integer::New(isolate, myinfo.detail.element_water));
	objd->Set(String::NewFromUtf8(isolate, "element_fire"), Integer::New(isolate, myinfo.detail.element_fire));
	objd->Set(String::NewFromUtf8(isolate, "element_wind"), Integer::New(isolate, myinfo.detail.element_wind));
	obj->Set(String::NewFromUtf8(isolate, "detail"), objd);

	info.GetReturnValue().Set(obj);
}

void GetPetsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

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
		obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "realname"), Nan::New(myinfo.realname).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "level"), Integer::New(isolate, myinfo.level));
		obj->Set(String::NewFromUtf8(isolate, "race"), Integer::New(isolate, myinfo.race));
		obj->Set(String::NewFromUtf8(isolate, "loyality"), Integer::New(isolate, myinfo.loyality));
		obj->Set(String::NewFromUtf8(isolate, "health"), Integer::New(isolate, myinfo.health));
		obj->Set(String::NewFromUtf8(isolate, "hp"), Integer::New(isolate, myinfo.hp));
		obj->Set(String::NewFromUtf8(isolate, "maxhp"), Integer::New(isolate, myinfo.maxhp));
		obj->Set(String::NewFromUtf8(isolate, "mp"), Integer::New(isolate, myinfo.mp));
		obj->Set(String::NewFromUtf8(isolate, "maxmp"), Integer::New(isolate, myinfo.maxmp));
		obj->Set(String::NewFromUtf8(isolate, "xp"), Integer::New(isolate, myinfo.xp));
		obj->Set(String::NewFromUtf8(isolate, "maxxp"), Integer::New(isolate, myinfo.maxxp));
		obj->Set(String::NewFromUtf8(isolate, "flags"), Integer::New(isolate, myinfo.flags));
		obj->Set(String::NewFromUtf8(isolate, "battle_flags"), Integer::New(isolate, myinfo.battle_flags));
		obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, myinfo.index));

		Local<Object> objd = Object::New(isolate);
		objd->Set(String::NewFromUtf8(isolate, "points_remain"), Integer::New(isolate, myinfo.detail.points_remain));
		objd->Set(String::NewFromUtf8(isolate, "points_endurance"), Integer::New(isolate, myinfo.detail.points_endurance));
		objd->Set(String::NewFromUtf8(isolate, "points_strength"), Integer::New(isolate, myinfo.detail.points_strength));
		objd->Set(String::NewFromUtf8(isolate, "points_defense"), Integer::New(isolate, myinfo.detail.points_defense));
		objd->Set(String::NewFromUtf8(isolate, "points_agility"), Integer::New(isolate, myinfo.detail.points_agility));
		objd->Set(String::NewFromUtf8(isolate, "points_magical"), Integer::New(isolate, myinfo.detail.points_magical));
		objd->Set(String::NewFromUtf8(isolate, "value_attack"), Integer::New(isolate, myinfo.detail.value_attack));
		objd->Set(String::NewFromUtf8(isolate, "value_defensive"), Integer::New(isolate, myinfo.detail.value_defensive));
		objd->Set(String::NewFromUtf8(isolate, "value_agility"), Integer::New(isolate, myinfo.detail.value_agility));
		objd->Set(String::NewFromUtf8(isolate, "value_spirit"), Integer::New(isolate, myinfo.detail.value_spirit));
		objd->Set(String::NewFromUtf8(isolate, "value_recovery"), Integer::New(isolate, myinfo.detail.value_recovery));
		objd->Set(String::NewFromUtf8(isolate, "resist_poison"), Integer::New(isolate, myinfo.detail.resist_poison));
		objd->Set(String::NewFromUtf8(isolate, "resist_sleep"), Integer::New(isolate, myinfo.detail.resist_sleep));
		objd->Set(String::NewFromUtf8(isolate, "resist_medusa"), Integer::New(isolate, myinfo.detail.resist_medusa));
		objd->Set(String::NewFromUtf8(isolate, "resist_drunk"), Integer::New(isolate, myinfo.detail.resist_drunk));
		objd->Set(String::NewFromUtf8(isolate, "resist_chaos"), Integer::New(isolate, myinfo.detail.resist_chaos));
		objd->Set(String::NewFromUtf8(isolate, "resist_forget"), Integer::New(isolate, myinfo.detail.resist_forget));
		objd->Set(String::NewFromUtf8(isolate, "fix_critical"), Integer::New(isolate, myinfo.detail.fix_critical));
		objd->Set(String::NewFromUtf8(isolate, "fix_strikeback"), Integer::New(isolate, myinfo.detail.fix_strikeback));
		objd->Set(String::NewFromUtf8(isolate, "fix_accurancy"), Integer::New(isolate, myinfo.detail.fix_accurancy));
		objd->Set(String::NewFromUtf8(isolate, "fix_dodge"), Integer::New(isolate, myinfo.detail.fix_dodge));
		objd->Set(String::NewFromUtf8(isolate, "element_earth"), Integer::New(isolate, myinfo.detail.element_earth));
		objd->Set(String::NewFromUtf8(isolate, "element_water"), Integer::New(isolate, myinfo.detail.element_water));
		objd->Set(String::NewFromUtf8(isolate, "element_fire"), Integer::New(isolate, myinfo.detail.element_fire));
		objd->Set(String::NewFromUtf8(isolate, "element_wind"), Integer::New(isolate, myinfo.detail.element_wind));
		obj->Set(String::NewFromUtf8(isolate, "detail"), objd);

		arr->Set(i, obj);
	}
	info.GetReturnValue().Set(arr);
}

void IsPetSkillValid(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be petid.");
		return;
	}
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be skill index.");
		return;
	}

	int petid = (int)info[0]->IntegerValue();
	int skillid = (int)info[1]->IntegerValue();
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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be petid.");
		return;
	}
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be skillid.");
		return;
	}

	int petid = (int)info[0]->IntegerValue();
	int skillid = (int)info[1]->IntegerValue();
	CGA::cga_pet_skill_info_t myinfo;
	if (!g_CGAInterface->GetPetSkillInfo(petid, skillid, myinfo))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "info"), Nan::New(myinfo.info).ToLocalChecked());
	obj->Set(String::NewFromUtf8(isolate, "cost"), Integer::New(isolate, myinfo.cost));
	obj->Set(String::NewFromUtf8(isolate, "flags"), Integer::New(isolate, myinfo.flags));
	obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, myinfo.index));
	info.GetReturnValue().Set(obj);
}

void GetPetSkillsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be petid.");
		return;
	}

	int petid = (int)info[0]->IntegerValue();
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
		obj->Set(String::NewFromUtf8(isolate, "name"), Nan::New(myinfo.name).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "info"), Nan::New(myinfo.info).ToLocalChecked());
		obj->Set(String::NewFromUtf8(isolate, "cost"), Integer::New(isolate, myinfo.cost));
		obj->Set(String::NewFromUtf8(isolate, "flags"), Integer::New(isolate, myinfo.flags));
		obj->Set(String::NewFromUtf8(isolate, "index"), Integer::New(isolate, myinfo.index));
		arr->Set(i, obj);
	}
	info.GetReturnValue().Set(arr);
}