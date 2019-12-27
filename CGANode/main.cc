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
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	
	if (!g_CGAInterface->LogBack())
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void LogOut(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (!g_CGAInterface->LogOut())
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}
}

void SayWords(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);

	if (info.Length() < 1 || !info[0]->IsString()) {
		Nan::ThrowTypeError("Arg[0] must be string.");
		return;
	}
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Arg[1] must be color.");
		return;
	}
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Arg[2] must be range.");
		return;
	}
	if (info.Length() < 4) {
		Nan::ThrowTypeError("Arg[3] must be size.");
		return;
	}

	v8::String::Utf8Value str(info[0]->ToString());
	int color = (int)info[1]->IntegerValue();
	int range = (int)info[2]->IntegerValue();
	int size = (int)info[3]->IntegerValue();

	std::string sstr(*str);
	if (!g_CGAInterface->SayWords(sstr, color, range, size))
	{
		Nan::ThrowError("RPC Invocation failed.");
		return;
	}

	info.GetReturnValue().Set(true);
}

void Init(v8::Local<v8::Object> exports) {
	g_CGAInterface = CGA::CreateInterface();

	exports->Set(Nan::New("Connect").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(Connect)->GetFunction());
	exports->Set(Nan::New("AsyncConnect").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncConnect)->GetFunction());
	exports->Set(Nan::New("IsInGame").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(IsInGame)->GetFunction());
	exports->Set(Nan::New("GetWorldStatus").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetWorldStatus)->GetFunction());
	exports->Set(Nan::New("GetGameStatus").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetGameStatus)->GetFunction());
	exports->Set(Nan::New("GetPlayerInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetPlayerInfo)->GetFunction());
	exports->Set(Nan::New("SetPlayerFlagEnabled").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(SetPlayerFlagEnabled)->GetFunction());
	exports->Set(Nan::New("IsPlayerFlagEnabled").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(IsPlayerFlagEnabled)->GetFunction());
	exports->Set(Nan::New("IsSkillValid").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(IsSkillValid)->GetFunction());
	exports->Set(Nan::New("GetSkillInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetSkillInfo)->GetFunction());
	exports->Set(Nan::New("GetSkillsInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetSkillsInfo)->GetFunction());
	exports->Set(Nan::New("GetSubSkillInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetSubSkillInfo)->GetFunction());
	exports->Set(Nan::New("GetSubSkillsInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetSubSkillsInfo)->GetFunction());
	exports->Set(Nan::New("IsItemValid").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(IsItemValid)->GetFunction());
	exports->Set(Nan::New("GetItemInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetItemInfo)->GetFunction());
	exports->Set(Nan::New("GetItemsInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetItemsInfo)->GetFunction());
	exports->Set(Nan::New("GetBankItemsInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetBankItemsInfo)->GetFunction());
	exports->Set(Nan::New("IsPetValid").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(IsPetValid)->GetFunction());
	exports->Set(Nan::New("GetPetInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetPetInfo)->GetFunction());
	exports->Set(Nan::New("GetPetsInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetPetsInfo)->GetFunction());
	exports->Set(Nan::New("IsPetSkillValid").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(IsPetSkillValid)->GetFunction());
	exports->Set(Nan::New("GetPetSkillInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetPetSkillInfo)->GetFunction());
	exports->Set(Nan::New("GetPetSkillsInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetPetSkillsInfo)->GetFunction());	
	exports->Set(Nan::New("GetMapXY").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMapXY)->GetFunction());
	exports->Set(Nan::New("GetMapXYFloat").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMapXYFloat)->GetFunction());
	exports->Set(Nan::New("GetMapIndex").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMapIndex)->GetFunction());
	exports->Set(Nan::New("GetMoveSpeed").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMoveSpeed)->GetFunction());
	exports->Set(Nan::New("GetMapName").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMapName)->GetFunction());
	exports->Set(Nan::New("GetMapUnits").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMapUnits)->GetFunction());
	exports->Set(Nan::New("GetMapCollisionTableRaw").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMapCollisionTableRaw)->GetFunction());
	exports->Set(Nan::New("GetMapCollisionTable").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMapCollisionTable)->GetFunction());
	exports->Set(Nan::New("GetMapObjectTable").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMapObjectTable)->GetFunction());
	exports->Set(Nan::New("GetMapTileTable").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMapTileTable)->GetFunction());
	exports->Set(Nan::New("AsyncWaitDownloadMap").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitDownloadMap)->GetFunction());
	exports->Set(Nan::New("WalkTo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(WalkTo)->GetFunction());
	exports->Set(Nan::New("TurnTo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(TurnTo)->GetFunction());
	exports->Set(Nan::New("ForceMove").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(ForceMove)->GetFunction());
	exports->Set(Nan::New("ForceMoveTo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(ForceMoveTo)->GetFunction());
	exports->Set(Nan::New("IsMapCellPassable").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(IsMapCellPassable)->GetFunction());
	exports->Set(Nan::New("AsyncWalkTo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWalkTo)->GetFunction());
	exports->Set(Nan::New("AsyncWaitMovement").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitMovement)->GetFunction());
	exports->Set(Nan::New("LogBack").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(LogBack)->GetFunction());
	exports->Set(Nan::New("LogOut").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(LogOut)->GetFunction());
	exports->Set(Nan::New("DropPet").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(DropPet)->GetFunction());
	exports->Set(Nan::New("ChangePetState").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(ChangePetState)->GetFunction());
	exports->Set(Nan::New("DropItem").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(DropItem)->GetFunction());
	exports->Set(Nan::New("UseItem").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(UseItem)->GetFunction());
	exports->Set(Nan::New("MoveItem").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(MoveItem)->GetFunction());
	exports->Set(Nan::New("MovePet").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(MovePet)->GetFunction());
	exports->Set(Nan::New("MoveGold").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(MoveGold)->GetFunction());
	exports->Set(Nan::New("AsyncWaitPlayerMenu").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitPlayerMenu)->GetFunction());
	exports->Set(Nan::New("AsyncWaitUnitMenu").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitUnitMenu)->GetFunction());
	exports->Set(Nan::New("PlayerMenuSelect").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(PlayerMenuSelect)->GetFunction());
	exports->Set(Nan::New("UnitMenuSelect").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(UnitMenuSelect)->GetFunction());
	exports->Set(Nan::New("AsyncWaitNPCDialog").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitNPCDialog)->GetFunction());	
	exports->Set(Nan::New("ClickNPCDialog").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(ClickNPCDialog)->GetFunction());
	exports->Set(Nan::New("SellNPCStore").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(SellNPCStore)->GetFunction());
	exports->Set(Nan::New("BuyNPCStore").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BuyNPCStore)->GetFunction());
	exports->Set(Nan::New("SayWords").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(SayWords)->GetFunction());
	exports->Set(Nan::New("AsyncWaitWorkingResult").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitWorkingResult)->GetFunction());
	exports->Set(Nan::New("SetImmediateDoneWork").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(SetImmediateDoneWork)->GetFunction());
	exports->Set(Nan::New("StartWork").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(StartWork)->GetFunction());
	exports->Set(Nan::New("CraftItem").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(CraftItem)->GetFunction());
	exports->Set(Nan::New("AssessItem").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AssessItem)->GetFunction());
	exports->Set(Nan::New("GetCraftInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetCraftInfo)->GetFunction());
	exports->Set(Nan::New("GetCraftsInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetCraftsInfo)->GetFunction());
	exports->Set(Nan::New("GetCraftStatus").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetCraftStatus)->GetFunction());
	exports->Set(Nan::New("GetImmediateDoneWorkState").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetImmediateDoneWorkState)->GetFunction());
	exports->Set(Nan::New("AsyncWaitChatMsg").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitChatMsg)->GetFunction());
	exports->Set(Nan::New("DoRequest").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(DoRequest)->GetFunction());
	exports->Set(Nan::New("EnableFlags").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(EnableFlags)->GetFunction());
	exports->Set(Nan::New("TradeAddStuffs").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(TradeAddStuffs)->GetFunction());
	exports->Set(Nan::New("AsyncWaitTradeStuffs").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitTradeStuffs)->GetFunction());
	exports->Set(Nan::New("AsyncWaitTradeDialog").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitTradeDialog)->GetFunction());
	exports->Set(Nan::New("AsyncWaitTradeState").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitTradeState)->GetFunction());
	exports->Set(Nan::New("GetTeamPlayerInfo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetTeamPlayerInfo)->GetFunction());
	exports->Set(Nan::New("FixMapWarpStuck").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(FixMapWarpStuck)->GetFunction());
	exports->Set(Nan::New("GetMoveHistory").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetMoveHistory)->GetFunction());
	exports->Set(Nan::New("RequestDownloadMap").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(RequestDownloadMap)->GetFunction());
	exports->Set(Nan::New("AsyncWaitBattleAction").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(AsyncWaitBattleAction)->GetFunction());
	exports->Set(Nan::New("GetBattleUnits").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetBattleUnits)->GetFunction());
	exports->Set(Nan::New("GetBattleContext").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetBattleContext)->GetFunction());
	exports->Set(Nan::New("BattleNormalAttack").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattleNormalAttack)->GetFunction());
	exports->Set(Nan::New("BattleSkillAttack").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattleSkillAttack)->GetFunction());
	exports->Set(Nan::New("BattleRebirth").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattleRebirth)->GetFunction());
	exports->Set(Nan::New("BattleGuard").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattleGuard)->GetFunction());
	exports->Set(Nan::New("BattleEscape").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattleEscape)->GetFunction());
	exports->Set(Nan::New("BattleExchangePosition").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattleExchangePosition)->GetFunction());
	exports->Set(Nan::New("BattleChangePet").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattleChangePet)->GetFunction());
	exports->Set(Nan::New("BattleUseItem").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattleUseItem)->GetFunction());
	exports->Set(Nan::New("BattlePetSkillAttack").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattlePetSkillAttack)->GetFunction());
	exports->Set(Nan::New("BattleDoNothing").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(BattleDoNothing)->GetFunction());
}

NODE_MODULE(node_cga, Init)
