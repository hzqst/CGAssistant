#include <Windows.h>
#include <stdlib.h>
#include "gameservice.h"

DWORD WINAPI CGAServerThread(LPVOID);
LRESULT CALLBACK NewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LONG WINAPI MinidumpCallback(EXCEPTION_POINTERS* pException);
extern CGA::CGAService g_CGAService;

int g_MainPort = 0;
HWND g_MainHwnd = NULL;
ULONG g_MainThreadId = 0;
WNDPROC g_OldProc = NULL;
HANDLE g_hQuitEvent = NULL;
HANDLE g_hFileMapping = NULL;
HANDLE g_hPortMutex;
HANDLE g_hDataLock = NULL;
HANDLE g_hServreThread = NULL;
char g_BasicWindowTitle[256] = {0};

LPCWSTR ExtractFileName(LPCWSTR szPath)
{
	SIZE_T len = wcslen(szPath);
	if (len < 2)
		return szPath;

	for (size_t i = len - 2; i >= 0; --i)
	{
		if (szPath[i] == L'\\' || szPath[i] == L'/')
			return &szPath[i + 1];
	}
	return szPath;
}

LRESULT UpdateGameWindowTitle(VOID)
{
	char szNewTitle[256] = { 0 };
	if (g_CGAService.IsInGame() && g_CGAService.GetPlayerName() && g_CGAService.GetPlayerName()[0])
		_snprintf(szNewTitle, 256, "%s CGA [%s] (%d线) #%d", g_BasicWindowTitle, g_CGAService.GetPlayerName(), g_CGAService.GetServerIndex(), g_MainPort - CGA_PORT_BASE + 1);
	else
		_snprintf(szNewTitle, 256, "%s CGA #%d", g_BasicWindowTitle, g_MainPort - CGA_PORT_BASE + 1);
	return DefWindowProcA(g_MainHwnd, WM_SETTEXT, NULL, (LPARAM)szNewTitle);
}

