#define _WIN32_WINNT 0x0501

#include <string>
#include <rest_rpc/server.hpp>
#include <memory>
#include <Windows.h>
#include "gameservice.h"

using server_t = timax::rpc::server<timax::rpc::msgpack_codec>;

size_t pool_size = 16;
std::shared_ptr<server_t> server;

extern CGA::CGAService g_CGAService;

extern int g_MainPort;
extern HWND g_MainHwnd;
extern ULONG g_MainThreadId;
extern HANDLE g_hQuitEvent;
extern HANDLE g_hPortMutex;
extern HANDLE g_hFileMapping;
extern HANDLE g_hDataLock;

void WriteLog(LPCSTR fmt, ...);

void CGA_NotifyBattleAction(int flags)
{
	if (server && server.get()) {
		server->pub("NotifyBattleAction", flags);
	}
}

void CGA_NotifyPlayerMenu(const CGA::cga_player_menu_items_t &players)
{
	if (server && server.get()) {
		server->pub("NotifyPlayerMenu", players);
	}
}

void CGA_NotifyUnitMenu(const CGA::cga_unit_menu_items_t &units)
{
	if (server && server.get()) {
		server->pub("NotifyUnitMenu", units);
	}
}

void CGA_NotifyNPCDialog(const CGA::cga_npc_dialog_t &dlg)
{
	if (server && server.get()) {
		server->pub("NotifyNPCDialog", dlg);
	}
}

void CGA_NotifyWorkingResult(const CGA::cga_working_result_t &result)
{
	if (server && server.get()) {
		server->pub("NotifyWorkingResult", result);
	}
}

void CGA_NotifyChatMsg(const CGA::cga_chat_msg_t &msg)
{
	if (server && server.get()) {
		server->pub("NotifyChatMsg", msg);
	}
}

void CGA_NotifyTradeStuffs(const CGA::cga_trade_stuff_info_t &msg)
{
	if (server && server.get()) {
		server->pub("NotifyTradeStuffs", msg);
	}
}

void CGA_NotifyTradeDialog(const CGA::cga_trade_dialog_t &msg)
{
	if (server && server.get()) {
		server->pub("NotifyTradeDialog", msg);
	}
}

void CGA_NotifyTradeState(int state)
{
	if (server && server.get()) {
		server->pub("NotifyTradeState", state);
	}
}

void CGA_NotifyDownloadMap(const CGA::cga_download_map_t &msg)
{
	if (server && server.get()) {
		server->pub("NotifyDownloadMap", msg);
	}
}

void CGA_NotifyConnectionState(const CGA::cga_conn_state_t &msg)
{
	if (server && server.get()) {
		server->pub("NotifyConnectionState", msg);
	}
}

BOOL CGA_CreatePortMutex(int port)
{
	WCHAR szMutex[64];
	wsprintfW(szMutex, L"CGAPort_%d", port);
	HANDLE hMutex = CreateMutexW(NULL, TRUE, szMutex);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (hMutex)
			CloseHandle(hMutex);
		return FALSE;
	}

	g_hPortMutex = hMutex;
	return TRUE;
}

void CGA_CreateSharedData(int port)
{
	ULONG ProcessId = GetCurrentProcessId();

	WCHAR szLockName[32];	
	wsprintfW(szLockName, L"CGASharedDataLock_%d", ProcessId);
	g_hDataLock = CreateMutexW(NULL, TRUE, szLockName);

	WCHAR szMappingName[32];
	wsprintfW(szMappingName, L"CGASharedData_%d", ProcessId);
	g_hFileMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CGA::CGAShare_t), szMappingName);
	if (g_hFileMapping && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		LPVOID pViewOfFile = MapViewOfFile(g_hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pViewOfFile)
		{
			CGA::CGAShare_t *data = (CGA::CGAShare_t *)pViewOfFile;

			data->ProcessId = ProcessId;
			data->ThreadId = g_MainThreadId;
			data->hWnd = (int)g_MainHwnd;
			data->Port = port;

			UnmapViewOfFile(pViewOfFile);
		}
	}
	ReleaseMutex(g_hDataLock);

	g_MainPort = port;

	char szTitle[64];
	GetWindowTextA(g_MainHwnd, szTitle, 64);
	SetWindowTextA(g_MainHwnd, szTitle);
}

