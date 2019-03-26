#include <Windows.h>
#include <Detours.h>
#include "gameservice.h"
#include <ddraw.h>
#include <boost/locale.hpp>

extern HWND g_MainHwnd;

const int index2player_flags[] = {
	PLAYER_ENABLE_FLAGS_PK,
	PLAYER_ENABLE_FLAGS_TEAMCHAT,
	PLAYER_ENABLE_FLAGS_JOINTEAM,
	PLAYER_ENABLE_FLAGS_CARD,
	PLAYER_ENABLE_FLAGS_TRADE,
	PLAYER_ENABLE_FLAGS_FAMILY
};

void WriteLog(LPCSTR fmt, ...)
{
	char buffer[256];
	va_list argptr;
	int cnt;
	va_start(argptr, fmt);
	cnt = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	FILE *fp = fopen("D:\\cga_log.txt", "a+");
	if (fp)
	{
		setlocale(LC_ALL, "chs");
		fputs(buffer, fp);
		fclose(fp);
	}
}

using namespace CGA;

CGAService g_CGAService;

CXorValue::CXorValue(int value)
{
	int r = rand();
	key2 = r;
	refcount = 0;
	key1 = value ^ r;
}

int CXorValue::decode()
{
	return key1^key2;
}

void CXorValue::set(int value)
{
	key1 = rand();
	key2 = value ^ key1;
}

void CGA_NotifyBattleAction(int flags);
void CGA_NotifyPlayerMenu(const cga_player_menu_items_t &players);
void CGA_NotifyUnitMenu(const cga_unit_menu_items_t &units);
void CGA_NotifyNPCDialog(const cga_npc_dialog_t &dlg);
void CGA_NotifyWorkingResult(const CGA::cga_working_result_t &result);
void CGA_NotifyChatMsg(const CGA::cga_chat_msg_t &msg);

void CGAService::NewBATTLE_PlayerAction()
{
	int prevPlayerStatus = GetBattlePlayerStatus();

	BATTLE_PlayerAction();

	int playerStatus = GetBattlePlayerStatus();

	if (prevPlayerStatus == 2 && playerStatus == 0)
	{
		//Double action
		m_btl_double_action = true;
	}
	if (prevPlayerStatus != 1 && playerStatus == 1)
	{
		int flags = FL_BATTLE_ACTION_ISPLAYER;
		if (m_btl_double_action)
			flags |= FL_BATTLE_ACTION_ISDOUBLE;
		if (GetBattleSkillPerformed())
			flags |= FL_BATTLE_ACTION_ISSKILLPERFORMED;

		CGA_NotifyBattleAction(flags);
	}
	if (prevPlayerStatus != 4 && playerStatus == 4)
	{
		//Notify pet action
		int flags = 0;
		if (m_btl_double_action)
			flags |= FL_BATTLE_ACTION_ISDOUBLE;
		if (GetBattleSkillPerformed())
			flags |= FL_BATTLE_ACTION_ISSKILLPERFORMED;

		CGA_NotifyBattleAction(flags);
	}
}

void __cdecl NewBATTLE_PlayerAction()
{
	g_CGAService.NewBATTLE_PlayerAction();
}

void CGAService::NewNET_ParseBattlePackets(int a1, const char *buf)
{
	if (*buf == 'M')
	{
		if (m_btl_highspeed_enable)
		{
			if (strstr(buf, "END|"))//battle end
			{
				NET_ParseBattlePackets(a1, "M|END|");
			}
			else
			{
				NET_ParseBattlePackets(a1, "M|");
			}
			return;
		}
	}
	else if (*buf == 'C')
	{
		m_btl_double_action = false;
		ParseBattleUnits(buf, strlen(buf));
	}

	NET_ParseBattlePackets(a1, buf);
}

void __cdecl NewNET_ParseBattlePackets(int a1, const char *buf)
{
	g_CGAService.NewNET_ParseBattlePackets(a1, buf);
}

void CGAService::ParseGatheringResult(int success, const char *buf)
{
	int xp = 0;
	int count = 0;
	int levelup = 0;
	int endurance = 0;
	int skillful = 0;
	int intelligence = 0;
	int	imgid = 0;
	char item_name[32] = {0};
	int unk = 0;

	int parsed = sscanf(buf, "%d|%d|%d|%d|%d|%d|%d|%[^|]|%d",
		&xp, &count, &levelup, &endurance, &skillful, &intelligence, &imgid,
		item_name, &unk);

	if (parsed >= 8)
	{
		cga_working_result_t result(
			WORK_TYPE_GATHERING,
			success ? true : false,
			levelup ? true : false,
			xp,			
			endurance,
			skillful,
			intelligence
		);
		result.count = count;
		result.imgid = imgid;
		if (parsed >= 9)
			result.name = boost::locale::conv::to_utf<char>(item_name, "GBK");

		CGA_NotifyWorkingResult(result);
	}
}

void CGAService::ParseHealingResult(int success, const char *buf)
{
	int xp = 0;
	int unk = 0;
	int levelup = 0;
	int endurance = 0;
	int skillful = 0;
	int intelligence = 0;
	int	status = 0;

	int parsed = sscanf(buf, "%d|%d|%d|%d|%d|%d|%d",
		&xp, &unk, &levelup, &endurance, &skillful, &intelligence, &status);

	if (parsed >= 7)
	{
		cga_working_result_t result(
			WORK_TYPE_HEALING,
			success ? true : false,
			levelup ? true : false,
			xp,
			endurance,
			skillful,
			intelligence
		);
		result.status = status;

		CGA_NotifyWorkingResult(result);
	}
}

void CGAService::ParseAssessingResult(int success, const char *buf)
{
	int xp = 0;
	int unk = 0;
	int levelup = 0;
	int endurance = 0;
	int skillful = 0;
	int intelligence = 0;
	int	unk2 = 0;

	int parsed = sscanf(buf, "%d|%d|%d|%d|%d|%d|%d",
		&xp, &unk, &levelup, &endurance, &skillful, &intelligence, &unk2);

	if (parsed >= 7)
	{
		cga_working_result_t result(
			WORK_TYPE_ASSESSING,
			success ? true : false,
			levelup ? true : false,
			xp,
			endurance,
			skillful,
			intelligence
		);

		CGA_NotifyWorkingResult(result);
	}
}

void CGAService::ParseCraftingResult(int success, const char *buf)
{
	int xp = 0;
	int unk = 0;
	int levelup = 0;
	int endurance = 0;
	int skillful = 0;
	int intelligence = 0;
	int	imgid = 0;
	int imgid2 = 0;

	int parsed = sscanf(buf, "%d|%d|%d|%d|%d|%d|%d|%d",
		&xp, &unk, &levelup, &endurance, &skillful, &intelligence, &imgid, &imgid2);

	if (parsed >= 8)
	{
		cga_working_result_t result(
			WORK_TYPE_CRAFTING,
			success ? true : false,
			levelup ? true : false,
			xp,
			endurance,
			skillful,
			intelligence
		);
		result.imgid = imgid2;

		CGA_NotifyWorkingResult(result);
	}
}

void CGAService::NewNET_ParseWorkingResult(int a1, int success, int type, const char *buf)
{
	NET_ParseWorkingResult(a1, success, type, buf);

	switch (type)
	{
	case WORK_TYPE_GATHERING:
		ParseGatheringResult(success, buf);
		break;
	case WORK_TYPE_HEALING:
		ParseHealingResult(success, buf);
		break;
	case WORK_TYPE_ASSESSING:
		ParseAssessingResult(success, buf);
		break;
	case WORK_TYPE_CRAFTING:
		ParseCraftingResult(success, buf);
		break;
	}
}

void __cdecl NewNET_ParseWorkingResult(int a1, int success, int type, const char *buf)
{
	//WriteLog("type=%d, success=%d, buf=%s\n", type, success, buf);

	g_CGAService.NewNET_ParseWorkingResult(a1, success, type, buf);
}

void CGAService::NewNET_ParseChatMsg(int a1, int unitid, const char *buf, int color, int size)
{
	WriteLog("u=%d, buf=%s, color=%d, size=%d\n", unitid, buf, color, size);

	if (buf[0] == 'P' && buf[1] == '|')
	{
		cga_chat_msg_t msg(unitid, boost::locale::conv::to_utf<char>(buf + 2, "GBK"), color, size);
		CGA_NotifyChatMsg(msg);
	}

	NET_ParseChatMsg(a1, unitid, buf, color, size);
}

void __cdecl NewNET_ParseChatMsg(int a1, int unitid, const char *buf, int color, int size)
{
	g_CGAService.NewNET_ParseChatMsg(a1, unitid, buf, color, size);
}

void __cdecl NewR_DrawText(int a1)
{
	g_CGAService.R_DrawText(a1);
	g_CGAService.DrawBattleInfo();
}

void CGAService::NewMove_Player()
{
	(*g_playerBase)->move_speed = m_move_speed;

	if (m_move_to & 2)
	{
		m_move_to &= ~2;

		int x = *g_move_xdest;
		int y = *g_move_ydest;
		int states = *g_mouse_states;
		int rmouse = *g_move_turning;
		*g_move_xdest = m_move_to_x;
		*g_move_ydest = m_move_to_y;
		*g_mouse_states = 2;
		*g_move_turning = 1;

		Move_Player();

		*g_move_xdest = x;
		*g_move_ydest = y;
		*g_mouse_states = states;
		*g_move_turning = rmouse;
		return;
	}
	else if (m_move_to & 1)
	{
		m_move_to &= ~1;

		int x = *g_move_xdest;
		int y = *g_move_ydest;
		int states = *g_mouse_states;
		int lmouse = *g_move_walking;
		*g_move_xdest = m_move_to_x;
		*g_move_ydest = m_move_to_y;
		*g_mouse_states = 1;
		*g_move_walking = 1;
		g_move_destx->set(m_move_to_x);
		g_move_desty->set(m_move_to_y);

		Move_Player();

		*g_move_xdest = x;
		*g_move_ydest = y;
		*g_mouse_states = states;
		*g_move_walking = lmouse;
		return;
	}

	Move_Player();
}

void __cdecl NewMove_Player()
{
	g_CGAService.NewMove_Player();
}

void CGAService::NewUI_PlayerMenuShowup(int a1)
{
	UI_PlayerMenuShowup(a1);

	if (a1 == -1)
	{
		cga_player_menu_items_t players;

		int count = *g_player_menu_count - *g_player_menu_begin;
		players.reserve(count);
		for (int i = 0; i < count; ++i)
		{
			players.emplace_back(
				boost::locale::conv::to_utf<char>((*g_player_menu_name)[i], "GBK"),
				(*g_player_menu_color)[i],
				*g_player_menu_begin + i
			);
		}

		CGA_NotifyPlayerMenu(players);
	}
}

void __cdecl NewUI_PlayerMenuShowup(int a1)
{
	g_CGAService.NewUI_PlayerMenuShowup(a1);
}

int CGAService::NewUI_PlayerMenuMouseEvent(int widget_index, char flags)
{
	if (m_menu_item_selected == 1 && m_menu_item_select_index + 14 == widget_index)
	{
		flags = 2;
		m_menu_item_selected = 0;
	}
	return UI_PlayerMenuMouseEvent(widget_index, flags);
}

int __cdecl NewUI_PlayerMenuMouseEvent(int widget_index, char flags)
{
	return g_CGAService.NewUI_PlayerMenuMouseEvent(widget_index, flags);
}

void CGAService::NewUI_UnitMenuShowup(int a1)
{
	UI_UnitMenuShowup(a1);

	if (a1 == -1)
	{
		cga_unit_menu_items_t units;
		int count = *g_unit_menu_count;
		units.reserve(count);
		for (int i = 0; i < count; ++i)
		{
			units.emplace_back(
				boost::locale::conv::to_utf<char>((*g_unit_menu_base)[i].name, "GBK"),
				(*g_unit_menu_base)[i].level,
				(*g_unit_menu_base)[i].health,
				(*g_unit_menu_base)[i].hp,
				(*g_unit_menu_base)[i].maxhp,
				(*g_unit_menu_base)[i].mp,
				(*g_unit_menu_base)[i].maxmp,
				(*g_unit_menu_base)[i].color,
				*g_unit_menu_begin + i
			);
		}

		CGA_NotifyUnitMenu(units);
	}
}

void __cdecl NewUI_UnitMenuShowup(int a1)
{
	g_CGAService.NewUI_UnitMenuShowup(a1);
}

