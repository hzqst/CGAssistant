#include <nan.h>
#include "../CGALib/gameinterface.h"

using namespace v8;

CGA::CGAInterface *g_CGAInterface = NULL;

#include "connect.h"
#include "info.h"
#include "item.h"
#include "map.h"
#include "npc.h"
#include "work.h"
#include "chat.h"
#include "battle.h"

void FreeUVHandleCallBack(uv_handle_t *handle)
{
	delete handle;
}

void LogBack(const Nan::FunctionCallbackInfo<v8::Value>& info) 
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();
	
	if (!g_CGAInterface->LogBack())
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void LogOut(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (!g_CGAInterface->LogOut())
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void SayWords(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsString()) {
		Nan::ThrowTypeError("Arg[0] must be string.");
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

	v8::String::Utf8Value str(isolate, info[0]->ToString(context).ToLocalChecked());

	int color = info[1]->Int32Value(context).ToChecked();

	int range = info[2]->Int32Value(context).ToChecked();

	int size = info[3]->Int32Value(context).ToChecked();

	std::string sstr(*str);

	if (!g_CGAInterface->SayWords(sstr, color, range, size))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(true);
}

void ChangeNickName(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsString()) {
		Nan::ThrowTypeError("Arg[0] must be string.");
		return;
	}

	v8::String::Utf8Value str(isolate, info[0]->ToString(context).ToLocalChecked());

	std::string sstr(*str);

	bool bResult = false;

	if (!g_CGAInterface->ChangeNickName(sstr, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}

void ChangeTitleName(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}

	int titleId = info[0]->Int32Value(context).ToChecked();

	bool bResult = false;

	if (!g_CGAInterface->ChangeTitleName(titleId, bResult))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(bResult);
}

void ChangePersDesc(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsObject()) {
		Nan::ThrowTypeError("Arg[0] must be object.");
		return;
	}

	CGA::cga_pers_desc_t desc;

	Local<Object> obj = Local<Object>::Cast(info[0]);

	{
		auto v_sellIcon = obj->Get(context, String::NewFromUtf8(isolate, "sell_icon").ToLocalChecked());
		if (!v_sellIcon.IsEmpty() && v_sellIcon.ToLocalChecked()->IsInt32())
		{
			desc.changeBits |= 1;
			desc.sellIcon = v_sellIcon.ToLocalChecked()->Int32Value(context).ToChecked();
		}
	}

	{
		auto v_sellString = obj->Get(context, String::NewFromUtf8(isolate, "sell_string").ToLocalChecked());
		if (!v_sellString.IsEmpty() && v_sellString.ToLocalChecked()->IsString())
		{
			desc.changeBits |= 2;

			v8::String::Utf8Value str(isolate, v_sellString.ToLocalChecked()->ToString(context).ToLocalChecked());
			std::string sellString(*str);
			desc.sellString = sellString;
		}
	}

	{
		auto v_buyIcon = obj->Get(context, String::NewFromUtf8(isolate, "buy_icon").ToLocalChecked());
		if (!v_buyIcon.IsEmpty() && v_buyIcon.ToLocalChecked()->IsInt32())
		{
			desc.changeBits |= 4;
			desc.buyIcon = v_buyIcon.ToLocalChecked()->Int32Value(context).ToChecked();
		}
	}

	{
		auto v_buyString = obj->Get(context, String::NewFromUtf8(isolate, "buy_string").ToLocalChecked());
		if (!v_buyString.IsEmpty() && v_buyString.ToLocalChecked()->IsString())
		{
			desc.changeBits |= 8;

			v8::String::Utf8Value str(isolate, v_buyString.ToLocalChecked()->ToString(context).ToLocalChecked());
			std::string buyString(*str);
			desc.buyString = buyString;
		}
	}

	{
		auto v_wantIcon = obj->Get(context, String::NewFromUtf8(isolate, "want_icon").ToLocalChecked());
		if (!v_wantIcon.IsEmpty() && v_wantIcon.ToLocalChecked()->IsInt32())
		{
			desc.changeBits |= 0x10;
			desc.wantIcon = v_wantIcon.ToLocalChecked()->Int32Value(context).ToChecked();
		}
	}

	{
		auto v_wantString = obj->Get(context, String::NewFromUtf8(isolate, "want_string").ToLocalChecked());
		if (!v_wantString.IsEmpty() && v_wantString.ToLocalChecked()->IsString())
		{
			desc.changeBits |= 0x20;

			v8::String::Utf8Value str(isolate, v_wantString.ToLocalChecked()->ToString(context).ToLocalChecked());
			std::string wantString(*str);
			desc.wantString = wantString;
		}
	}

	{
		auto v_descString = obj->Get(context, String::NewFromUtf8(isolate, "desc_string").ToLocalChecked());
		if (!v_descString.IsEmpty() && v_descString.ToLocalChecked()->IsString())
		{
			desc.changeBits |= 0x40;

			v8::String::Utf8Value str(isolate, v_descString.ToLocalChecked()->ToString(context).ToLocalChecked());
			std::string descString(*str);
			desc.descString = descString;
		}
	}

	if (!g_CGAInterface->ChangePersDesc(desc))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void Init(v8::Local<v8::Object> exports) {

	auto isolate = exports->GetIsolate();
	auto context = isolate->GetCurrentContext();

	g_CGAInterface = CGA::CreateInterface();

	exports->Set(context, Nan::New("Connect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(Connect)->GetFunction(context).ToLocalChecked() );
	exports->Set(context, Nan::New("AsyncConnect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncConnect)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("IsInGame").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(IsInGame)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetWorldStatus").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetWorldStatus)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetGameStatus").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetGameStatus)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetBGMIndex").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetBGMIndex)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetSysTime").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetSysTime)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetPlayerInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetPlayerInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("SetPlayerFlagEnabled").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(SetPlayerFlagEnabled)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("IsPlayerFlagEnabled").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(IsPlayerFlagEnabled)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("IsSkillValid").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(IsSkillValid)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetSkillInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetSkillInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetSkillsInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetSkillsInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetSubSkillInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetSubSkillInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetSubSkillsInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetSubSkillsInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("IsItemValid").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(IsItemValid)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetItemInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetItemInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetItemsInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetItemsInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetBankItemsInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetBankItemsInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetBankGold").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetBankGold)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("IsPetValid").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(IsPetValid)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetPetInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetPetInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetPetsInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetPetsInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetBankPetsInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetBankPetsInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("IsPetSkillValid").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(IsPetSkillValid)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetPetSkillInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetPetSkillInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetPetSkillsInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetPetSkillsInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMapXY").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMapXY)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMapXYFloat").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMapXYFloat)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMapIndex").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMapIndex)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMoveSpeed").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMoveSpeed)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMapName").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMapName)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMapUnits").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMapUnits)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMapCollisionTableRaw").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMapCollisionTableRaw)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMapCollisionTable").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMapCollisionTable)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMapObjectTable").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMapObjectTable)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetMapTileTable").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetMapTileTable)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitDownloadMap").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitDownloadMap)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("WalkTo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(WalkTo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("TurnTo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(TurnTo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("ForceMove").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(ForceMove)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("ForceMoveTo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(ForceMoveTo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("IsMapCellPassable").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(IsMapCellPassable)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWalkTo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWalkTo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitMovement").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitMovement)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("LogBack").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(LogBack)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("LogOut").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(LogOut)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("DropPet").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(DropPet)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("ChangePetState").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(ChangePetState)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("DropItem").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(DropItem)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("UseItem").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(UseItem)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("MoveItem").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(MoveItem)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("MoveGold").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(MoveGold)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("MovePet").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(MovePet)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitPlayerMenu").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitPlayerMenu)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitUnitMenu").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitUnitMenu)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("PlayerMenuSelect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(PlayerMenuSelect)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("UnitMenuSelect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(UnitMenuSelect)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("UpgradePlayer").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(UpgradePlayer)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("UpgradePet").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(UpgradePet)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitNPCDialog").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitNPCDialog)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("ClickNPCDialog").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(ClickNPCDialog)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("SellNPCStore").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(SellNPCStore)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BuyNPCStore").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BuyNPCStore)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("SayWords").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(SayWords)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("ChangeNickName").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(ChangeNickName)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("ChangeTitleName").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(ChangeTitleName)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("ChangePersDesc").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(ChangePersDesc)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitWorkingResult").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitWorkingResult)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("SetImmediateDoneWork").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(SetImmediateDoneWork)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("StartWork").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(StartWork)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("CraftItem").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(CraftItem)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AssessItem").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AssessItem)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetCraftInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetCraftInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetCraftsInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetCraftsInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetCraftStatus").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetCraftStatus)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetImmediateDoneWorkState").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetImmediateDoneWorkState)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitChatMsg").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitChatMsg)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("DoRequest").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(DoRequest)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("EnableFlags").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(EnableFlags)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("TradeAddStuffs").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(TradeAddStuffs)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitTradeStuffs").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitTradeStuffs)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitTradeDialog").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitTradeDialog)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitTradeState").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitTradeState)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetTeamPlayerInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetTeamPlayerInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("FixMapWarpStuck").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(FixMapWarpStuck)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetTeamPlayerInfo").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetTeamPlayerInfo)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("IsUIDialogPresent").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(IsUIDialogPresent)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("RequestDownloadMap").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(RequestDownloadMap)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitBattleAction").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitBattleAction)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetBattleUnits").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetBattleUnits)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("GetBattleContext").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetBattleContext)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattleNormalAttack").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattleNormalAttack)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattleSkillAttack").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattleSkillAttack)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattleRebirth").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattleRebirth)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattleGuard").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattleGuard)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattleEscape").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattleEscape)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattleExchangePosition").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattleExchangePosition)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattleChangePet").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattleChangePet)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattleUseItem").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattleUseItem)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattlePetSkillAttack").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattlePetSkillAttack)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("BattleDoNothing").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(BattleDoNothing)->GetFunction(context).ToLocalChecked());
	exports->Set(context, Nan::New("AsyncWaitConnectionState").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(AsyncWaitConnectionState)->GetFunction(context).ToLocalChecked());
}

NODE_MODULE(node_cga, Init)