LRESULT CALLBACK NewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
	{
		if (g_hQuitEvent)
			SetEvent(g_hQuitEvent);
		if (g_hServreThread)
		{
			WaitForSingleObject(g_hServreThread, INFINITE);
			CloseHandle(g_hServreThread);
			g_hServreThread = NULL;
		}
		g_CGAService.Uninitialize();
		if (g_hDataLock)
		{
			g_hDataLock = NULL;
			CloseHandle(g_hDataLock);
		}
		if (g_hFileMapping) 
		{
			CloseHandle(g_hFileMapping);
			g_hFileMapping = NULL;
		}
		if (g_hQuitEvent)
		{
			CloseHandle(g_hQuitEvent);
			g_hQuitEvent = NULL;
		}
		if (g_hPortMutex)
		{
			CloseHandle(g_hPortMutex);
			g_hPortMutex = NULL;
		}
		break;
	}
	case WM_SETTEXT: 
	{
		if (g_MainPort)
		{
			strcpy(g_BasicWindowTitle, (const char *)lParam);
			return UpdateGameWindowTitle();
		}
		break;
	}
	case WM_CGA_BATTLE_NORMALATTACK:
		return g_CGAService.WM_BattleNormalAttack(wParam) ? 1 : 0;
	case WM_CGA_BATTLE_SKILLATTACK:
		return g_CGAService.WM_BattleSkillAttack((wParam & 0xFF), (wParam >> 8), lParam) ? 1 : 0;
	case WM_CGA_BATTLE_GUARD:
		return g_CGAService.WM_BattleGuard() ? 1 : 0;
	case WM_CGA_BATTLE_REBIRTH:
		return g_CGAService.WM_BattleRebirth() ? 1 : 0;
	case WM_CGA_BATTLE_ESCAPE:
		return g_CGAService.WM_BattleEscape() ? 1 : 0;
	case WM_CGA_BATTLE_EXCHANGE_POSITION:
		return g_CGAService.WM_BattleExchangePosition() ? 1 : 0;
	case WM_CGA_BATTLE_CHANGE_PET:
		return g_CGAService.WM_BattleChangePet(wParam) ? 1 : 0;
	case WM_CGA_BATTLE_USEITEM:
		return g_CGAService.WM_BattleUseItem(wParam, lParam) ? 1 : 0;
	case WM_CGA_BATTLE_PETSKILLATTACK:
		return g_CGAService.WM_BattlePetSkillAttack(wParam, lParam) ? 1 : 0;
	case WM_CGA_BATTLE_DONOTHING:
		return g_CGAService.WM_BattleDoNothing() ? 1 : 0;
	case WM_CGA_DROP_ITEM:
		return g_CGAService.WM_DropItem(wParam) ? 1 : 0;
	case WM_CGA_DROP_PET:
		return g_CGAService.WM_DropPet(wParam) ? 1 : 0;
	case WM_CGA_CHANGE_PET_STATE:
		return g_CGAService.WM_ChangePetState(wParam, lParam) ? 1 : 0;
	case WM_CGA_USE_ITEM:
		return g_CGAService.WM_UseItem(wParam) ? 1 : 0;
	case WM_CGA_MOVE_ITEM:
		return g_CGAService.WM_MoveItem((CGA::move_xxx_t *)wParam) ? 1 : 0;
	case WM_CGA_MOVE_PET:
		return g_CGAService.WM_MovePet((CGA::move_xxx_t *)wParam) ? 1 : 0;
	case WM_CGA_MOVE_GOLD:
		return g_CGAService.WM_MoveGold(wParam, lParam) ? 1 : 0;
	case WM_CGA_LOG_BACK:
		g_CGAService.WM_LogBack();
		return 1;
	case WM_CGA_LOG_OUT:
		g_CGAService.WM_LogOut();
		return 1;
	case WM_CGA_GET_MAP_UNITS:
		g_CGAService.WM_GetMapUnits((CGA::cga_map_units_t *)wParam);
		return 1;
	case WM_CGA_GET_MAP_NAME:
		g_CGAService.WM_GetMapName((std::string *)wParam);
		return 1;
	case WM_CGA_GET_PET_INFO:
		g_CGAService.WM_GetPetInfo(wParam, (CGA::cga_pet_info_t *)lParam);
		return 1;
	case WM_CGA_GET_PETS_INFO:
		g_CGAService.WM_GetPetsInfo((CGA::cga_pets_info_t *)wParam);
		return 1;
	case WM_CGA_GET_BANK_PETS_INFO:
		g_CGAService.WM_GetBankPetsInfo((CGA::cga_pets_info_t *)wParam);
		return 1;
	case WM_CGA_GET_SKILL_INFO:
		g_CGAService.WM_GetSkillInfo(wParam, (CGA::cga_skill_info_t *)lParam);
		return 1;
	case WM_CGA_GET_SKILLS_INFO:
		g_CGAService.WM_GetSkillsInfo((CGA::cga_skills_info_t *)wParam);
		return 1;
	case WM_CGA_GET_SUBSKILL_INFO:
		g_CGAService.WM_GetSubSkillInfo(wParam & 0xFF, (wParam >> 8) & 0xFF, (CGA::cga_subskill_info_t *)lParam);
		return 1;
	case WM_CGA_GET_SUBSKILLS_INFO:
		g_CGAService.WM_GetSubSkillsInfo(wParam, (CGA::cga_subskills_info_t *)lParam);
		return 1;
	case WM_CGA_GET_PLAYER_INFO:
		g_CGAService.WM_GetPlayerInfo((CGA::cga_player_info_t *)wParam);
		return 1;
	case WM_CGA_GET_ITEM_INFO:
		g_CGAService.WM_GetItemInfo(wParam, (CGA::cga_item_info_t *)lParam);
		return 1;
	case WM_CGA_GET_ITEMS_INFO:
		g_CGAService.WM_GetItemsInfo((CGA::cga_items_info_t *)wParam);
		return 1;
	case WM_CGA_GET_BANK_ITEMS_INFO:
		g_CGAService.WM_GetBankItemsInfo((CGA::cga_items_info_t *)wParam);
		return 1;
	case WM_CGA_GET_BATTLE_UNIT_INFO:
		g_CGAService.WM_GetBattleUnit(wParam, (CGA::cga_battle_unit_t *)lParam);
		return 1;
	case WM_CGA_GET_BATTLE_UNITS_INFO:
		g_CGAService.WM_GetBattleUnits((CGA::cga_battle_units_t *)wParam);
		return 1;
	case WM_CGA_GET_PET_SKILL_INFO:
		g_CGAService.WM_GetPetSkillInfo(wParam & 0xFF, (wParam >> 8) & 0xFF, (CGA::cga_pet_skill_info_t *)lParam);
		return 1;
	case WM_CGA_GET_PET_SKILLS_INFO:
		g_CGAService.WM_GetPetSkillsInfo(wParam, (CGA::cga_pet_skills_info_t *)lParam);
		return 1;
	case WM_CGA_CLICK_NPC_DIALOG:
		return g_CGAService.WM_ClickNPCDialog(wParam, lParam) ? 1 : 0;
	case WM_CGA_SELL_NPC_STORE:
		return g_CGAService.WM_SellNPCStore((CGA::cga_sell_items_t *)wParam) ? 1 : 0;
	case WM_CGA_BUY_NPC_STORE:
		return g_CGAService.WM_BuyNPCStore((CGA::cga_buy_items_t *)wParam) ? 1 : 0;
	case WM_CGA_SAY_WORDS:
		g_CGAService.WM_SayWords((const char *)wParam, lParam & 0xFF, (lParam >> 8) & 0xFF, (lParam >> 16) & 0xFF);
		return 1;
	case WM_CGA_START_WORK:
		return g_CGAService.WM_StartWork(wParam, lParam) ? 1 : 0;
	case WM_CGA_CRAFT_ITEM:
		return g_CGAService.WM_CraftItem((CGA::cga_craft_item_t *)wParam) ? 1 : 0;
	case WM_CGA_GET_CRAFT_INFO:
		g_CGAService.WM_GetCraftInfo((CGA::cga_craft_item_t *)wParam, (CGA::cga_craft_info_t *)lParam);
		return 1;
	case WM_CGA_GET_CRAFTS_INFO:
		g_CGAService.WM_GetCraftsInfo(wParam, (CGA::cga_crafts_info_t *)lParam);
		return 1;
	case WM_CGA_IS_MAP_CELL_PASSABLE:
		return g_CGAService.WM_IsMapCellPassable(wParam, lParam) ? 1 : 0;
	case WM_CGA_FORCE_MOVE:
		return g_CGAService.WM_ForceMove(wParam, lParam ? true : false) ? 1 : 0;
	case WM_CGA_FORCE_MOVE_TO:
		return g_CGAService.WM_ForceMoveTo(wParam & 0xFFFF, (wParam >> 16) & 0xFFFF, lParam ? true : false) ? 1 : 0;
	case WM_CGA_ASSESS_ITEM:
		return g_CGAService.WM_AssessItem(wParam, lParam);
	case WM_CGA_DO_REQUEST:
		return g_CGAService.WM_DoRequest((int)wParam) ? true : false;
	case WM_CGA_TRADE_ADD_STUFFS:
		g_CGAService.WM_TradeAddStuffs((CGA::cga_trade_stuff_t *)wParam);
		return 1;
	case WM_CGA_GET_TEAM_PLAYERS_INFO:
		g_CGAService.WM_GetTeamPlayerInfo((CGA::cga_team_players_t *)wParam);
		return 1;
	case WM_CGA_FIX_WARP_STUCK:
		g_CGAService.WM_FixMapWarpStuck((int)wParam);
		return 1;
	case WM_CGA_GET_MOVE_HISTORY:
		g_CGAService.WM_GetMoveHistory((std::vector<DWORD> *)wParam);
		return 1;
	case WM_CGA_ENABLE_FLAGS:
		return g_CGAService.WM_EnableFlags((int)wParam, lParam ? true : false) ? true : false;
	case WM_CGA_PLAYER_MENU_SELECT:
		return g_CGAService.WM_PlayerMenuSelect((int)wParam, (const char *)lParam) ? true : false;
	case WM_CGA_UNIT_MENU_SELECT:
		return g_CGAService.WM_UnitMenuSelect((int)wParam) ? true : false;
	case WM_CGA_SET_WINDOW_RESOLUTION:
		g_CGAService.WM_SetWindowResolution((int)wParam, (int)lParam);
		return 1;
	case WM_CGA_REQUEST_DOWNLOAD_MAP:
		g_CGAService.WM_RequestDownloadMap((int)wParam & 0xffff, (int)lParam & 0xffff, (int)(wParam >> 16) & 0xffff, (int)(lParam >> 16) & 0xffff);
		return 1;
	case WM_CGA_GET_COLLISION_TABLE_RAW:
		g_CGAService.WM_GetMapCollisionTableRaw(wParam ? true : false, (CGA::cga_map_cells_t *)lParam);
		return 1;
	case WM_CGA_GET_COLLISION_TABLE:
		g_CGAService.WM_GetMapCollisionTable(wParam ? true : false, (CGA::cga_map_cells_t *)lParam);
		return 1;
	case WM_CGA_GET_OBJECT_TABLE:
		g_CGAService.WM_GetMapObjectTable(wParam ? true : false, (CGA::cga_map_cells_t *)lParam);
		return 1;
	case WM_CGA_GET_TILE_TABLE:
		g_CGAService.WM_GetMapTileTable(wParam ? true : false, (CGA::cga_map_cells_t *)lParam);
		return 1;
	case WM_CGA_UPGRADE_PLAYER:
		g_CGAService.WM_UpgradePlayer((int)wParam);
		return 1;
	case WM_CGA_UPGRADE_PET:
		g_CGAService.WM_UpgradePet((int)wParam, (int)lParam);
		return 1;
	case WM_CGA_CHANGE_NICK_NAME:
		return g_CGAService.WM_ChangeNickName((const char *)wParam);
	case WM_CGA_CHANGE_TITLE_NAME:
		return g_CGAService.WM_ChangeTitleName((int)wParam);
	case WM_CGA_CHANGE_PERS_DESC:
		g_CGAService.WM_ChangePersDesc((CGA::cga_pers_desc_t *)wParam);
		return 1;
	case WM_KEYDOWN:
		if (GetForegroundWindow() == g_MainHwnd)
		{
			switch (wParam)
			{
			case VK_F1:
				g_CGAService.WM_LogBack();
				return 1;
			case VK_F2:
				g_CGAService.WM_LogOut();
				return 1;
			case VK_F3:
				g_CGAService.WM_ForceMove(g_CGAService.GetMouseOrientation(), true);
				return 1;
			case VK_F4:
				g_CGAService.AddAllTradeItems();
				return 1;
			}
		}
	}

	return CallWindowProcA(g_OldProc, hWnd, message, wParam, lParam);
}