int CGAService::NewUI_UnitMenuMouseEvent(int widget_index, char flags)
{
	if (m_menu_item_selected == 2 && m_menu_item_select_index + 16 == widget_index) 
	{
		flags = 2;
		m_menu_item_selected = 0;
	}
	return UI_UnitMenuMouseEvent(widget_index, flags);
}

int __cdecl NewUI_UnitMenuMouseEvent(int widget_index, char flags)
{
	return g_CGAService.NewUI_UnitMenuMouseEvent(widget_index, flags);
}

void CGAService::NewNPC_ShowDialogInternal(int type, int options, int dlgid, int objid, const char *message)
{
	NPC_ShowDialogInternal(type, options, dlgid, objid, message);

	cga_npc_dialog_t dlg;
	dlg.type = type;
	dlg.options = options;
	dlg.dialog_id = dlgid;
	dlg.npc_id = objid;
	dlg.message = boost::locale::conv::to_utf<char>(message, "GBK");

	CGA_NotifyNPCDialog(dlg);
}

void __cdecl NewNPC_ShowDialogInternal(int type, int options, int dlgid, int objid, const char *message)
{
	g_CGAService.NewNPC_ShowDialogInternal(type, options, dlgid, objid, message);
}

char __cdecl NewSys_CheckModify(const char *a1)
{
	return 0;
}

int CGAService::NewUI_HandleMiniDialogMouseEvent(int widget, char flags)
{
	m_ui_minidialog_loop_index = 0;
	m_ui_minidialog_loop = true;
	int mouse_states = *g_mouse_states;

	if (m_ui_minidialog_click_index != -1)
		flags = 1;

	int result = UI_HandleMiniDialogMouseEvent(widget, flags);

	m_ui_minidialog_loop = false;
	*g_mouse_states = mouse_states;

	return result;
}

int __cdecl NewUI_HandleMiniDialogMouseEvent(int widget, char flags)
{
	return g_CGAService.NewUI_HandleMiniDialogMouseEvent(widget, flags);
}

int CGAService::NewUI_IsMouseInRect(int a1, int a2, int a3, int a4, int a5)
{
	if (m_ui_minidialog_loop)
	{
		if (m_ui_minidialog_loop_index == m_ui_minidialog_click_index)
		{
			*g_mouse_states |= 1;
			++m_ui_minidialog_loop_index;
			m_ui_minidialog_click_index = -1;
			return 1;
		}
		++ m_ui_minidialog_loop_index;
	}
	else if (m_ui_craftdialog_loop)
	{
		if (m_ui_craftdialog_loop_index == m_ui_craftdialog_click_index)
		{
			++m_ui_craftdialog_loop_index;
			m_ui_craftdialog_click_index = -1;
			return 1;
		}
		++m_ui_craftdialog_loop_index;
	}

	return g_CGAService.UI_IsMouseInRect(a1, a2, a3, a4, a5);
}

int __cdecl NewUI_IsMouseInRect(int a1, int a2, int a3, int a4, int a5)
{
	return g_CGAService.NewUI_IsMouseInRect(a1, a2, a3, a4, a5);
}

int __cdecl NewUI_HandleLearnSkillConfirmMouseEvent(int index, char flags)
{
	if (g_CGAService.m_ui_learn_skill_confirm == 1) 
	{
		flags = 2;
		g_CGAService.m_ui_learn_skill_confirm = -1;
	}

	return g_CGAService.UI_HandleLearnSkillConfirmMouseEvent(index, flags);
}

int CGAService::NewUI_HandleEnablePlayerFlagsMouseEvent(int index, char flags)
{
	if (index >= 17 && index < 17 + _ARRAYSIZE(index2player_flags))
	{
		int fl = index2player_flags[index - 17];
		if (m_change_player_enable_flags & fl)
		{
			if (((*g_playerBase)->enable_flags & fl) && !(m_desired_player_enable_flags & fl))
				flags = 2;
			else if (!((*g_playerBase)->enable_flags & fl) && (m_desired_player_enable_flags & fl))
				flags = 2;

			m_change_player_enable_flags &= ~fl;
		}
	}

	return UI_HandleEnablePlayerFlagsMouseEvent(index, flags);
}

int __cdecl NewUI_HandleEnablePlayerFlagsMouseEvent(int index, char flags)
{
	return g_CGAService.NewUI_HandleEnablePlayerFlagsMouseEvent(index, flags);
}

int CGAService::NewUI_HandleCraftItemSlotMouseEvent(int index, char flags)
{
	if (m_ui_craftdialog_additemcount)
	{
		m_ui_craftdialog_loop = true;
		m_ui_craftdialog_loop_index = 0;
		m_ui_craftdialog_click_index = -1;

		--m_ui_craftdialog_additemcount;

		for (int i = 0; i < 5; ++i)
		{
			if (m_ui_craftdialog_additem[i] != -1)
			{
				m_ui_craftdialog_click_index = m_ui_craftdialog_additem[i];
				m_ui_craftdialog_additem[i] = -1;				
				break;
			}
		}

		int states = *g_mouse_states;
		*g_mouse_states |= 0x10;
		flags |= 0x21;

		char temp[20];
		memcpy(temp, g_ui_craftdialog_additem_count, 20);
		memset(g_ui_craftdialog_additem_count, 0, 20);

		int result = UI_HandleCraftItemSlotMouseEvent(index, flags);

		for (int i = 0; i < 20; ++i)
			g_ui_craftdialog_additem_count[i] |= temp[i];

		m_ui_craftdialog_loop = false;
		m_ui_craftdialog_loop_index = -1;
		m_ui_craftdialog_click_index = -1;
		*g_mouse_states = states;

		if (!m_ui_craftdialog_additemcount)
			m_ui_craftdialog_click_begin = true;

		return result;
	}

	return UI_HandleCraftItemSlotMouseEvent(index, flags);
}

int __cdecl NewUI_HandleCraftItemSlotMouseEvent(int index, char flags)
{
	return g_CGAService.NewUI_HandleCraftItemSlotMouseEvent(index, flags);
}

int CGAService::NewUI_HandleCraftItemButtonMouseEvent(int index, char flags)
{
	if (m_ui_craftdialog_click_begin && (*g_craft_step == 0 || *g_craft_step == 3))
	{
		m_ui_craftdialog_click_begin = false;
		flags |= 2;		

		char backup_acc = *g_work_accelerate;
		char backup_percent = g_work_accelerate_percent[0];

		*g_work_accelerate = 1;
		g_work_accelerate_percent[0] = m_work_acceleration;

		int result = UI_HandleCraftItemButtonMouseEvent(index, flags);

		*g_work_accelerate = backup_acc;
		g_work_accelerate_percent[0] = backup_percent;

		return result;
	}

	return UI_HandleCraftItemButtonMouseEvent(index, flags);
}

int __cdecl NewUI_HandleCraftItemButtonMouseEvent(int index, char flags)
{
	return g_CGAService.NewUI_HandleCraftItemButtonMouseEvent(index, flags);
}

ULONG MH_GetModuleSize(HMODULE hModule)
{
	return ((IMAGE_NT_HEADERS *)((DWORD)hModule + ((IMAGE_DOS_HEADER *)hModule)->e_lfanew))->OptionalHeader.SizeOfImage;
}

