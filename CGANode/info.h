#pragma once 

void IsUIDialogPresent(const Nan::FunctionCallbackInfo<v8::Value>& info);
void IsInGame(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetWorldStatus(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetGameStatus(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetBGMIndex(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetSysTime(const Nan::FunctionCallbackInfo<v8::Value>& info);

void GetPlayerInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void SetPlayerFlagEnabled(const Nan::FunctionCallbackInfo<v8::Value>& info);
void IsPlayerFlagEnabled(const Nan::FunctionCallbackInfo<v8::Value>& info);

void IsSkillValid(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetSkillInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetSkillsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetSubSkillInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetSubSkillsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);

void IsItemValid(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetItemInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetItemsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetBankItemsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetBankGold(const Nan::FunctionCallbackInfo<v8::Value>& info);

void IsPetValid(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetPetInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetPetsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetBankPetsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void IsPetSkillValid(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetPetSkillInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetPetSkillsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);

void GetCraftInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetCraftsInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetTeamPlayerInfo(const Nan::FunctionCallbackInfo<v8::Value>& info);