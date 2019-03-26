#pragma once 

void DropItem(const Nan::FunctionCallbackInfo<v8::Value>& info);
void UseItem(const Nan::FunctionCallbackInfo<v8::Value>& info);
void MoveItem(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWaitPlayerMenu(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWaitUnitMenu(const Nan::FunctionCallbackInfo<v8::Value>& info);
void PlayerMenuSelect(const Nan::FunctionCallbackInfo<v8::Value>& info);
void UnitMenuSelect(const Nan::FunctionCallbackInfo<v8::Value>& info);