void CGAService::Initialize(game_type type)
{
	if (m_initialized)
		return;

	HMODULE hGameBase = (HMODULE)GetModuleHandle(NULL);
	m_ImageBase = (ULONG_PTR)hGameBase;
	m_ImageSize = MH_GetModuleSize(hGameBase);

	m_game_type = type;

#define CONVERT_GAMEVAR(type, offset) (type)((ULONG_PTR)hGameBase + offset)

	if (m_game_type == cg_se_3000)
	{
		g_world_status = CONVERT_GAMEVAR(CXorValue *, 0xC0C350);
		g_game_status = CONVERT_GAMEVAR(CXorValue *, 0xC0C360);
		g_player_xpos = CONVERT_GAMEVAR(CXorValue *, 0x8B03B4);
		g_player_ypos = CONVERT_GAMEVAR(CXorValue *, 0x8B03E4);
		g_playerBase = CONVERT_GAMEVAR(playerbase_t **, 0xCAEF88);
		g_player_name = CONVERT_GAMEVAR(char *, 0xA150B0);
		g_bank_item_base = CONVERT_GAMEVAR(bank_item_info_t *, 0);//TODO
		g_pet_base = CONVERT_GAMEVAR(pet_t *, 0xC0A0D0);
		g_pet_id = CONVERT_GAMEVAR(int *, 0x8B918C);
		g_job_name = CONVERT_GAMEVAR(char *, 0xA16420);
		g_skill_base = CONVERT_GAMEVAR(skill_t *, 0xA1643C);
		g_map_x = CONVERT_GAMEVAR(short *, 0x71CB10);
		g_map_y = CONVERT_GAMEVAR(short *, 0x83B7F8);

		g_map_name = CONVERT_GAMEVAR(char *, 0x83B8C4);
		g_move_xdest = CONVERT_GAMEVAR(int *, 0x8B8990);
		g_move_ydest = CONVERT_GAMEVAR(int *, 0x8B8988);
		g_move_destx = CONVERT_GAMEVAR(CXorValue *, 0);//TODO
		g_move_desty = CONVERT_GAMEVAR(CXorValue *, 0);//TODO
		g_move_xspeed = CONVERT_GAMEVAR(float *, 0x83B8C0);
		g_move_yspeed = CONVERT_GAMEVAR(float *, 0x81EEF4);
		g_map_xf = CONVERT_GAMEVAR(float *, 0x5D6164);
		g_map_yf = CONVERT_GAMEVAR(float *, 0x820384);
		g_move_walking = CONVERT_GAMEVAR(int *, 0x8B89B4);
		g_move_turning = CONVERT_GAMEVAR(int *, 0x8B89BC);
		g_mouse_states = CONVERT_GAMEVAR(int *, 0x957EE4);
		g_is_ingame = CONVERT_GAMEVAR(int *, 0xA15190);

		g_btl_buffers = CONVERT_GAMEVAR(char *, 0x241230);
		g_btl_buffer_index = CONVERT_GAMEVAR(int *, 0x24107C);
		g_btl_round_count = CONVERT_GAMEVAR(int *, 0x245698);
		g_btl_player_pos = CONVERT_GAMEVAR(int *, 0x245710);
		g_btl_skill_allowbit = CONVERT_GAMEVAR(int *, 0x2456A4);
		g_btl_player_status = CONVERT_GAMEVAR(int *, 0x2456C0);
		g_btl_petid = CONVERT_GAMEVAR(int *, 0x24563C);
		g_btl_action_done = CONVERT_GAMEVAR(int *, 0x2455A0);
		g_btl_skill_performed = CONVERT_GAMEVAR(int *, 0x245634);
		g_btl_round_endtick = CONVERT_GAMEVAR(unsigned int *, 0x2455A8);

		g_net_buffer = CONVERT_GAMEVAR(char **, 0xCAF90C);
		g_net_socket = CONVERT_GAMEVAR(int *, 0xA10820);

		net_header_common = CONVERT_GAMEVAR(char *, 0xCAEFC0);
		net_header_walk = CONVERT_GAMEVAR(char *, 0xCAF6D4);
		net_header_battle = CONVERT_GAMEVAR(char *, 0x1C6AE4);
		net_header_logback = CONVERT_GAMEVAR(char *, 0x1CB9DC);
		net_header_useitem = CONVERT_GAMEVAR(char *, 0x1CDED8);
		net_header_dropitem = CONVERT_GAMEVAR(char *, 0x1CB908);
		net_header_sell = CONVERT_GAMEVAR(char *, 0x1CB890);
		net_header_sayword = CONVERT_GAMEVAR(char *, 0x1D00E8);
		net_header_secondary = CONVERT_GAMEVAR(char *, 0x1C690A);

		g_item_base = CONVERT_GAMEVAR(item_t *, 0x8D3EE8);

		g_logback = CONVERT_GAMEVAR(int *, 0x957160);
		g_item_cur_using = CONVERT_GAMEVAR(int *, 0x8E3ACC);
		g_item_menu_players = CONVERT_GAMEVAR(item_menu_player_t *, 0x94C2F8);
		g_item_menu_player_count = CONVERT_GAMEVAR(int *, 0x8E3C38);
		g_item_menu_units = CONVERT_GAMEVAR(item_menu_unit_t *, 0x946870);
		g_item_menu_unit_count = CONVERT_GAMEVAR(int *, 0x94BAC8);
		g_player_menu_clickable = CONVERT_GAMEVAR(char *, 0x8C3909);
		g_unit_menu_clickable = CONVERT_GAMEVAR(char *, 0x8B9187);

		g_unit_count = CONVERT_GAMEVAR(int *, 0x245774);
		g_map_units = CONVERT_GAMEVAR(map_unit_t *, 0x245B34);
		g_npc_dialog_show = CONVERT_GAMEVAR(int *, 0x90162C);
		g_npc_dialog_type = CONVERT_GAMEVAR(int *, 0x2130FC);
		g_npc_dialog_option = CONVERT_GAMEVAR(int *, 0x95520C);
		g_npc_dialog_dlgid = CONVERT_GAMEVAR(int *, 0x8E462C);
		g_npc_dialog_npcid = CONVERT_GAMEVAR(int *, 0x8E42CC);

		g_work_basedelay = CONVERT_GAMEVAR(int *, 0);//TODO

		g_map_x_bottom = CONVERT_GAMEVAR(short *, 0x81EF04);
		g_map_y_bottom = CONVERT_GAMEVAR(short *, 0x81EF00);
		g_map_x_size = CONVERT_GAMEVAR(short *, 0x81EF14);
		g_map_y_size = CONVERT_GAMEVAR(short *, 0x806164);
		g_map_collision_table = CONVERT_GAMEVAR(short *, 0x5D6178);

		g_mutex = CONVERT_GAMEVAR(HANDLE *, 0x5D5D1C);

		Sys_CheckModify = CONVERT_GAMEVAR(char(__cdecl *)(const char *), 0x1448B0);
		COMMON_PlaySound = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int), 0x136640);
		BATTLE_PlayerAction = CONVERT_GAMEVAR(void(__cdecl *)(), 0xCD0E0);
		NET_ParseBattlePackets = CONVERT_GAMEVAR(void(__cdecl *)(int, const char *), 0x11E710);
		NET_ParseWorkingResult = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, const char *), 0);//TODO
		R_DrawText = CONVERT_GAMEVAR(void(__cdecl *)(int), 0x6C230);
		g_pGameBase = CONVERT_GAMEVAR(void *, 0x2C1794);
		NET_WritePacket = CONVERT_GAMEVAR(void(__cdecl *)(void *, int, const char *, const char *), 0x50970);
		NET_WritePacket1 = CONVERT_GAMEVAR(void(__cdecl *)(void *, int, const char *, int, int, int), 0x6AD80);
		NET_WritePacket2 = CONVERT_GAMEVAR(void(__cdecl *)(void *, int, const char *, int, int, int, int), 0x89AD0);
		NET_WritePacket3 = CONVERT_GAMEVAR(void(__cdecl *)(void *, int, const char *, int, int, int, int, int, const char *), 0x89A50);
		NET_WriteSayWords = CONVERT_GAMEVAR(void(__cdecl *)(void *, int, const char *, int, int, const char *, int, int, int), 0x118D50);
		NET_WriteLogbackPacket_cgse = CONVERT_GAMEVAR(void(__cdecl *)(void *, int, const char *), 0x6ACD0);
		Move_Player = CONVERT_GAMEVAR(void(__cdecl *)(), 0x89070);
		CL_MoveItemEx = CONVERT_GAMEVAR(void(__cdecl *)(int), 0);//TODO
		UI_ClickLogout = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0x40E0);
		UI_PlayerMenuSelect = CONVERT_GAMEVAR(int(__cdecl **)(int, int), 0);//TODO
		UI_UnitMenuSelect = CONVERT_GAMEVAR(int(__cdecl **)(int, int), 0);//TODO
		UI_PlayerMenuShowup = CONVERT_GAMEVAR(int(__cdecl *)(int), 0x1A190);
		UI_PlayerMenuMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0);//TODO
		UI_UnitMenuShowup = CONVERT_GAMEVAR(int(__cdecl *)(int), 0x8B5F0);
		UI_UnitMenuMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0x8B7C0);
		NPC_ShowDialogInternal = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int, const char *), 0xED060);
		NPC_ClickDialog = CONVERT_GAMEVAR(int(__cdecl *)(int, int, int, char), 0x28A0);
		NPC_ClickExchangeDialog = CONVERT_GAMEVAR(void(__cdecl *)(int), 0);//TODO
		UI_OpenGatherDialog = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0);//TODO
	}
	else if (m_game_type == cg_item_6000)
	{
		g_world_status_cgitem = CONVERT_GAMEVAR(int *, 0xE1E000);//ok
		g_game_status_cgitem = CONVERT_GAMEVAR(int *, 0xE1DFFC);//ok
		g_player_xpos = CONVERT_GAMEVAR(CXorValue *, 0x8AB384);//ok
		g_player_ypos = CONVERT_GAMEVAR(CXorValue *, 0x8AB3B4);//ok
		g_playerBase_cgitem = CONVERT_GAMEVAR(playerbase_t *, 0xE12C30);//ok
		g_playerBase = &g_playerBase_cgitem;
		g_bank_item_base = CONVERT_GAMEVAR(bank_item_info_t *, 0x8E4C38);//ok
		g_player_name = CONVERT_GAMEVAR(char *, 0xBDB998);//ok
		g_pet_base = CONVERT_GAMEVAR(pet_t *, 0xE109F8);//ok
		g_pet_id = CONVERT_GAMEVAR(int *, 0x8B4164);//ok
		g_job_name = CONVERT_GAMEVAR(char *, 0xC1CD48);//ok
		g_skill_base = CONVERT_GAMEVAR(skill_t *, 0xC1CD64);//ok
		g_map_x = CONVERT_GAMEVAR(short *, 0x717AE0);//ok
		g_map_y = CONVERT_GAMEVAR(short *, 0x8367C8);//ok
		
		g_map_name = CONVERT_GAMEVAR(char *, 0x836894);//ok
		g_move_xdest = CONVERT_GAMEVAR(int *, 0x8B3968);//ok
		g_move_ydest = CONVERT_GAMEVAR(int *, 0x8B3960);//ok
		g_move_destx = CONVERT_GAMEVAR(CXorValue *, 0x8AB3D4);//ok
		g_move_desty = CONVERT_GAMEVAR(CXorValue *, 0x8AB3C4);//ok
		g_move_xspeed = CONVERT_GAMEVAR(float *, 0x836890);//ok
		g_move_yspeed = CONVERT_GAMEVAR(float *, 0x819EC4);//ok
		g_map_xf = CONVERT_GAMEVAR(float *, 0x5D1134);//ok
		g_map_yf = CONVERT_GAMEVAR(float *, 0x81B354);//ok
		g_map_xf2 = CONVERT_GAMEVAR(float *, 0x819EE0);//ok
		g_map_yf2 = CONVERT_GAMEVAR(float *, 0x5D6870);//ok
		g_map_xpos = CONVERT_GAMEVAR(CXorValue *, 0x8AB324);//ok
		g_map_ypos = CONVERT_GAMEVAR(CXorValue *, 0x8AB344);//ok
		g_move_walking = CONVERT_GAMEVAR(int *, 0x8AB2BC);//ok
		g_move_turning = CONVERT_GAMEVAR(int *, 0x8B3994);//ok
		g_mouse_states = CONVERT_GAMEVAR(int *, 0xB1E7CC);//ok
		g_is_ingame = CONVERT_GAMEVAR(int *, 0xBDBA78);//ok

		g_btl_buffers = CONVERT_GAMEVAR(char *, 0x23B830);//ok
		g_btl_buffer_index = CONVERT_GAMEVAR(int *, 0x23B828);//ok
		g_btl_round_count = CONVERT_GAMEVAR(int *, 0x23FA1C);//ok
		g_btl_player_pos = CONVERT_GAMEVAR(int *, 0x23FA94);//ok
		g_btl_skill_allowbit = CONVERT_GAMEVAR(int *, 0x23FA28);//ok
		g_btl_player_status = CONVERT_GAMEVAR(int *, 0x23FA44);//ok
		g_btl_petid = CONVERT_GAMEVAR(int *, 0x23F9C0);//ok
		g_btl_action_done = CONVERT_GAMEVAR(int *, 0x23F924);//ok
		g_btl_skill_performed = CONVERT_GAMEVAR(int *, 0x23F9F8);//ok
		g_btl_round_endtick = CONVERT_GAMEVAR(unsigned int *, 0x23F92C);//ok

		g_net_socket = CONVERT_GAMEVAR(int *, 0xBD7108);//ok

		g_item_base = CONVERT_GAMEVAR(item_t *, 0x8C70B0);//ok
		g_item_info_base = CONVERT_GAMEVAR(item_info_t *, 0xE12DCC);//ok

		g_logback = CONVERT_GAMEVAR(int *, 0xB12AA4);//ok
		g_item_cur_using = CONVERT_GAMEVAR(int *, 0x8CEFFC);//ok
		g_item_menu_players = CONVERT_GAMEVAR(item_menu_player_t *, 0x92F638);//ok
		g_item_menu_player_count = CONVERT_GAMEVAR(int *, 0x8CF00C);//ok
		g_item_menu_units = CONVERT_GAMEVAR(item_menu_unit_t *, 0x929BA0);//ok
		g_item_menu_unit_count = CONVERT_GAMEVAR(int *, 0x92EE00);//ok
		g_player_menu_clickable = CONVERT_GAMEVAR(char *, 0x8BE8C1);//ok
		g_unit_menu_clickable = CONVERT_GAMEVAR(char *, 0x8B415F);//ok

		g_unit_menu_base = CONVERT_GAMEVAR(unit_menu_t **, 0xD38E60 - 0x400000);//ok
		g_unit_menu_count = CONVERT_GAMEVAR(int *, 0xCB4140 - 0x400000);//ok
		g_unit_menu_begin = CONVERT_GAMEVAR(int *, 0xCE4C34 - 0x400000);//ok
		g_unit_count = CONVERT_GAMEVAR(int *, 0x23FB0C);//ok

		g_player_menu_name = CONVERT_GAMEVAR(char ***, 0xD29734 - 0x400000);//ok
		g_player_menu_color = CONVERT_GAMEVAR(char **, 0xCE4AA0 - 0x400000);//ok
		g_player_menu_count = CONVERT_GAMEVAR(int *, 0xCE4C28 - 0x400000);//ok
		g_player_menu_begin = CONVERT_GAMEVAR(int *, 0xCDBB24 - 0x400000);//ok
		g_map_units = CONVERT_GAMEVAR(map_unit_t *, 0x240008);//ok

		g_npc_dialog_show = CONVERT_GAMEVAR(int *, 0x8E4A98);//ok
		g_npc_dialog_type = CONVERT_GAMEVAR(int *, 0x2277FC);//ok
		g_npc_dialog_option = CONVERT_GAMEVAR(int *, 0x9386E4);//ok
		g_npc_dialog_dlgid = CONVERT_GAMEVAR(int *, 0x8CF9F0);//ok
		g_npc_dialog_npcid = CONVERT_GAMEVAR(int *, 0x8CF6A0);//ok

		g_work_basedelay = CONVERT_GAMEVAR(int *, 0x8B4190);//ok

		g_map_x_bottom = CONVERT_GAMEVAR(short *, 0x819ED4);//ok
		g_map_y_bottom = CONVERT_GAMEVAR(short *, 0x819ED0);//ok
		g_map_x_size = CONVERT_GAMEVAR(short *, 0x801134);//ok
		g_map_y_size = CONVERT_GAMEVAR(short *, 0x819EE4);//ok
		g_map_collision_table = CONVERT_GAMEVAR(short *, 0x5D1148);//ok
		
		g_healing_skill_index = CONVERT_GAMEVAR(int *, 0x8E481C);//ok
		g_healing_subskill_index = CONVERT_GAMEVAR(int *, 0x8E3C44);//ok
		g_ui_craftdialog_additem_count = CONVERT_GAMEVAR(char *, 0xF12880-0x400000);//ok

		g_craft_step = CONVERT_GAMEVAR(int *, 0xCBF15E - 0x400000);//ok
		g_work_accelerate = CONVERT_GAMEVAR(char *, 0x63FFFE - 0x400000);//ok;
		g_work_accelerate_percent = CONVERT_GAMEVAR(char *, 0x627E8C - 0x400000);//ok;
		g_mutex = CONVERT_GAMEVAR(HANDLE *, 0x5D0D00);//ok

		Sys_CheckModify = CONVERT_GAMEVAR(char(__cdecl *)(const char *), 0x1BD030);//ok
		COMMON_PlaySound = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int), 0x1B1570);//ok
		BATTLE_PlayerAction = CONVERT_GAMEVAR(void(__cdecl *)(), 0xD83A0);//ok
		NET_ParseBattlePackets = CONVERT_GAMEVAR(void(__cdecl *)(int, const char *), 0x1822A0);//ok
		NET_ParseWorkingResult = CONVERT_GAMEVAR(void(__cdecl *)(int , int , int , const char *), 0x181140);//ok
		NET_ParseChatMsg = CONVERT_GAMEVAR(void(__cdecl *)(int, int, const char *, int, int), 0x1814F0);//ok
		R_DrawText = CONVERT_GAMEVAR(void(__cdecl *)(int), 0x79490);//ok
		g_pGameBase = CONVERT_GAMEVAR(void *, 0x2BBA7C);//ok
		NET_WritePacket3_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int, int, int, const char *), 0x189530);//ok
		NET_WriteSayWords_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, const char *, int , int , int ), 0x188710);//ok
		NET_WriteLogbackPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int), 0x189D50);//ok
		NET_WriteUseItemPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int, int), 0x187730);//ok
		NET_WriteBattlePacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, const char *), 0x189CB0);//ok
		NET_WriteDropItemPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int), 0x187950);//ok
		NET_WriteMovePacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, const char *), 0x181240);//ok
		NET_WriteWorkPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int, const char *), 0x188E50);//ok

		Move_Player = CONVERT_GAMEVAR(void(__cdecl *)(), 0x98280);//ok
		UI_ClickLogout = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0xD2CD0);//ok
		CL_MoveItemEx = CONVERT_GAMEVAR(void(__cdecl *)(int), 0xC0EC0);//ok
		UI_PlayerMenuSelect = CONVERT_GAMEVAR(int(__cdecl **)(int, int), 0xCB4144-0x400000);//ok
		UI_UnitMenuSelect = CONVERT_GAMEVAR(int(__cdecl **)(int, int), 0xCCF014-0x400000);//ok
		UI_PlayerMenuShowup = CONVERT_GAMEVAR(int(__cdecl *)(int), 0xE0FE0);//ok
		UI_UnitMenuShowup = CONVERT_GAMEVAR(int(__cdecl *)(int), 0xE18F0);//ok
		UI_PlayerMenuMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xE17A0);//ok
		UI_UnitMenuMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xE1E40);//ok
		NPC_ShowDialogInternal = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int, const char *), 0xDE390);//ok
		NPC_ClickDialog = CONVERT_GAMEVAR(int(__cdecl *)(int, int, int, char), 0xDEC00);//ok
		NPC_ClickExchangeDialog = CONVERT_GAMEVAR(void(__cdecl *)(int), 0xE2880);//ok
		UI_OpenGatherDialog = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0xBDB30);//ok
		UI_OpenCraftDialog = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int), 0xB5E10);//ok
		UI_OpenAssessDialog = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0xB5FF0);//ok
		UI_HandleMiniDialogMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xE22E0);//ok
		UI_IsMouseInRect = CONVERT_GAMEVAR(int(__cdecl *)(int, int, int, int, int), 0x174160);//ok
		UI_HandleLearnSkillConfirmMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0x111F20);//ok
		UI_HandleEnablePlayerFlagsMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xA9320);//ok
		UI_HandleCraftItemSlotMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xBACA0);//ok
		UI_HandleCraftItemButtonMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xB9BC0);//ok
		NET_WriteOpenHealDialog_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0x188AA0);//ok
	}

	m_hFont = CreateFontW(16, 0, 0, 0, FW_THIN, false, false, false,
		CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, L"ËÎÌו");

	m_btl_showhpmp_enable = false;
	m_btl_highspeed_enable = false;
	m_menu_item_selected = 0;
	m_menu_item_select_index = -1;

	for (int i = 0; i < 20; ++i)
		m_battle_units[i].exist = false;

	if (IsInGame() && GetWorldStatus() == 10) {
		char *battle_buffer = g_btl_buffers + 4096 * ((*g_btl_buffer_index == 0) ? 3 : (*g_btl_buffer_index - 1));
		ParseBattleUnits(battle_buffer, strlen(battle_buffer));
	}

	m_move_to = 0;
	m_move_to_x = 0;
	m_move_to_y = 0;
	m_move_speed = 100;
	m_ui_minidialog_loop = false;
	m_ui_minidialog_loop_index = -1;
	m_ui_minidialog_click_index = -1;
	m_ui_learn_skill_confirm = -1;
	m_desired_player_enable_flags = 0;
	m_change_player_enable_flags = 0;
	m_ui_craftdialog_click_begin = false;
	m_ui_craftdialog_loop = false;
	m_ui_craftdialog_loop_index = -1;
	m_ui_craftdialog_click_index = -1;
	for (int i = 0; i < 5; ++i)
		m_ui_craftdialog_additem[i] = -1;
	m_ui_craftdialog_additemcount = 0;
	m_work_acceleration = 100;

	DetourTransactionBegin();
	DetourAttach(&(void *&)BATTLE_PlayerAction, ::NewBATTLE_PlayerAction);
	DetourAttach(&(void *&)NET_ParseBattlePackets, ::NewNET_ParseBattlePackets);
	DetourAttach(&(void *&)NET_ParseWorkingResult, ::NewNET_ParseWorkingResult);
	DetourAttach(&(void *&)NET_ParseChatMsg, ::NewNET_ParseChatMsg);
	DetourAttach(&(void *&)R_DrawText, NewR_DrawText);
	DetourAttach(&(void *&)Move_Player, ::NewMove_Player);
	DetourAttach(&(void *&)UI_PlayerMenuShowup, ::NewUI_PlayerMenuShowup);
	DetourAttach(&(void *&)UI_PlayerMenuMouseEvent, ::NewUI_PlayerMenuMouseEvent);
	DetourAttach(&(void *&)UI_UnitMenuShowup, ::NewUI_UnitMenuShowup);
	DetourAttach(&(void *&)UI_UnitMenuMouseEvent, ::NewUI_UnitMenuMouseEvent);
	DetourAttach(&(void *&)NPC_ShowDialogInternal, ::NewNPC_ShowDialogInternal);
	DetourAttach(&(void *&)Sys_CheckModify, ::NewSys_CheckModify);
	DetourAttach(&(void *&)UI_HandleMiniDialogMouseEvent, ::NewUI_HandleMiniDialogMouseEvent);
	DetourAttach(&(void *&)UI_IsMouseInRect, ::NewUI_IsMouseInRect);
	DetourAttach(&(void *&)UI_HandleLearnSkillConfirmMouseEvent, ::NewUI_HandleLearnSkillConfirmMouseEvent);
	DetourAttach(&(void *&)UI_HandleEnablePlayerFlagsMouseEvent, ::NewUI_HandleEnablePlayerFlagsMouseEvent);
	DetourAttach(&(void *&)UI_HandleCraftItemSlotMouseEvent, ::NewUI_HandleCraftItemSlotMouseEvent);
	DetourAttach(&(void *&)UI_HandleCraftItemButtonMouseEvent, ::NewUI_HandleCraftItemButtonMouseEvent);
	DetourTransactionCommit();

	if (*g_mutex)
	{
		CloseHandle(*g_mutex);
		*g_mutex = NULL;
	}

	m_initialized = true;
}