LRESULT CALLBACK NewWndProcPOLCN(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
	{
		if (g_hQuitEvent)
			SetEvent(g_hQuitEvent);
		if (g_hServreThread)
		{
			WaitForSingleObject(g_hServreThread, INFINITE);
			CloseHandle(g_hServreThread);
			g_hServreThread = NULL;
		}
		g_CGAService.Uninitialize();
		if (g_hDataLock)
		{
			g_hDataLock = NULL;
			CloseHandle(g_hDataLock);
		}
		if (g_hFileMapping)
		{
			CloseHandle(g_hFileMapping);
			g_hFileMapping = NULL;
		}
		if (g_hQuitEvent)
		{
			CloseHandle(g_hQuitEvent);
			g_hQuitEvent = NULL;
		}
		if (g_hPortMutex)
		{
			CloseHandle(g_hPortMutex);
			g_hPortMutex = NULL;
		}
		break;
	}
	case WM_CGA_LOGIN_GAME_SERVER:
		g_CGAService.WM_SendClientLogin((const char *)wParam, (const char *)lParam, 4);
		return 1;
	}

	return CallWindowProcA(g_OldProc, hWnd, message, wParam, lParam);
}

void InitializeHooks(int ThreadId, HWND hWnd, CGA::game_type type)
{
	g_MainThreadId = ThreadId;
	g_MainHwnd = hWnd;

	SetUnhandledExceptionFilter(MinidumpCallback);

	g_CGAService.Initialize(type);

	if (hWnd)
	{
		g_OldProc = (WNDPROC)GetWindowLongPtrA(hWnd, GWL_WNDPROC);
		SetWindowLongPtrA(hWnd, GWL_WNDPROC, (LONG_PTR)(type == CGA::game_type::polcn ? NewWndProcPOLCN : NewWndProc));

		g_hServreThread = CreateThread(NULL, 0, CGAServerThread, NULL, 0, NULL);
	}
}

