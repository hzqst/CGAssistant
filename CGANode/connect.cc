#include <nan.h>
#include "../CGALib/gameinterface.h"

using namespace v8;

extern CGA::CGAInterface *g_CGAInterface;

void BattleActionNotify(int flags);
void TradeStateNotify(int state);
void TradeDialogNotify(CGA::cga_trade_dialog_t info);
void TradeStuffsNotify(CGA::cga_trade_stuff_info_t info);
void PlayerMenuNotify(CGA::cga_player_menu_items_t players);
void UnitMenuNotify(CGA::cga_unit_menu_items_t units);
void NPCDialogNotify(CGA::cga_npc_dialog_t dlg);
void WorkingResultNotify(CGA::cga_working_result_t results);
void ChatMsgNotify(CGA::cga_chat_msg_t msg);
void DownloadMapNotify(CGA::cga_download_map_t msg);
void ConnectionStateNotify(CGA::cga_conn_state_t msg);

class ConnectWorkerData
{
public:
	ConnectWorkerData(int port)
	{
		m_port = port;
		m_result = false;
		m_worker.data = this;
	}

	bool m_result;
	int m_port;
	Persistent<Function> m_callback;
	uv_work_t m_worker;
};

void ConnectWorker(uv_work_t* req)
{
	auto data = (ConnectWorkerData *)req->data;

	if (g_CGAInterface->IsConnected())
	{
		data->m_result = true;
		return;
	}

	data->m_result = g_CGAInterface->Connect(data->m_port);

	if (data->m_result)
	{
		g_CGAInterface->RegisterBattleActionNotify(std::bind(&BattleActionNotify, std::placeholders::_1));
		g_CGAInterface->RegisterTradeStateNotify(std::bind(&TradeStateNotify, std::placeholders::_1));
		g_CGAInterface->RegisterTradeDialogNotify(std::bind(&TradeDialogNotify, std::placeholders::_1));
		g_CGAInterface->RegisterTradeStuffsNotify(std::bind(&TradeStuffsNotify, std::placeholders::_1));
		g_CGAInterface->RegisterPlayerMenuNotify(std::bind(&PlayerMenuNotify, std::placeholders::_1));
		g_CGAInterface->RegisterUnitMenuNotify(std::bind(&UnitMenuNotify, std::placeholders::_1));
		g_CGAInterface->RegisterNPCDialogNotify(std::bind(&NPCDialogNotify, std::placeholders::_1));
		g_CGAInterface->RegisterWorkingResultNotify(std::bind(&WorkingResultNotify, std::placeholders::_1));
		g_CGAInterface->RegisterChatMsgNotify(std::bind(&ChatMsgNotify, std::placeholders::_1));
		g_CGAInterface->RegisterDownloadMapNotify(std::bind(&DownloadMapNotify, std::placeholders::_1));
		g_CGAInterface->RegisterConnectionStateNotify(std::bind(&ConnectionStateNotify, std::placeholders::_1));
	}
}

void ConnectAfterWorker(uv_work_t* req, int status)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	auto data = (ConnectWorkerData *)req->data;

	Local<Value> nullValue = Nan::Null();
	Local<Value> argv[1];
	argv[0] = data->m_result ? nullValue : Nan::Error("Unknown exception.");

	Local<Function>::New(isolate, data->m_callback)->Call(context, Null(isolate), 1, argv);
	data->m_callback.Reset();

	delete data;
}

void AsyncConnect(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	auto isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();

	if (info.Length() < 1 || !info[0]->IsInt32()) {
		Nan::ThrowTypeError("Arg[0] must be integer.");
		return;
	}
	if (info.Length() < 2 || !info[1]->IsFunction()) {
		Nan::ThrowTypeError("Arg[1] must be function.");
		return;
	}

	if (g_CGAInterface->IsConnected()) {
		info.GetReturnValue().Set(true);
		return;
	}

	auto port = info[0]->Int32Value(context).ToChecked();
	auto callback = Local<Function>::Cast(info[1]);

	auto data = new ConnectWorkerData(port);
	data->m_callback.Reset(isolate, callback);

	uv_queue_work(uv_default_loop(), &data->m_worker, ConnectWorker, ConnectAfterWorker);
}

void Connect(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
	Isolate* isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	Local<Context> context = isolate->GetCurrentContext();

	if (info.Length() < 1) {
		Nan::ThrowTypeError("Arg[0] must be a port number.");
		return;
	}

	if (g_CGAInterface->IsConnected()) {
		info.GetReturnValue().Set(true);
		return;
	}

	auto port = info[0]->Int32Value(context).ToChecked();

	auto bResult = g_CGAInterface->Connect(port);

	if (bResult)
	{
		g_CGAInterface->RegisterBattleActionNotify(std::bind(&BattleActionNotify, std::placeholders::_1));
		g_CGAInterface->RegisterTradeStateNotify(std::bind(&TradeStateNotify, std::placeholders::_1));
		g_CGAInterface->RegisterTradeDialogNotify(std::bind(&TradeDialogNotify, std::placeholders::_1));
		g_CGAInterface->RegisterTradeStuffsNotify(std::bind(&TradeStuffsNotify, std::placeholders::_1));
		g_CGAInterface->RegisterPlayerMenuNotify(std::bind(&PlayerMenuNotify, std::placeholders::_1));
		g_CGAInterface->RegisterUnitMenuNotify(std::bind(&UnitMenuNotify, std::placeholders::_1));
		g_CGAInterface->RegisterNPCDialogNotify(std::bind(&NPCDialogNotify, std::placeholders::_1));
		g_CGAInterface->RegisterWorkingResultNotify(std::bind(&WorkingResultNotify, std::placeholders::_1));
		g_CGAInterface->RegisterChatMsgNotify(std::bind(&ChatMsgNotify, std::placeholders::_1));
		g_CGAInterface->RegisterDownloadMapNotify(std::bind(&DownloadMapNotify, std::placeholders::_1));
	}

	info.GetReturnValue().Set(bResult);
}