void CGAService::Uninitialize()
{
	if (!m_initialized)
		return;

	m_initialized = false;
	DetourTransactionBegin();
	DetourDetach(&(void *&)BATTLE_PlayerAction, ::NewBATTLE_PlayerAction);
	DetourDetach(&(void *&)NET_ParseBattlePackets, ::NewNET_ParseBattlePackets);
	DetourDetach(&(void *&)NET_ParseWorkingResult, ::NewNET_ParseWorkingResult);
	DetourDetach(&(void *&)NET_ParseChatMsg, ::NewNET_ParseChatMsg);
	DetourDetach(&(void *&)R_DrawText, NewR_DrawText);
	DetourDetach(&(void *&)Move_Player, ::NewMove_Player);
	DetourDetach(&(void *&)UI_PlayerMenuShowup, ::NewUI_PlayerMenuShowup);
	DetourDetach(&(void *&)UI_PlayerMenuMouseEvent, ::NewUI_PlayerMenuMouseEvent);
	DetourDetach(&(void *&)UI_UnitMenuShowup, ::NewUI_UnitMenuShowup);
	DetourDetach(&(void *&)UI_UnitMenuMouseEvent, ::NewUI_UnitMenuMouseEvent);
	DetourDetach(&(void *&)NPC_ShowDialogInternal, ::NewNPC_ShowDialogInternal);
	DetourDetach(&(void *&)Sys_CheckModify, ::NewSys_CheckModify);
	DetourDetach(&(void *&)UI_HandleMiniDialogMouseEvent, ::NewUI_HandleMiniDialogMouseEvent);
	DetourDetach(&(void *&)UI_IsMouseInRect, ::NewUI_IsMouseInRect);
	DetourDetach(&(void *&)UI_HandleLearnSkillConfirmMouseEvent, ::NewUI_HandleLearnSkillConfirmMouseEvent);
	DetourDetach(&(void *&)UI_HandleEnablePlayerFlagsMouseEvent, ::NewUI_HandleEnablePlayerFlagsMouseEvent);
	DetourDetach(&(void *&)UI_HandleCraftItemSlotMouseEvent, ::NewUI_HandleCraftItemSlotMouseEvent);
	DetourDetach(&(void *&)UI_HandleCraftItemButtonMouseEvent, ::NewUI_HandleCraftItemButtonMouseEvent);
	DetourTransactionCommit();

	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

const int s_UnitPosToXY[][2] =
{
	{ 480, 370 },{ 480 + 60, 370 - 50 },{ 480 - 60, 370 + 50 },{ 480 + 120, 370 - 100 },{ 480 - 120, 370 + 100 },
	{ 420, 327 },{ 420 + 60, 327 - 50 },{ 420 - 60, 327 + 50 },{ 420 + 120, 327 - 100 },{ 420 - 120, 317 + 100 },
	{ 158, 188 },{ 158 + 60, 188 - 50 },{ 158 - 60, 188 + 50 },{ 158 + 120, 158 - 100 },{ 158 - 120, 158 + 100 },
	{ 210, 240 },{ 210 + 60, 240 - 50 },{ 210 - 60, 240 + 50 },{ 210 + 120, 240 - 100 },{ 210 - 120, 240 + 100 },
};

void CGAService::DrawBattleInfo()
{
	if (!m_btl_showhpmp_enable)
		return;

	LPDIRECTDRAWSURFACE pSurface = GetDirectDrawBackSurface();

	if (GetWorldStatus() == 10)
	{
		HDC hDC;
		if (S_OK == pSurface->GetDC(&hDC))
		{
			SetBkMode(hDC, TRANSPARENT);
			SelectObject(hDC, g_CGAService.m_hFont);

			int x, y;
			char buf[64];
			for (int i = 0; i < 20; ++i)
			{
				if (m_battle_units[i].exist)
				{
					x = s_UnitPosToXY[i][0];
					y = s_UnitPosToXY[i][1];

					sprintf(buf, "%d / %d", m_battle_units[i].curhp, m_battle_units[i].maxhp);
					int len = strlen(buf);

					ABC abc[1];
					int width = 0;
					for (int j = 0; j < len; ++j) {
						GetCharABCWidthsA(hDC, buf[j], buf[j], abc);
						width += abc[0].abcA + abc[0].abcB + abc[0].abcC;
					}
					SetTextColor(hDC, RGB(0, 0, 0));
					TextOutA(hDC, x - width / 2, y, buf, len);
					SetTextColor(hDC, RGB(255, 200, 0));
					TextOutA(hDC, x - width / 2 - 1, y - 1, buf, len);

					sprintf(buf, "%d / %d", m_battle_units[i].curmp, m_battle_units[i].maxmp);
					len = strlen(buf);

					width = 0;
					for (int j = 0; j < len; ++j) {
						GetCharABCWidthsA(hDC, buf[j], buf[j], abc);
						width += abc[0].abcA + abc[0].abcB + abc[0].abcC;
					}
					SetTextColor(hDC, RGB(0, 0, 0));
					TextOutA(hDC, x - width / 2, y + 20, buf, len);
					SetTextColor(hDC, RGB(0, 200, 255));
					TextOutA(hDC, x - width / 2 - 1, y - 1 + 20, buf, len);
				}
			}

			pSurface->ReleaseDC(hDC);
		}
	}
}

void CGAService::ParseBattleUnits(const char *buf, size_t len)
{
	if (len < 4)
		return;

	char *p = (char *)buf + 4;
	int unitdata[11];
	char unitname[32];
	char buf2[256];

	for (int i = 0; i < 20; ++i)
		m_battle_units[i].exist = false;

	while (*p)
	{
		int parsed = sscanf(p, "%x|%[^|]|%x|%x|%x|%x|%x|%x|%x|%x|%x|%x|",
			&unitdata[0], unitname, &unitdata[1], &unitdata[2],
			&unitdata[3], &unitdata[4], &unitdata[5], &unitdata[6],
			&unitdata[7], &unitdata[8], &unitdata[9], &unitdata[10]);
		if (parsed < 12)
			break;

		sprintf(buf2, "%x|%s|%x|%x|%x|%x|%x|%x|%x|%x|%x|%x|",
			unitdata[0], unitname, unitdata[1], unitdata[2],
			unitdata[3], unitdata[4], unitdata[5], unitdata[6],
			unitdata[7], unitdata[8], unitdata[9], unitdata[10]);
		p += strlen(buf2);

		int pos = unitdata[0];

		if (pos < 0 || pos > 19)
			break;

		m_battle_units[pos].modelid = unitdata[1];
		m_battle_units[pos].level = unitdata[3];
		m_battle_units[pos].curhp = unitdata[4];
		m_battle_units[pos].maxhp = unitdata[5];
		m_battle_units[pos].curmp = unitdata[6];
		m_battle_units[pos].maxmp = unitdata[7];
		m_battle_units[pos].pos = pos;
		strcpy(m_battle_units[pos].name, unitname);
		m_battle_units[pos].exist = true;
	}
}

IDirectDraw *CGAService::GetDirectDraw()
{
	return *(IDirectDraw **)(*(char **)g_pGameBase + 4);
}

IDirectDrawSurface *CGAService::GetDirectDrawBackSurface()
{
	return *(IDirectDrawSurface **)(*(char **)g_pGameBase + 12);
}

CGAService::CGAService()
{
	m_initialized = false;
}

void CGAService::InitializeGameData(cga_game_data_t data)
{

}

bool CGAService::Connect()
{
	return true;
}

int CGAService::IsInGame()
{
	return *g_is_ingame;
}

int CGAService::GetWorldStatus()
{
	if (!IsInGame())
		return 0;

	if (m_game_type == cg_item_6000)
		return *g_world_status_cgitem;

	return g_world_status->decode();
}

int CGAService::GetGameStatus()
{
	if (!IsInGame())
		return 0;

	if (*g_logback)
		return 200;

	if (m_game_type == cg_item_6000)
		return *g_game_status_cgitem;

	return g_game_status->decode();
}

void CGAService::WM_GetPlayerInfo(cga_player_info_t *info)
{
	if (!IsInGame())
		return;

	info->hp = (*g_playerBase)->hp.decode();
	info->maxhp = (*g_playerBase)->maxhp.decode();
	info->mp = (*g_playerBase)->mp.decode();
	info->maxmp = (*g_playerBase)->maxmp.decode();
	info->xp = (*g_playerBase)->xp.decode();
	info->maxxp = (*g_playerBase)->maxxp.decode();
	info->health = (*g_playerBase)->health;
	info->level = (*g_playerBase)->level;
	info->gold = (*g_playerBase)->gold;
	info->unitid = (*g_playerBase)->unitid;
	info->punchclock = (*g_playerBase)->punchclock;
	info->usingpunchclock = (*g_playerBase)->using_punchclock;
	info->petid = *g_pet_id;
	info->name = boost::locale::conv::to_utf<char>(g_player_name, "GBK");
	info->job = boost::locale::conv::to_utf<char>(g_job_name, "GBK");
}

cga_player_info_t CGAService::GetPlayerInfo()
{
	cga_player_info_t info;
	SendMessageA(g_MainHwnd, WM_CGA_GET_PLAYER_INFO, (WPARAM)&info, 0);
	return info;
}

void CGAService::SetPlayerFlagEnabled(int index, bool enable)
{
	if (index >= 0 && index < _ARRAYSIZE(index2player_flags)) {
		m_change_player_enable_flags |= index2player_flags[index];
		if(enable)
			m_desired_player_enable_flags |= index2player_flags[index];
		else
			m_desired_player_enable_flags &= ~index2player_flags[index];
	}
}

bool CGAService::IsPlayerFlagEnabled(int index)
{
	if (!IsInGame())
		return false;

	if (index >= 0 && index < _ARRAYSIZE(index2player_flags)) {
		return ((*g_playerBase)->enable_flags & index2player_flags[index]) ? true : false;
	}
	return false;
}

bool CGAService::IsSkillValid(int skillid)
{
	if (skillid < 0 || skillid > 9)
		return false;
	return g_skill_base[skillid].valid ? true : false;
}

void CGAService::WM_GetSkillInfo(int index, cga_skill_info_t *info)
{
	if (!IsInGame())
		return;

	if (IsSkillValid(index))
	{
		info->name = boost::locale::conv::to_utf<char>(g_skill_base[index].name, "GBK");
		info->lv = g_skill_base[index].level;
		info->maxlv = g_skill_base[index].maxlevel;
		info->xp = g_skill_base[index].xp;
		info->maxxp = g_skill_base[index].maxxp;
		info->type = g_skill_base[index].type;
		info->pos = g_skill_base[index].pos;
		info->skill_id = g_skill_base[index].skill_id;
		info->index = index;
	}
}

cga_skill_info_t CGAService::GetSkillInfo(int index)
{
	cga_skill_info_t info;
	SendMessageA(g_MainHwnd, WM_CGA_GET_SKILL_INFO, index, (LPARAM)&info);
	return info;
}

void CGAService::WM_GetSkillsInfo(cga_skills_info_t *info)
{
	if (!IsInGame())
		return;

	for (int i = 0; i < 10; ++i)
	{
		if (IsSkillValid(i))
		{
			info->emplace_back(
				boost::locale::conv::to_utf<char>(g_skill_base[i].name, "GBK"),
				g_skill_base[i].level,
				g_skill_base[i].maxlevel,
				g_skill_base[i].xp,
				g_skill_base[i].maxxp,
				g_skill_base[i].skill_id,
				g_skill_base[i].type,
				g_skill_base[i].pos,
				i
			);
		}
	}
}


cga_skills_info_t CGAService::GetSkillsInfo()
{
	cga_skills_info_t info;
	SendMessageA(g_MainHwnd, WM_CGA_GET_SKILLS_INFO, (WPARAM)&info, 0);
	return info;
}

void CGAService::WM_GetSubSkillInfo(int index, int stage, cga_subskill_info_t *info)
{
	if (!IsInGame())
		return;

	if (IsSkillValid(index) && stage >= 0 && stage <= g_skill_base[index].level - 1 && g_skill_base[index].sub[stage].name[0])
	{
		info->name = boost::locale::conv::to_utf<char>(g_skill_base[index].sub[stage].name, "GBK");
		info->info = boost::locale::conv::to_utf<char>(g_skill_base[index].sub[stage].info, "GBK");
		info->cost = g_skill_base[index].sub[stage].cost;
		info->level = g_skill_base[index].sub[stage].level;
		info->flags = g_skill_base[index].sub[stage].flags;
	}
}

cga_subskill_info_t CGAService::GetSubSkillInfo(int index, int stage)
{
	cga_subskill_info_t info;

	SendMessageA(g_MainHwnd, WM_CGA_GET_SUBSKILL_INFO, (index & 0xFF) | (stage << 8), (LPARAM)&info);
	
	return info;
}

void CGAService::WM_GetSubSkillsInfo(int index, cga_subskills_info_t *info)
{
	if (!IsInGame())
		return;

	if (IsSkillValid(index))
	{
		for (int i = 0; i < g_skill_base[index].level; ++i)
		{
			if (g_skill_base[index].sub[i].name[0])
			{
				info->emplace_back(
					boost::locale::conv::to_utf<char>(g_skill_base[index].sub[i].name, "GBK"),
					boost::locale::conv::to_utf<char>(g_skill_base[index].sub[i].info, "GBK"),
					g_skill_base[index].sub[i].cost,
					g_skill_base[index].sub[i].flags,
					g_skill_base[index].sub[i].level
				);
			}
		}
	}
}

cga_subskills_info_t CGAService::GetSubSkillsInfo(int index)
{
	cga_subskills_info_t info;

	SendMessageA(g_MainHwnd, WM_CGA_GET_SUBSKILLS_INFO, index, (LPARAM)&info);

	return info;
}

bool CGAService::IsPetValid(int petid)
{
	if (petid < 0 || petid > 4)
		return false;
	return (g_pet_base[petid].level > 0);
}

void CGAService::WM_GetPetInfo(int index, cga_pet_info_t *info)
{
	if (!IsInGame())
		return;

	if (IsPetValid(index))
	{
		info->hp = g_pet_base[index].hp.decode();
		info->maxhp = g_pet_base[index].maxhp.decode();
		info->mp = g_pet_base[index].mp.decode();
		info->maxmp = g_pet_base[index].maxmp.decode();
		info->xp = g_pet_base[index].xp.decode();
		info->maxxp = g_pet_base[index].maxxp.decode();
		info->health = g_pet_base[index].health;
		info->level = g_pet_base[index].level;
		info->flags = g_pet_base[index].flags;
		info->race = g_pet_base[index].race;
		info->battle_flags = g_pet_base[index].battle_flags;
		info->index = index;
		info->name = boost::locale::conv::to_utf<char>(g_pet_base[index].name, "GBK");
		info->realname = boost::locale::conv::to_utf<char>(g_pet_base[index].realname, "GBK");;
	}
}

cga_pet_info_t CGAService::GetPetInfo(int index)
{
	cga_pet_info_t info;

	SendMessageA(g_MainHwnd, WM_CGA_GET_PET_INFO, index, (LPARAM)&info);

	return info;
}

void CGAService::WM_GetPetsInfo(cga_pets_info_t *info)
{
	if (!IsInGame())
		return;

	for (int i = 0; i < 5; ++i)
	{
		if (IsPetValid(i))
		{
			info->emplace_back(
				g_pet_base[i].hp.decode(),
				g_pet_base[i].maxhp.decode(),
				g_pet_base[i].mp.decode(),
				g_pet_base[i].maxmp.decode(),
				g_pet_base[i].xp.decode(),
				g_pet_base[i].maxxp.decode(),
				g_pet_base[i].health,
				g_pet_base[i].level,
				g_pet_base[i].flags,
				g_pet_base[i].race,
				g_pet_base[i].battle_flags,
				i,
				boost::locale::conv::to_utf<char>(g_pet_base[i].name, "GBK"),
				boost::locale::conv::to_utf<char>(g_pet_base[i].realname, "GBK")
			);
		}
	}
}

cga_pets_info_t CGAService::GetPetsInfo()
{
	cga_pets_info_t info;

	SendMessageA(g_MainHwnd, WM_CGA_GET_PETS_INFO, (WPARAM)&info, 0);

	return info;
}

bool CGAService::IsPetSkillValid(int petid, int skillid)
{
	if (!IsInGame())
		return false;

	if (!IsPetValid(petid))
		return false;

	if (skillid < 0 || skillid > 9)
		return false;

	return g_pet_base[petid].skills[skillid].valid ? true : false;
}

void CGAService::WM_GetPetSkillInfo(int petid, int skillid, cga_pet_skill_info_t *info)
{
	if (IsPetSkillValid(petid, skillid))
	{
		info->name = boost::locale::conv::to_utf<char>(g_pet_base[petid].skills[skillid].name, "GBK");
		info->info = boost::locale::conv::to_utf<char>(g_pet_base[petid].skills[skillid].info, "GBK");
		info->cost = g_pet_base[petid].skills[skillid].cost;
		info->flags = g_pet_base[petid].skills[skillid].flags;
		info->index = skillid;
	}
}

cga_pet_skill_info_t CGAService::GetPetSkillInfo(int petid, int skillid)
{
	cga_pet_skill_info_t info;
	SendMessageA(g_MainHwnd, WM_CGA_GET_PET_SKILL_INFO, (petid & 0xFF) | ((skillid << 8) & 0xFF), (LPARAM)&info);
	return info;
}

void CGAService::WM_GetPetSkillsInfo(int petid, cga_pet_skills_info_t *info)
{
	if (!IsInGame())
		return;

	for (int i = 0; i < 10; ++i)
	{
		if (IsPetSkillValid(petid, i))
		{
			info->emplace_back(
				boost::locale::conv::to_utf<char>(g_pet_base[petid].skills[i].name, "GBK"),
				boost::locale::conv::to_utf<char>(g_pet_base[petid].skills[i].info, "GBK"),
				g_pet_base[petid].skills[i].cost,
				g_pet_base[petid].skills[i].flags,
				i
			);
		}
	}
}

cga_pet_skills_info_t CGAService::GetPetSkillsInfo(int petid)
{
	cga_pet_skills_info_t info;
	SendMessageA(g_MainHwnd, WM_CGA_GET_PET_SKILLS_INFO, petid, (LPARAM)&info);
	return info;
}

bool CGAService::IsItemValid(int itempos)
{
	if (!IsInGame())
		return false;

	if (itempos >= 100 && itempos < 180)
	{
		return g_bank_item_base[itempos - 100].valid ? true : false;
	}

	if (itempos < 0 || itempos >= 100 || (*g_playerBase)->iteminfos[itempos].valid == 0)
		return false;

	return true;
}

void CGAService::WM_GetItemInfo(int itempos, cga_item_info_t *info)
{
	if (!IsInGame())
		return;

	if (IsItemValid(itempos))
	{
		if (itempos >= 100 && itempos < 180)
		{
			itempos -= 100;
			info->name = boost::locale::conv::to_utf<char>(g_bank_item_base[itempos].name, "GBK");
			info->attr = boost::locale::conv::to_utf<char>(g_bank_item_base[itempos].attr, "GBK");
			info->info = boost::locale::conv::to_utf<char>(g_bank_item_base[itempos].info, "GBK");
			info->info2 = boost::locale::conv::to_utf<char>(g_bank_item_base[itempos].info2, "GBK");
			info->itemid = 0;
			info->count = g_bank_item_base[itempos].count;
			info->pos = itempos + 100;
			info->level = 0;
			info->type = 0;
			info->assessed = false;
		}
		else
		{
			info->name = boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].name, "GBK");
			info->attr = boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].attr, "GBK");
			info->attr2 = boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].attr2, "GBK");
			info->info = boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].info, "GBK");
			info->info2 = boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].info2, "GBK");
			info->itemid = (*g_playerBase)->iteminfos[itempos].item_id;
			info->count = (*g_playerBase)->iteminfos[itempos].count;
			info->pos = itempos;
			info->level = (*g_playerBase)->iteminfos[itempos].level;
			info->type = (*g_playerBase)->iteminfos[itempos].type;
			info->assessed = (*g_playerBase)->iteminfos[itempos].assessed ? true : false;
		}
	}
}

