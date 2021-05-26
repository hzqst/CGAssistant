#pragma once 

void UpgradePlayer(const Nan::FunctionCallbackInfo<v8::Value>& info);
void UpgradePet(const Nan::FunctionCallbackInfo<v8::Value>& info);
void ChangePetState(const Nan::FunctionCallbackInfo<v8::Value>& info);
void DropPet(const Nan::FunctionCallbackInfo<v8::Value>& info);
void DropItem(const Nan::FunctionCallbackInfo<v8::Value>& info);
void UseItem(const Nan::FunctionCallbackInfo<v8::Value>& info);
void MoveItem(const Nan::FunctionCallbackInfo<v8::Value>& info);
void MovePet(const Nan::FunctionCallbackInfo<v8::Value>& info);
void MoveGold(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWaitPlayerMenu(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWaitUnitMenu(const Nan::FunctionCallbackInfo<v8::Value>& info);
void PlayerMenuSelect(const Nan::FunctionCallbackInfo<v8::Value>& info);
void UnitMenuSelect(const Nan::FunctionCallbackInfo<v8::Value>& info);
void DoRequest(const Nan::FunctionCallbackInfo<v8::Value>& info);
void EnableFlags(const Nan::FunctionCallbackInfo<v8::Value>& info);
void TradeAddStuffs(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWaitTradeStuffs(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWaitTradeDialog(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWaitTradeState(const Nan::FunctionCallbackInfo<v8::Value>& info);