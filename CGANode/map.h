#pragma once 

void AsyncWaitDownloadMap(const Nan::FunctionCallbackInfo<v8::Value>& info);
void FixMapWarpStuck(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapXY(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapXYFloat(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapIndex(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMoveSpeed(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapName(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapUnits(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapCollisionTableRaw(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapCollisionTable(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapObjectTable(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMapTileTable(const Nan::FunctionCallbackInfo<v8::Value>& info);
void GetMoveHistory(const Nan::FunctionCallbackInfo<v8::Value>& info);
void WalkTo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void TurnTo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWalkTo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void AsyncWaitMovement(const Nan::FunctionCallbackInfo<v8::Value>& info);
void IsMapCellPassable(const Nan::FunctionCallbackInfo<v8::Value>& info);
void ForceMove(const Nan::FunctionCallbackInfo<v8::Value>& info);
void ForceMoveTo(const Nan::FunctionCallbackInfo<v8::Value>& info);
void RequestDownloadMap(const Nan::FunctionCallbackInfo<v8::Value>& info);