cga_item_info_t CGAService::GetItemInfo(int itempos)
{
	cga_item_info_t info;

	SendMessageA(g_MainHwnd, WM_CGA_GET_ITEM_INFO, itempos, (LPARAM)&info);

	return info;
}

void CGAService::WM_GetItemsInfo(cga_items_info_t *info)
{
	if (!IsInGame())
		return;

	int maxitems = 8;
	if (m_game_type == cg_item_6000)
		maxitems += 20;
	else
		maxitems += 40;

	for (int itempos = 0; itempos < maxitems; ++itempos)
	{
		if (IsItemValid(itempos))
		{
			info->emplace_back(
				boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].name, "GBK"),
				boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].attr, "GBK"),
				boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].attr2, "GBK"),
				boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].info, "GBK"),
				boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].info2, "GBK"),
				(*g_playerBase)->iteminfos[itempos].item_id,
				(*g_playerBase)->iteminfos[itempos].count,		
				itempos,
				(*g_playerBase)->iteminfos[itempos].level,
				(*g_playerBase)->iteminfos[itempos].type,
				(*g_playerBase)->iteminfos[itempos].assessed ? true : false
			);
		}
	}
}

cga_items_info_t CGAService::GetItemsInfo()
{
	cga_items_info_t info;

	SendMessageA(g_MainHwnd, WM_CGA_GET_ITEMS_INFO, (WPARAM)&info, 0);

	return info;
}

