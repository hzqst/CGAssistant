#pragma once 

void AsyncWaitBattleAction(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetBattleUnits(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetBattleContext(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattleNormalAttack(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattleSkillAttack(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattleRebirth(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattleGuard(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattleEscape(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattleExchangePosition(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattleDoNothing(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattleChangePet(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattleUseItem(const Nan::FunctionCallbackInfo<v8::Value>& info);
void BattlePetSkillAttack(const Nan::FunctionCallbackInfo<v8::Value>& info);