extern "C"
{
	__declspec(dllexport) LRESULT WINAPI GetMsgProc(int Code, WPARAM wParam, LPARAM lParam)
	{
		PMSG pMsg = (PMSG)lParam;
		if (Code == HC_ACTION && pMsg->hwnd != NULL && !g_MainHwnd)
		{
			char szClass[256];
			if (GetClassNameA(pMsg->hwnd, szClass, 256))
			{
				WCHAR szModulePath[MAX_PATH];
				GetModuleFileNameW(NULL, szModulePath, MAX_PATH);
				LPCWSTR pModuleName = ExtractFileName(szModulePath);
				if (!_wcsicmp(pModuleName, L"cg_se_3000.exe") && !strcmp(szClass, "魔力宝贝"))
				{
					InitializeHooks(GetCurrentThreadId(), pMsg->hwnd, CGA::cg_se_3000);
				}
				else if (!_wcsicmp(pModuleName, L"cg_item_6000.exe") && !strcmp(szClass, "魔力宝贝"))
				{
					InitializeHooks(GetCurrentThreadId(), pMsg->hwnd, CGA::cg_item_6000);
				}
				else if (!_wcsicmp(pModuleName, L"POLCN_Launcher.exe"))
				{
					InitializeHooks(GetCurrentThreadId(), pMsg->hwnd, CGA::polcn);
				}				
			}
		}
		return CallNextHookEx(NULL, Code, wParam, lParam);
	}
}

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		WCHAR szModulePath[MAX_PATH];
		GetModuleFileNameW(NULL, szModulePath, MAX_PATH);
		LPCWSTR pModuleName = ExtractFileName(szModulePath);

		if (!_wcsicmp(pModuleName, L"POLCN_Launcher.exe"))
		{
			auto hWnd = FindWindowA(NULL, "“易玩通”娱乐平台");
			if (hWnd)
			{
				DWORD pid;
				if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId())
					return TRUE;
			}

			InitializeHooks(GetCurrentThreadId(), NULL, CGA::polcn);
		}
	}
	return TRUE;
}