void CGAService::WM_GetBankItemsInfo(cga_items_info_t *info)
{
	if (!IsInGame())
		return;

	int maxitems = 80;

	for (int itempos = 0; itempos < maxitems; ++itempos)
	{
		if (IsItemValid(100 + itempos))
		{
			info->emplace_back(
				boost::locale::conv::to_utf<char>(g_bank_item_base[itempos].name, "GBK"),
				boost::locale::conv::to_utf<char>(g_bank_item_base[itempos].attr, "GBK"),
				std::string(),
				boost::locale::conv::to_utf<char>(g_bank_item_base[itempos].info, "GBK"),
				boost::locale::conv::to_utf<char>(g_bank_item_base[itempos].info2, "GBK"),
				0,
				g_bank_item_base[itempos].count,
				100 + itempos,
				0,
				0,
				false
			);
		}
	}
}

cga_items_info_t CGAService::GetBankItemsInfo()
{
	cga_items_info_t info;

	SendMessageA(g_MainHwnd, WM_CGA_GET_BANK_ITEMS_INFO, (WPARAM)&info, 0);

	return info;
}

bool CGAService::WM_DropItem(int itempos)
{
	if (!IsInGame())
		return false;

	if (!IsItemValid(itempos))
		return false;

	if (m_game_type == cg_item_6000)
		NET_WriteDropItemPacket_cgitem(*g_net_socket, g_player_xpos->decode(), g_player_ypos->decode(), itempos);
	else
		NET_WritePacket1(*g_net_buffer, *g_net_socket, net_header_dropitem, g_player_xpos->decode(), g_player_ypos->decode(), itempos);

	return true;
}

bool CGAService::DropItem(int itempos)
{
	return SendMessageA(g_MainHwnd, WM_CGA_DROP_ITEM, itempos, 0) ? true : false;
}

bool CGAService::WM_MoveItem(cga_move_item_t *mov)
{
	if (!IsInGame())
		return false;

	if (!IsItemValid(mov->itempos))
		return false;

	char buffer[64];
	sprintf(buffer, "I\\z%d\\z%d\\z%d", mov->itempos, mov->dstpos, mov->count);

	if (m_game_type == cg_item_6000)
		NET_WritePacket3_cgitem(*g_net_socket,
			g_player_xpos->decode(), g_player_ypos->decode(),
			*g_npc_dialog_dlgid, *g_npc_dialog_npcid, 0, buffer);
	//else
		//NET_WritePacket1(*g_net_buffer, *g_net_socket, net_header_dropitem, g_player_xpos->decode(), g_player_ypos->decode(), itempos);

	return true;
}

bool CGAService::MoveItem(int itempos, int dstpos, int count)
{
	cga_move_item_t mov;
	mov.itempos = itempos;
	mov.dstpos = dstpos;
	mov.count = count;
	return SendMessageA(g_MainHwnd, WM_CGA_MOVE_ITEM, (WPARAM)&mov, 0) ? true : false;
}

std::tuple<int, int> CGAService::GetMapXY()
{
	return std::tuple<int, int>(g_player_xpos->decode(), g_player_ypos->decode());
}

std::tuple<float, float> CGAService::GetMapXYFloat()
{
	return std::tuple<float, float>(*g_map_xf, *g_map_yf);
}

std::tuple<float, float> CGAService::GetMoveSpeed()
{
	return std::tuple<float, float>(*g_move_xspeed, *g_move_yspeed);
}

std::string CGAService::GetMapName()
{
	return boost::locale::conv::to_utf<char>(g_map_name, "GBK");
}

cga_map_units_t CGAService::GetMapUnits()
{
	cga_map_units_t units;

	SendMessageA(g_MainHwnd, WM_CGA_GET_MAP_UNITS, (WPARAM)&units, 0);

	return units;
}

void CGAService::WalkTo(int x, int y)
{
	m_move_to |= 1;
	m_move_to_x = x;
	m_move_to_y = y;
}

void CGAService::TurnTo(int x, int y)
{
	m_move_to |= 2;
	m_move_to_x = x;
	m_move_to_y = y;
}

void CGAService::SetMoveSpeed(int speed)
{
	if(speed >= 1 && speed <= 1000)
		m_move_speed = speed;
}

bool CGAService::IsBattleUnitValid(int pos)
{
	return (pos >= 0 && pos < 20 && m_battle_units[pos].exist) ? true : false;
}

void CGAService::WM_GetBattleUnit(int pos, cga_battle_unit_t *u)
{
	if (!IsInGame())
		return;

	if (IsBattleUnitValid(pos))
	{
		u->name = boost::locale::conv::to_utf<char>(m_battle_units[pos].name, "GBK");
		u->modelid = m_battle_units[pos].modelid;
		u->level = m_battle_units[pos].level;
		u->curhp = m_battle_units[pos].curhp;
		u->maxhp = m_battle_units[pos].maxhp;
		u->curmp = m_battle_units[pos].curmp;
		u->maxmp = m_battle_units[pos].maxmp;
		u->pos = m_battle_units[pos].pos;
	}
}

cga_battle_unit_t CGAService::GetBattleUnit(int pos)
{
	cga_battle_unit_t u;
	SendMessageA(g_MainHwnd, WM_CGA_GET_BATTLE_UNIT_INFO, pos, (LPARAM)&u);
	return u;
}

void CGAService::WM_GetBattleUnits(cga_battle_units_t *u)
{
	if (!IsInGame())
		return;

	for (int i = 0; i < 20; ++i)
	{
		if (IsBattleUnitValid(i))
		{
			u->emplace_back(
				boost::locale::conv::to_utf<char>(m_battle_units[i].name, "GBK"),
				m_battle_units[i].modelid,
				m_battle_units[i].level,
				m_battle_units[i].curhp,
				m_battle_units[i].maxhp,
				m_battle_units[i].curmp,
				m_battle_units[i].maxmp,
				m_battle_units[i].pos
			);
		}
	}
}

cga_battle_units_t CGAService::GetBattleUnits()
{
	cga_battle_units_t u;
	SendMessageA(g_MainHwnd, WM_CGA_GET_BATTLE_UNITS_INFO, (WPARAM)&u, 0);
	return u;
}

int CGAService::GetBattleRoundCount()
{
	return *g_btl_round_count;
}

int CGAService::GetBattlePlayerPosition()
{
	return *g_btl_player_pos;
}

int CGAService::GetBattlePlayerStatus()
{
	return *g_btl_player_status;
}

int CGAService::GetBattleSkillPerformed()
{
	return *g_btl_skill_performed;
}

int CGAService::GetBattlePetId()
{
	return *g_btl_petid;
}

bool CGAService::WM_BattleNormalAttack(int target)
{
	if (!IsInGame())
		return false;
	if (*g_btl_player_status != 1)
		return false;
	char buf[32];
	sprintf(buf, "H|%X", target);
	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, buf);
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, buf);
	*g_btl_action_done = 1;
	COMMON_PlaySound(57, 320, 240);
	//Battle_EndSelect();

	return true;
}

bool CGAService::BattleNormalAttack(int target)
{
	return SendMessageA(g_MainHwnd, WM_CGA_BATTLE_NORMALATTACK, target, 0) ? true : false;
}

bool CGAService::WM_BattleSkillAttack(int skillpos, int skilllv, int target)
{
	if (!IsInGame())
		return false;
	if (*g_btl_player_status != 1)
		return false;
	if (!g_skill_base[skillpos].valid)
		return false;
	if (!(*g_btl_skill_allowbit & (1 << skillpos)))//skill not allowed!!!
		return false;
	if (g_skill_base[skillpos].level < skilllv)
		return false;
	if (g_skill_base[skillpos].sub[skilllv - 1].cost > m_battle_units[*g_btl_player_pos].curmp)
		return false;

	char buf[32];
	sprintf(buf, "S|%X|%X|%X", skillpos, skilllv, target);
	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, buf);
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, buf);
	*g_btl_action_done = 1;
	COMMON_PlaySound(57, 320, 240);

	return true;
}

bool CGAService::BattleSkillAttack(int skillpos, int skilllv, int target)
{
	return SendMessageA(g_MainHwnd, WM_CGA_BATTLE_SKILLATTACK, (skillpos & 0xFF) | (skilllv << 8), target) ? true : false;
}