DWORD WINAPI CGAServerThread(LPVOID)
{
	using namespace CGA;

	g_hQuitEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

	for (uint16_t port = CGA_PORT_BASE; port < CGA_PORT_BASE + 1000; port++)
	{
		DWORD dwWait = WAIT_OBJECT_0;

		if (CGA_CreatePortMutex(port))
		{
			CGA_CreateSharedData(port);

			try
			{
				server = std::make_shared<server_t>(port, pool_size, std::chrono::seconds{ 15 });

				server->register_handler("Initialize", timax::bind(&CGAService::InitializeGameData, &g_CGAService));
				server->register_handler("Connect", timax::bind(&CGAService::Connect, &g_CGAService));

				server->register_handler("IsInGame", timax::bind(&CGAService::IsInGame, &g_CGAService));
				server->register_handler("GetWorldStatus", timax::bind(&CGAService::GetWorldStatus, &g_CGAService));
				server->register_handler("GetGameStatus", timax::bind(&CGAService::GetGameStatus, &g_CGAService));
				server->register_handler("GetBGMIndex", timax::bind(&CGAService::GetBGMIndex, &g_CGAService));
				server->register_handler("GetSysTime", timax::bind(&CGAService::GetSysTime, &g_CGAService));

				server->register_handler("GetPlayerInfo", timax::bind(&CGAService::GetPlayerInfo, &g_CGAService));
				server->register_handler("SetPlayerFlagEnabled", timax::bind(&CGAService::SetPlayerFlagEnabled, &g_CGAService));
				server->register_handler("IsPlayerFlagEnabled", timax::bind(&CGAService::IsPlayerFlagEnabled, &g_CGAService));

				server->register_handler("IsSkillValid", timax::bind(&CGAService::IsSkillValid, &g_CGAService));
				server->register_handler("GetSkillInfo", timax::bind(&CGAService::GetSkillInfo, &g_CGAService));
				server->register_handler("GetSkillsInfo", timax::bind(&CGAService::GetSkillsInfo, &g_CGAService));
				server->register_handler("GetSubSkillInfo", timax::bind(&CGAService::GetSubSkillInfo, &g_CGAService));
				server->register_handler("GetSubSkillsInfo", timax::bind(&CGAService::GetSubSkillsInfo, &g_CGAService));

				server->register_handler("IsPetValid", timax::bind(&CGAService::IsPetValid, &g_CGAService));
				server->register_handler("GetPetInfo", timax::bind(&CGAService::GetPetInfo, &g_CGAService));
				server->register_handler("GetPetsInfo", timax::bind(&CGAService::GetPetsInfo, &g_CGAService));
				server->register_handler("GetBankPetsInfo", timax::bind(&CGAService::GetBankPetsInfo, &g_CGAService));
				server->register_handler("IsPetSkillValid", timax::bind(&CGAService::IsPetSkillValid, &g_CGAService));
				server->register_handler("GetPetSkillInfo", timax::bind(&CGAService::GetPetSkillInfo, &g_CGAService));
				server->register_handler("GetPetSkillsInfo", timax::bind(&CGAService::GetPetSkillsInfo, &g_CGAService));

				server->register_handler("GetMapName", timax::bind(&CGAService::GetMapName, &g_CGAService));
				server->register_handler("GetMapIndex", timax::bind(&CGAService::GetMapIndex, &g_CGAService));
				server->register_handler("GetMapXY", timax::bind(&CGAService::GetMapXY, &g_CGAService));
				server->register_handler("GetMapXYFloat", timax::bind(&CGAService::GetMapXYFloat, &g_CGAService));
				server->register_handler("GetMapUnits", timax::bind(&CGAService::GetMapUnits, &g_CGAService));
				server->register_handler("GetMapCollisionTable", timax::bind(&CGAService::GetMapCollisionTable, &g_CGAService));
				server->register_handler("GetMapCollisionTableRaw", timax::bind(&CGAService::GetMapCollisionTableRaw, &g_CGAService));
				server->register_handler("GetMapObjectTable", timax::bind(&CGAService::GetMapObjectTable, &g_CGAService));
				server->register_handler("GetMapTileTable", timax::bind(&CGAService::GetMapTileTable, &g_CGAService));
				server->register_handler("GetMoveSpeed", timax::bind(&CGAService::GetMoveSpeed, &g_CGAService));

				server->register_handler("WalkTo", timax::bind(&CGAService::WalkTo, &g_CGAService));
				server->register_handler("TurnTo", timax::bind(&CGAService::TurnTo, &g_CGAService));
				server->register_handler("SetMoveSpeed", timax::bind(&CGAService::SetMoveSpeed, &g_CGAService));
				server->register_handler("ForceMove", timax::bind(&CGAService::ForceMove, &g_CGAService));
				server->register_handler("ForceMoveTo", timax::bind(&CGAService::ForceMoveTo, &g_CGAService));
				server->register_handler("IsMapCellPassable", timax::bind(&CGAService::IsMapCellPassable, &g_CGAService));
				server->register_handler("LogBack", timax::bind(&CGAService::LogBack, &g_CGAService));
				server->register_handler("LogOut", timax::bind(&CGAService::LogOut, &g_CGAService));
				server->register_handler("SayWords", timax::bind(&CGAService::SayWords, &g_CGAService));
				server->register_handler("ChangeNickName", timax::bind(&CGAService::ChangeNickName, &g_CGAService));
				server->register_handler("ChangeTitleName", timax::bind(&CGAService::ChangeTitleName, &g_CGAService));
				server->register_handler("ChangePersDesc", timax::bind(&CGAService::ChangePersDesc, &g_CGAService));

				server->register_handler("IsItemValid", timax::bind(&CGAService::IsItemValid, &g_CGAService));
				server->register_handler("GetItemInfo", timax::bind(&CGAService::GetItemInfo, &g_CGAService));
				server->register_handler("GetItemsInfo", timax::bind(&CGAService::GetItemsInfo, &g_CGAService));
				server->register_handler("GetBankItemsInfo", timax::bind(&CGAService::GetBankItemsInfo, &g_CGAService));
				server->register_handler("GetBankGold", timax::bind(&CGAService::GetBankGold, &g_CGAService));
				server->register_handler("UseItem", timax::bind(&CGAService::UseItem, &g_CGAService));
				server->register_handler("MoveItem", timax::bind(&CGAService::MoveItem, &g_CGAService));
				server->register_handler("MovePet", timax::bind(&CGAService::MovePet, &g_CGAService));
				server->register_handler("MoveGold", timax::bind(&CGAService::MoveGold, &g_CGAService));
				server->register_handler("DropItem", timax::bind(&CGAService::DropItem, &g_CGAService));
				server->register_handler("DropPet", timax::bind(&CGAService::DropPet, &g_CGAService));
				server->register_handler("ChangePetState", timax::bind(&CGAService::ChangePetState, &g_CGAService));
				server->register_handler("ClickNPCDialog", timax::bind(&CGAService::ClickNPCDialog, &g_CGAService));
				server->register_handler("SellNPCStore", timax::bind(&CGAService::SellNPCStore, &g_CGAService));
				server->register_handler("BuyNPCStore", timax::bind(&CGAService::BuyNPCStore, &g_CGAService));
				server->register_handler("PlayerMenuSelect", timax::bind(&CGAService::PlayerMenuSelect, &g_CGAService));
				server->register_handler("UnitMenuSelect", timax::bind(&CGAService::UnitMenuSelect, &g_CGAService));
				server->register_handler("UpgradePlayer", timax::bind(&CGAService::UpgradePlayer, &g_CGAService));
				server->register_handler("UpgradePet", timax::bind(&CGAService::UpgradePet, &g_CGAService));

				server->register_handler("IsBattleUnitValid", timax::bind(&CGAService::IsBattleUnitValid, &g_CGAService));
				server->register_handler("GetBattleUnit", timax::bind(&CGAService::GetBattleUnit, &g_CGAService));
				server->register_handler("GetBattleUnits", timax::bind(&CGAService::GetBattleUnits, &g_CGAService));
				server->register_handler("GetBattleContext", timax::bind(&CGAService::GetBattleContext, &g_CGAService));
				server->register_handler("BattleNormalAttack", timax::bind(&CGAService::BattleNormalAttack, &g_CGAService));
				server->register_handler("BattleSkillAttack", timax::bind(&CGAService::BattleSkillAttack, &g_CGAService));
				server->register_handler("BattleRebirth", timax::bind(&CGAService::BattleRebirth, &g_CGAService));
				server->register_handler("BattleGuard", timax::bind(&CGAService::BattleGuard, &g_CGAService));
				server->register_handler("BattleEscape", timax::bind(&CGAService::BattleEscape, &g_CGAService));
				server->register_handler("BattleExchangePosition", timax::bind(&CGAService::BattleExchangePosition, &g_CGAService));
				server->register_handler("BattleDoNothing", timax::bind(&CGAService::BattleDoNothing, &g_CGAService));
				server->register_handler("BattleChangePet", timax::bind(&CGAService::BattleChangePet, &g_CGAService));
				server->register_handler("BattleUseItem", timax::bind(&CGAService::BattleUseItem, &g_CGAService));
				server->register_handler("BattlePetSkillAttack", timax::bind(&CGAService::BattlePetSkillAttack, &g_CGAService));
				server->register_handler("BattleSetHighSpeedEnabled", timax::bind(&CGAService::BattleSetHighSpeedEnabled, &g_CGAService));
				server->register_handler("BattleSetShowHPMPEnabled", timax::bind(&CGAService::BattleSetShowHPMPEnabled, &g_CGAService));
				server->register_handler("GetBattleEndTick", timax::bind(&CGAService::GetBattleEndTick, &g_CGAService));
				server->register_handler("SetBattleEndTick", timax::bind(&CGAService::SetBattleEndTick, &g_CGAService));

				server->register_handler("SetWorkDelay", timax::bind(&CGAService::SetWorkDelay, &g_CGAService));
				server->register_handler("StartWork", timax::bind(&CGAService::StartWork, &g_CGAService));
				server->register_handler("SetWorkAcceleration", timax::bind(&CGAService::SetWorkAcceleration, &g_CGAService));
				server->register_handler("SetImmediateDoneWork", timax::bind(&CGAService::SetImmediateDoneWork, &g_CGAService));
				server->register_handler("GetImmediateDoneWorkState", timax::bind(&CGAService::GetImmediateDoneWorkState, &g_CGAService));
				server->register_handler("CraftItem", timax::bind(&CGAService::CraftItem, &g_CGAService));
				server->register_handler("AssessItem", timax::bind(&CGAService::AssessItem, &g_CGAService));
				server->register_handler("GetCraftInfo", timax::bind(&CGAService::GetCraftInfo, &g_CGAService));
				server->register_handler("GetCraftsInfo", timax::bind(&CGAService::GetCraftsInfo, &g_CGAService));
				server->register_handler("GetCraftStatus", timax::bind(&CGAService::GetCraftStatus, &g_CGAService));
				server->register_handler("AssessItem", timax::bind(&CGAService::AssessItem, &g_CGAService));
				server->register_handler("DoRequest", timax::bind(&CGAService::DoRequest, &g_CGAService));
				server->register_handler("TradeAddStuffs", timax::bind(&CGAService::TradeAddStuffs, &g_CGAService));
				server->register_handler("IsUIDialogPresent", timax::bind(&CGAService::IsUIDialogPresent, &g_CGAService));
				server->register_handler("GetTeamPlayerInfo", timax::bind(&CGAService::GetTeamPlayerInfo, &g_CGAService));
				server->register_handler("FixMapWarpStuck", timax::bind(&CGAService::FixMapWarpStuck, &g_CGAService));
				server->register_handler("SetNoSwitchAnim", timax::bind(&CGAService::SetNoSwitchAnim, &g_CGAService));
				server->register_handler("GetMoveHistory", timax::bind(&CGAService::GetMoveHistory, &g_CGAService));
				server->register_handler("EnableFlags", timax::bind(&CGAService::EnableFlags, &g_CGAService));
				server->register_handler("SetWindowResolution", timax::bind(&CGAService::SetWindowResolution, &g_CGAService));
				server->register_handler("RequestDownloadMap", timax::bind(&CGAService::RequestDownloadMap, &g_CGAService));
				server->register_handler("GetNextAnimTickCount", timax::bind(&CGAService::GetNextAnimTickCount, &g_CGAService));
				server->register_handler("LoginGameServer", timax::bind(&CGAService::LoginGameServer, &g_CGAService));

				server->start();

				dwWait = WaitForSingleObject(g_hQuitEvent, INFINITE);

				if (dwWait == WAIT_OBJECT_0)
				{
					server->pub("NotifyServerShutdown", port);
				}

				server->stop();

				return 0;
			}
			catch (...)
			{
				if (dwWait != WAIT_OBJECT_0)
				{
					continue;
				}
				else
				{
					break;
				}
			}
		}
	}

	return 0;
}