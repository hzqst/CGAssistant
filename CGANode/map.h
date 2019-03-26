#pragma once 

void GetMapXY(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapXYFloat(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapName(const Nan::FunctionCallbackInfo<v8::Value>& info);
void WalkTo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void TurnTo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWalkTo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapUnits(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWaitMovement(const Nan::FunctionCallbackInfo<v8::Value>& info);
void IsMapCellPassable(const Nan::FunctionCallbackInfo<v8::Value>& info);
void ForceMove(const Nan::FunctionCallbackInfo<v8::Value>& info);
void ForceMoveTo(const Nan::FunctionCallbackInfo<v8::Value>& info);