bool CGAService::WM_BattleDefense()
{
	if (!IsInGame())
		return false;
	if (*g_btl_player_status != 1)
		return false;

	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, "G");
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, "G");
	*g_btl_action_done = 1;
	COMMON_PlaySound(56, 320, 240);

	return true;
}

bool CGAService::BattleDefense()
{
	return SendMessageA(g_MainHwnd, WM_CGA_BATTLE_DEFENSE, 0, 0) ? true : false;
}

bool CGAService::WM_BattleEscape()
{
	if (!IsInGame())
		return false;
	if (*g_btl_player_status != 1)
		return false;

	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, "E");
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, "E");
	*g_btl_action_done = 1;
	COMMON_PlaySound(56, 320, 240);

	return false;
}

bool CGAService::BattleEscape()
{
	return SendMessageA(g_MainHwnd, WM_CGA_BATTLE_ESCAPE, 0, 0) ? true : false;
}

bool CGAService::WM_BattleExchangePosition()
{
	if (!IsInGame())
		return false;
	if (*g_btl_player_status != 1)
		return false;

	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, "P");
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, "P");
	*g_btl_action_done = 1;
	COMMON_PlaySound(56, 320, 240);

	return false;
}

bool CGAService::BattleExchangePosition()
{
	return SendMessageA(g_MainHwnd, WM_CGA_BATTLE_EXCHANGE_POSITION, 0, 0) ? true : false;
}

bool CGAService::WM_BattleChangePet(int petid)
{
	if (!IsInGame())
		return false;
	if (*g_btl_player_status != 1)
		return false;

	if (petid < 0 || petid > 4)
		petid = 255;

	char buf[32];
	sprintf(buf, "M|%X", petid);
	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, buf);
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, buf);
	*g_btl_action_done = 1;
	COMMON_PlaySound(56, 320, 240);

	return false;
}

bool CGAService::BattleChangePet(int petid)
{
	return SendMessageA(g_MainHwnd, WM_CGA_BATTLE_CHANGE_PET, petid, 0) ? true : false;
}

bool CGAService::WM_BattleUseItem(int itempos, int target)
{
	if (!IsInGame())
		return false;
	if (*g_btl_player_status != 1 || itempos < 8)
		return false;

	char buf[32];
	sprintf(buf, "I|%X|%X", itempos, target);
	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, buf);
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, buf);
	*g_btl_action_done = 1;
	COMMON_PlaySound(57, 320, 240);

	return true;
}

bool CGAService::BattleUseItem(int itempos, int target)
{
	return SendMessageA(g_MainHwnd, WM_CGA_BATTLE_USEITEM, itempos, target) ? true : false;
}

bool CGAService::WM_BattlePetSkillAttack(int skillpos, int target)
{
	if (!IsInGame())
		return false;
	if (*g_btl_player_status != 4)
		return false;
	//if (g_pet_base[*g_btl_petid].skills[skillpos].cost> m_battle_units[*g_btl_player_pos].curmp)
	//	return false;	
	char buf[32];
	sprintf(buf, "W|%X|%X", skillpos, target);
	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, buf);
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, buf);
	*g_btl_action_done = 1;
	COMMON_PlaySound(57, 320, 240);

	return true;
}

bool CGAService::BattlePetSkillAttack(int skillpos, int target)
{
	return SendMessageA(g_MainHwnd, WM_CGA_BATTLE_PETSKILLATTACK, skillpos, target) ? true : false;
}

void CGAService::BattleSetHighSpeedEnabled(bool enable)
{
	m_btl_highspeed_enable = enable;
}

void CGAService::BattleSetShowHPMPEnabled(bool enable)
{
	m_btl_showhpmp_enable = enable;
}

int CGAService::GetBattleEndTick()
{
	unsigned int currentTick = ((unsigned int)GetTickCount() <= 900000000) ? GetTickCount() : GetTickCount() % 900000000u;

	return (*g_btl_round_endtick - currentTick);
}

void CGAService::SetBattleEndTick(int msec)
{
	unsigned int currentTick = ((unsigned int)GetTickCount() <= 900000000) ? GetTickCount() : GetTickCount() % 900000000u;

	*g_btl_round_endtick = currentTick + msec;
}

void CGAService::LogBack()
{
	SendMessage(g_MainHwnd, WM_CGA_LOG_BACK, 0, 0);
}

void CGAService::WM_LogBack()
{
	if (!IsInGame() || *g_logback)
		return;

	if (m_game_type == cg_item_6000)
		NET_WriteLogbackPacket_cgitem(*g_net_socket);
	else if (m_game_type == cg_se_3000)
		NET_WriteLogbackPacket_cgse(*g_net_buffer, *g_net_socket, net_header_logback);

	COMMON_PlaySound(60, 320, 240);
	*g_logback = 1;
}

void CGAService::LogOut()
{
	SendMessage(g_MainHwnd, WM_CGA_LOG_OUT, 0, 0);
}

void CGAService::WM_LogOut()
{
	if (!IsInGame())
		return;

	UI_ClickLogout(9, 2);
	COMMON_PlaySound(60, 320, 240);
}

bool CGAService::WM_UseItem(int itempos)
{
	if (!IsInGame())
		return false;

	if (IsItemValid(itempos))
	{
		*g_item_cur_using = itempos;
		if (m_game_type == cg_item_6000)
			NET_WriteUseItemPacket_cgitem(*g_net_socket, g_player_xpos->decode(), g_player_ypos->decode(), itempos, 0);
		else
			NET_WritePacket2(*g_net_buffer, *g_net_socket, net_header_useitem, g_player_xpos->decode(), g_player_ypos->decode(), itempos, 0);
		COMMON_PlaySound(77, 320, 240);
		return true;
	}

	return false;
}

bool CGAService::UseItem(int itempos)
{
	return SendMessageA(g_MainHwnd, WM_CGA_USE_ITEM, itempos, 0) ? true : false;
}

bool CGAService::WM_ClickNPCDialog(int option, int index)
{
	if (!IsInGame())
		return false;

	if (*g_npc_dialog_type == 5 || *g_npc_dialog_type == 16 || *g_npc_dialog_type == 27)
	{
		if (index >= -1 && index <= 2)
		{
			m_ui_minidialog_click_index = index;
			return true;
		}
		return false;
	}

	if (*g_npc_dialog_type == 17)
	{
		if (option == 0)
		{
			m_ui_learn_skill_confirm = 1;
			return true;
		}
		return false;
	}

	NPC_ClickDialog(option, index, 0, 0) ? true : false;
	return true;
}

bool CGAService::ClickNPCDialog(int option, int index)
{
	return SendMessageA(g_MainHwnd, WM_CGA_CLICK_NPC_DIALOG, option, index) ? true : false;
}

bool CGAService::WM_SellNPCStore(cga_sell_items_t *items)
{
	if (!IsInGame())
		return false;

	std::string buffer;
	char buf[32];

	if (*g_npc_dialog_type != 7)
		return false;

	for (size_t i = 0; i < items->size(); ++i)
	{
		if (items->at(i).itemid != 0 && g_item_base[items->at(i).itempos].item_id != items->at(i).itemid)
			return false;
		if (g_item_base[items->at(i).itempos].count < items->at(i).count)
			return false;
		sprintf(buf, "%d\\z%d\\z", items->at(i).itempos, items->at(i).count);
		buffer += buf;
	}
	if(m_game_type == cg_item_6000)
		NET_WritePacket3_cgitem(*g_net_socket, 
			g_player_xpos->decode(), g_player_ypos->decode(),
			*g_npc_dialog_dlgid, *g_npc_dialog_npcid, 0, buffer.c_str());
	else
		NET_WritePacket3(*g_net_buffer, *g_net_socket, net_header_sell,
			g_player_xpos->decode(), g_player_ypos->decode(), 
			*g_npc_dialog_dlgid, *g_npc_dialog_npcid, 0, buffer.c_str());

	COMMON_PlaySound(79, 320, 240);
	return true;
}

bool CGAService::SellNPCStore(cga_sell_items_t items)
{
	return SendMessageA(g_MainHwnd, WM_CGA_SELL_NPC_STORE, (WPARAM)&items, 0) ? true : false;
}

bool CGAService::WM_BuyNPCStore(cga_buy_items_t *items)
{
	if (!IsInGame())
		return false;

	std::string buffer;
	char buf[32];

	if (*g_npc_dialog_type != 6 && *g_npc_dialog_type != 28)
		return false;

	if (*g_npc_dialog_type == 28 && items->size() == 1) {
		sprintf(buf, "%d\\z%d", items->at(0).index, items->at(0).count);
		buffer += buf;
	}
	else
	{
		for (size_t i = 0; i < items->size(); ++i)
		{
			sprintf(buf, "%d\\z%d\\z", items->at(i).index, items->at(i).count);
			buffer += buf;
		}
	}

	if (m_game_type == cg_item_6000)
		NET_WritePacket3_cgitem(*g_net_socket,
			g_player_xpos->decode(), g_player_ypos->decode(),
			*g_npc_dialog_dlgid, *g_npc_dialog_npcid, 0, buffer.c_str());
	else
		NET_WritePacket3(*g_net_buffer, *g_net_socket, net_header_sell,
			g_player_xpos->decode(), g_player_ypos->decode(),
			*g_npc_dialog_dlgid, *g_npc_dialog_npcid, 0, buffer.c_str());

	COMMON_PlaySound(79, 320, 240);
	return true;
}

bool CGAService::BuyNPCStore(cga_buy_items_t items)
{
	return SendMessageA(g_MainHwnd, WM_CGA_BUY_NPC_STORE, (WPARAM)&items, 0) ? true : false;
}

bool CGAService::PlayerMenuSelect(int menuindex)
{
	if (menuindex >= 0 && menuindex < *g_player_menu_count)
	{
		m_menu_item_selected = 1;
		m_menu_item_select_index = menuindex;
		return true;
	}

	return false;
}

bool CGAService::UnitMenuSelect(int menuindex)
{
	if (menuindex >= 0 && menuindex < *g_unit_menu_count)
	{
		m_menu_item_selected = 2;
		m_menu_item_select_index = menuindex;
		return true;
	}
	return false;
}

void CGAService::WM_GetMapUnits(cga_map_units_t *units)
{
	if (!IsInGame())
		return;

	for (int i = 0; i < *g_unit_count; ++i)
	{
		if (g_map_units[i].valid == 0)
			continue;

		units->emplace_back(
			g_map_units[i].valid,
			g_map_units[i].type,
			g_map_units[i].unit_id,
			g_map_units[i].model_id,
			g_map_units[i].xpos.decode(),
			g_map_units[i].ypos.decode(),
			g_map_units[i].item_count,
			boost::locale::conv::to_utf<char>(g_map_units[i].unit_name, "GBK"),
			boost::locale::conv::to_utf<char>(g_map_units[i].nick_name, "GBK"),
			boost::locale::conv::to_utf<char>(g_map_units[i].item_name, "GBK")
		);

		WriteLog("name1=%s, name2=%s, name3=%s, unk10=%d, unk11=%d, unk12=%d",
			g_map_units[i].unit_name,
			g_map_units[i].nick_name,
			g_map_units[i].item_name,
			g_map_units[i].unk10,
			g_map_units[i].unk11,
			g_map_units[i].item_count
		);
	}
}

void CGAService::WM_SayWords(const char *str, int color, int range, int size)
{
	if (!IsInGame())
		return;

	static char message[1024 + 3];
	sprintf(message, "P|%s", boost::locale::conv::from_utf<char>(str, "GBK").c_str());
	if(m_game_type == cg_item_6000)
		NET_WriteSayWords_cgitem(*g_net_socket, g_player_xpos->decode(), g_player_ypos->decode(), message, color, range, size);
	else
		NET_WriteSayWords(*g_net_buffer, *g_net_socket, net_header_sayword,
		g_player_xpos->decode(), g_player_ypos->decode(), message,
		color, range, size);
}

void CGAService::SayWords(std::string str, int color, int range, int size)
{
	SendMessageA(g_MainHwnd, WM_CGA_SAY_WORDS, (WPARAM)str.c_str(), (color & 0xFF) | ((range & 0xFF) << 8) | ((size & 0xFF) << 16));
}

void CGAService::SetWorkDelay(int delay)
{
	if (delay < 0)
		delay = 0;
	*g_work_basedelay = delay - 4500;
}

void CGAService::SetWorkAcceleration(int percent)
{
	m_work_acceleration = percent;
}

bool CGAService::WM_StartWork(int skill_index, int sub_index)
{
	if (!IsInGame())
		return false;
	if (!IsSkillValid(skill_index))
		return false;

	switch (g_skill_base[skill_index].type) {
	case WORK_TYPE_GATHERING:
		UI_OpenGatherDialog(skill_index, 0);
		return true;
	case WORK_TYPE_HEALING:
		if (sub_index < 0 || sub_index > 10 || !g_skill_base[skill_index].sub[sub_index].name[0] || !g_skill_base[skill_index].sub[sub_index].available)
			return false;
		*g_healing_skill_index = skill_index;
		*g_healing_subskill_index = sub_index;
		if(m_game_type == cg_item_6000)
			NET_WriteOpenHealDialog_cgitem(*g_net_socket, skill_index);
		return true;
	case WORK_TYPE_ASSESSING:
		UI_OpenAssessDialog(skill_index, 0);
		return true;
	case WORK_TYPE_CRAFTING:
		int sub_index2 = skill_index & 0xFF;
		int sub_type = (sub_index >> 8) & 0xFF;
		if (sub_index2 < 0 || sub_index2 > 50 || !g_skill_base[skill_index].craft[sub_index2].available)
			return false;
		UI_OpenCraftDialog(skill_index, sub_index, sub_type);
		return true;
	}

	return false;
}

bool CGAService::StartWork(int skill_index, int sub_index)
{
	return SendMessageA(g_MainHwnd, WM_CGA_START_WORK, skill_index, sub_index) ? true : false;
}

int CGAService::IsItemTypeAssessable(int type)
{
	if (type < 0 || type > 6)
	{
		if (type < 7 || type > 14)
			return 0;
		else
			return  2;
	}
	return 1;
}

bool CGAService::WM_AssessItem(int skill_index, int itempos)
{
	if (!IsInGame())
		return false;
	if (!IsSkillValid(skill_index) || g_skill_base[skill_index].type != 2)
		return false;
	if(itempos < 8 || !IsItemValid(itempos))
		return false;

	//do validate before assessing item
	switch (g_skill_base[skill_index].skill_id)
	{
	case 219:
		if ((*g_playerBase)->iteminfos[itempos].assessed)
			return false;
		if ((*g_playerBase)->iteminfos[itempos].level > g_skill_base[skill_index].level + (*g_work_accelerate) ? 1 : 0)
			return false;
		break;
	case 217: case 218:
		if (!(*g_playerBase)->iteminfos[itempos].assessed)
			return false;
		if (g_skill_base[skill_index].skill_id == 217 && IsItemTypeAssessable((*g_playerBase)->iteminfos[itempos].type) == 2)
			return false;
		if (g_skill_base[skill_index].skill_id == 218 && IsItemTypeAssessable((*g_playerBase)->iteminfos[itempos].type) == 1)
			return false;
		if(!IsItemTypeAssessable((*g_playerBase)->iteminfos[itempos].type))
			return false;
		if ((*g_playerBase)->iteminfos[itempos].level > g_skill_base[skill_index].level + (*g_work_accelerate) ? 1 : 0)
			return false;
		break;
	case 220:
		if (((*g_playerBase)->iteminfos[itempos].assess_flags & 0x200) && !((*g_playerBase)->iteminfos[itempos].assess_flags & 0x400))
			return false;
		if (((*g_playerBase)->iteminfos[itempos].assess_flags & 0x100) && !((*g_playerBase)->iteminfos[itempos].assess_flags & 0x400))
			return false;
		if (!(*g_playerBase)->iteminfos[itempos].assessed)
			return false;
		if ((*g_playerBase)->iteminfos[itempos].count > 0 || (*g_playerBase)->iteminfos[itempos].type == 47)
			return false;
		break;
	}

	for (int i = 1; i < 5; ++i)
		m_ui_craftdialog_additem[i] = -1;

	m_ui_craftdialog_additem[0] = itempos - 8;
	m_ui_craftdialog_additemcount = 1;

	return true;
}

bool CGAService::AssessItem(int skill_index, int itempos)
{
	return SendMessageA(g_MainHwnd, WM_CGA_ASSESS_ITEM, skill_index, itempos) ? true : false;
}

bool CGAService::WM_CraftItem(cga_craft_item_t *craft)
{
	if (!IsInGame())
		return false;

	if (!IsSkillValid(craft->skill_index) || g_skill_base[craft->skill_index].type != 1)
		return false;

	if (craft->subskill_index < 0 || craft->subskill_index > 50)
		return false;
	
	CGA::skill_craft_t &skill_craft = g_skill_base[craft->skill_index].craft[craft->subskill_index];
	if (!skill_craft.available || !skill_craft.name[0])
		return false;
	
	m_ui_craftdialog_additemcount = 0;
	int x = 0;
	for (int i = 0; i < 5; ++i)
	{
		m_ui_craftdialog_additem[i] = -1;
		if (!skill_craft.mats[i].name[0])
			continue;
		bool found = false;
		int itemid = skill_craft.mats[i].itemid;
		int desiredcount = skill_craft.mats[i].count;
		for (size_t j = 0; j < 5; ++j)
		{
			int itempos = craft->itempos[j];
			if (itempos >= 8 && IsItemValid(itempos))
			{
				if (itemid == (*g_playerBase)->iteminfos[itempos].item_id)
				{
					int itemcount = (*g_playerBase)->iteminfos[itempos].count;
					if (!itemcount)
						itemcount = 1;
					if (itemcount >= desiredcount)
					{
						m_ui_craftdialog_additem[m_ui_craftdialog_additemcount] = itempos - 8;
						++m_ui_craftdialog_additemcount;
						found = true;
						break;
					}
				}
			}
		}
		if (!found)
			return false;
	}

	return true;
}

bool CGAService::CraftItem(cga_craft_item_t craft)
{
	return SendMessageA(g_MainHwnd, WM_CGA_CRAFT_ITEM, (WPARAM)&craft, 0) ? true : false;
}

bool CGAService::WM_IsMapCellPassable(int x, int y)
{
	int offsetX = x - *g_map_x_bottom;
	int offsetY = y - *g_map_y_bottom;

	return (offsetX >= 0 && offsetX < *g_map_x_size && offsetY >= 0 && offsetY < *g_map_y_size &&(g_map_collision_table[offsetX + offsetY * (*g_map_x_size) ] != 1)) ?  true : false;
}

bool CGAService::IsMapCellPassable(int x, int y)
{
	return SendMessageA(g_MainHwnd, WM_CGA_IS_MAP_CELL_PASSABLE, x, y) ? true : false;
}

bool CGAService::WM_ForceMove(int dir, bool show)
{
	if (!IsInGame())
		return false;

	if (GetWorldStatus() != 9 || GetGameStatus() != 3)
		return false;

	int x, y, dx, dy;
	dx = x = g_player_xpos->decode();
	dy = y = g_player_ypos->decode();

	switch (dir) {
	case 0: ++dx; break;
	case 1: ++dx; ++dy; break;
	case 2: ++dy; break;
	case 3: --dx; ++dy; break;
	case 4: --dx; break;
	case 5: --dx; --dy; break;
	case 6: --dy; break;
	case 7: ++dx; --dy; break;
	default: return false;
	}

	if (WM_IsMapCellPassable(dx, dy))
	{
		char buf[2];
		buf[0] = (dir >= 6) ? dir-6 + 'a ' : dir + 'c';
		buf[1] = 0;
		if (m_game_type == cg_item_6000)
		{
			NET_WriteMovePacket_cgitem(x, y, buf);
		}
		else 
		{
			//TODO
		}

		g_player_xpos->set(dx);
		g_player_ypos->set(dy);

		if (show)
		{
			*g_map_xf = dx * 64.0f;
			*g_map_xf2 = dx * 64.0f;
			*g_map_yf = dy * 64.0f;
			*g_map_yf2 = dy * 64.0f;
			g_map_xpos->set(dx);
			g_map_ypos->set(dy);
		}
		return true;
	}

	return false;
}

bool CGAService::ForceMove(int dir, bool show)
{
	return SendMessageA(g_MainHwnd, WM_CGA_FORCE_MOVE, dir, show ? 1 : 0) ? true : false;
}

bool CGAService::WM_ForceMoveTo(int x, int y, bool show)
{
	if (!IsInGame())
		return false;

	if (GetWorldStatus() != 9 || GetGameStatus() != 3)
		return false;

	if (WM_IsMapCellPassable(x, y))
	{
		int dir = (*g_playerBase)->direction;
		g_player_xpos->set(x);
		g_player_ypos->set(y);

		char buf[2];
		buf[0] = dir + 'A';
		buf[1] = 0;
		if (m_game_type == cg_item_6000)
		{
			NET_WriteMovePacket_cgitem(x, y, buf);
		}
		else
		{
			//TODO
		}

		if (show)
		{
			*g_map_xf = x * 64.0f;
			*g_map_xf2 = x * 64.0f;
			*g_map_yf = y * 64.0f;
			*g_map_yf2 = y * 64.0f;
			g_map_xpos->set(x);
			g_map_ypos->set(y);
		}

		return true;
	}

	return false;
}

bool CGAService::ForceMoveTo(int x, int y, bool show)
{
	return SendMessageA(g_MainHwnd, WM_CGA_FORCE_MOVE_TO, (x & 0xFFFF) | ((y & 0xFFFF) << 16) , show ? 1 : 0) ? true : false;
}

void CGAService::WM_GetCraftInfo(cga_craft_item_t *craft, cga_craft_info_t *info)
{
	if (!IsInGame())
		return;

	if (!IsSkillValid(craft->skill_index) || g_skill_base[craft->skill_index].type != 1)
		return;

	if (craft->subskill_index < 0 || craft->subskill_index > 50)
		return;

	CGA::skill_craft_t &skill_craft = g_skill_base[craft->skill_index].craft[craft->subskill_index];
	if (skill_craft.available && skill_craft.name[0])
	{
		info->id = skill_craft.subskill_id;
		info->cost = skill_craft.cost;
		info->level = skill_craft.level;
		info->itemid = skill_craft.itemid;
		info->index = craft->subskill_index;
		info->name = boost::locale::conv::to_utf<char>(skill_craft.name, "GBK");
		info->info = boost::locale::conv::to_utf<char>(skill_craft.info, "GBK");
		for (int i = 0; i < 5; ++i)
		{
			if (skill_craft.mats[i].name[0])
			{
				info->materials[i].name = boost::locale::conv::to_utf<char>(skill_craft.mats[i].name, "GBK");
				info->materials[i].count = skill_craft.mats[i].count;
				info->materials[i].itemid = skill_craft.mats[i].itemid;
			}
		}
	}
}

cga_craft_info_t CGAService::GetCraftInfo(int skill_index, int subskill_index)
{
	cga_craft_info_t info;
	cga_craft_item_t craft(skill_index, subskill_index, 0);
	SendMessageA(g_MainHwnd, WM_CGA_GET_CRAFT_INFO, (WPARAM)&craft, (LPARAM)&info) ? true : false;
	return info;
}

void CGAService::WM_GetCraftsInfo(int skill_index, cga_crafts_info_t *info)
{
	if (!IsInGame())
		return;

	if (!IsSkillValid(skill_index) || g_skill_base[skill_index].type != 1)
		return;

	for (int i = 0; i < 50; ++i)
	{
		CGA::skill_craft_t &skill_craft = g_skill_base[skill_index].craft[i];
		if (skill_craft.available && skill_craft.name[0])
		{
			info->emplace_back(
				skill_craft.subskill_id,
				skill_craft.cost,
				skill_craft.level,
				skill_craft.itemid,
				i,
				boost::locale::conv::to_utf<char>(skill_craft.name, "GBK"),
				boost::locale::conv::to_utf<char>(skill_craft.info, "GBK")
			);
			cga_craft_info_t &inf = info->at(info->size() - 1);
			for (int j = 0; j < 5; ++j)
			{
				if (skill_craft.mats[j].name[0])
				{
					inf.materials[j].name = boost::locale::conv::to_utf<char>(skill_craft.mats[j].name, "GBK");
					inf.materials[j].count = skill_craft.mats[j].count;
					inf.materials[j].itemid = skill_craft.mats[j].itemid;
				}
			}
		}
	}	
}

cga_crafts_info_t CGAService::GetCraftsInfo(int skill_index)
{
	cga_crafts_info_t info;
	SendMessageA(g_MainHwnd, WM_CGA_GET_CRAFTS_INFO, skill_index, (LPARAM)&info) ? true : false;
	return info;
}