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
	char buffer[4096];
	va_list argptr;
	int cnt;
	va_start(argptr, fmt);
	cnt = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	FILE *fp = fopen("D:\\cga_log.txt", "a+");
	if (fp)
	{
		setlocale(LC_ALL, "chs");

		SYSTEMTIME tm;
		GetLocalTime(&tm);
		fprintf(fp, "%02d:%02d:%02d.%03d\t", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);

		fputs(buffer, fp);
		fclose(fp);
	}
}

void WriteLog2(LPCSTR buffer)
{
	FILE *fp = fopen("D:\\cga_log.txt", "a+");
	if (fp)
	{
		setlocale(LC_ALL, "chs");

		SYSTEMTIME tm;
		GetLocalTime(&tm);
		fprintf(fp, "%02d:%02d:%02d.%03d\t", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);

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

char *NET_FindDelimeter(const char *src, char deli)
{
	char *p2;
	for (auto p = (char *)src; ; p = p2 + 1)
	{
		while (1)
		{
			if (!*p)
				return 0;
			if (*p >= 128 || *p == 255)
				break;
			if (*p == deli)
				return p + 1;
			++p;
		}
		p2 = p + 1;
		if (!*(char *)p2)
			break;
	}
	return NULL;
}

int NET_SubStrDelimeter(const char *src, char deli, int len, char *buf)
{
	int i;

	for (i = 0; i < len; ++i)
	{
		if (src[i] >= 128 || src[i] == 255)
		{
			buf[i] = src[i];
			++i;
			if (i >= len)
				break;
			buf[i] = src[i];
			if (!buf[i])
				return 1;
		}
		else
		{
			if (src[i] == deli)
			{
				buf[i] = 0;
				return 0;
			}
			buf[i] = src[i];
			if (!buf[i])
				return 1;
		}
	}
	buf[i] = 0;
	return 1;
}

int NET_ParseDelimeter(const char *src, char deli, int position, int buflen, char *buf)
{
	auto p = (char *)src;
	for (int i = 0; i < position - 1 && p; ++i)
		p = NET_FindDelimeter(p, deli);

	if (p)
		return NET_SubStrDelimeter(p, deli, buflen, buf);

	*buf = 0;
	return 1;
}

int NET_ParseInteger(const char *src, char deli, int position)
{
	char buf[128] = {0};

	NET_ParseDelimeter(src, deli, position, 127, buf);
	if (buf[0])
		return atoi((const char *)buf);

	return -1;
}

int NET_ParseHexInteger(const char *src, char deli, int position)
{
	char buf[128] = { 0 };

	NET_ParseDelimeter(src, deli, position, 127, buf);
	if (buf[0]) {
		int res;
		if(1 == sscanf(buf, "%X", &res))
			return res;
	}

	return -1;
}

char NET_EscapeCharFromTable(char ch)
{
	static char table[] = "n\nc,z|y\\";

	for (int i = 0; i < 4; ++i)
	{
		if (table[2 * i] == ch)
			return table[2 * i + 1];
	}
	return ch;
}

char *NET_EscapeStringEx(char *src)
{
	size_t len; // kr00_4@1
	int v2; // ST18_4@4
	int v4; // [sp+14h] [bp-8h]@1

	len = strlen(src);
	v4 = 0;
	for (size_t i = 0; i < len; ++i)
	{
		if (src[i] & 0x80)
		{
			src[v4] = src[i];
			v2 = v4 + 1;
			++i;
			src[v2] = src[i];
			v4 = v2 + 1;
		}
		else if (src[i] == '\\')
		{
			++i;
			src[v4++] = NET_EscapeCharFromTable(src[i]);
		}
		else
		{
			src[v4++] = src[i];
		}
	}
	src[v4] = 0;
	return src;
}

void CGA_NotifyBattleAction(int flags);
void CGA_NotifyPlayerMenu(const cga_player_menu_items_t &players);
void CGA_NotifyUnitMenu(const cga_unit_menu_items_t &units);
void CGA_NotifyNPCDialog(const cga_npc_dialog_t &dlg);
void CGA_NotifyWorkingResult(const CGA::cga_working_result_t &result);
void CGA_NotifyChatMsg(const CGA::cga_chat_msg_t &msg);
void CGA_NotifyTradeStuffs(const CGA::cga_trade_stuff_info_t &msg);
void CGA_NotifyTradeDialog(const CGA::cga_trade_dialog_t &msg);
void CGA_NotifyTradeState(int state);

void CGAService::NewBATTLE_PlayerAction()
{
	int prevPlayerStatus = *g_btl_player_status;

	BATTLE_PlayerAction();

	int playerStatus = *g_btl_player_status;

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
		if (*g_btl_skill_performed)
			flags |= FL_BATTLE_ACTION_ISSKILLPERFORMED;

		CGA_NotifyBattleAction(flags);
	}
	if (prevPlayerStatus != 4 && playerStatus == 4)
	{
		//Notify pet action
		int flags = 0;
		if (m_btl_double_action)
			flags |= FL_BATTLE_ACTION_ISDOUBLE;
		if (*g_btl_skill_performed)
			flags |= FL_BATTLE_ACTION_ISSKILLPERFORMED;

		CGA_NotifyBattleAction(flags);
	}
}

void __cdecl NewBATTLE_PlayerAction()
{
	g_CGAService.NewBATTLE_PlayerAction();
}

void CGAService::NewNET_ParseTradeItemsPackets(int a1, const char *buf)
{
	WriteLog("NewNET_ParseTradeItemsPackets\n");

	NET_ParseTradeItemsPackets(a1, buf);

	cga_trade_stuff_info_t info(TRADE_STUFFS_ITEM);

	char buffer[1024];

	for (int i = 0; ; ++i)
	{
		int position = 14 * i + 1;
		int itempos = NET_ParseInteger(buf, '|', position);
		
		if (NET_ParseDelimeter(buf, 124, position + 1, 255, buffer) == 1)
			break;
		NET_EscapeStringEx(buffer);
		std::string itemname = boost::locale::conv::to_utf<char>(buffer, "GBK");

		NET_ParseDelimeter(buf, 124, position + 3, 767, buffer);
		NET_EscapeStringEx(buffer);
		std::string itemattr = boost::locale::conv::to_utf<char>(buffer, "GBK");

		NET_ParseDelimeter(buf, 124, position + 13, 255, buffer);
		NET_EscapeStringEx(buffer);
		std::string itemattr2 = boost::locale::conv::to_utf<char>(buffer, "GBK");

		int image_id = NET_ParseInteger(buf, '|', position + 4);
		int level = NET_ParseInteger(buf, '|', position + 8);
		int type = NET_ParseInteger(buf, '|', position + 9);
		int item_id = NET_ParseInteger(buf, '|', position + 10);
		int count = NET_ParseInteger(buf, '|', position + 12);

		info.items.emplace_back(
			itemname,
			itemattr,
			itemattr2,
			std::string(),
			std::string(),
			item_id,
			count,
			itempos,
			level,
			type,
			false
		);
	}

	CGA_NotifyTradeStuffs(info);
}

void __cdecl NewNET_ParseTradeItemsPackets(int a1, const char *buf)
{
	g_CGAService.NewNET_ParseTradeItemsPackets(a1, buf);
}

void CGAService::NewNET_ParseTradePetPackets(int a1, int index, const char *buf)
{
	NET_ParseTradePetPackets(a1, index, buf);

	WriteLog("NewNET_ParseTradePetPackets\n");

	WriteLog2(buf);

	char name[256];

	if (0 == NET_ParseDelimeter(buf, '|', 1, 255, name))
	{
		cga_trade_stuff_info_t info(TRADE_STUFFS_PET);

		NET_EscapeStringEx(name);
		info.pet.index = index;
		info.pet.name = boost::locale::conv::to_utf<char>(name, "GBK");
		info.pet.level = NET_ParseInteger(buf, '|', 2);
		info.pet.detail.points_remain = NET_ParseInteger(buf, '|', 3);
		info.pet.race = NET_ParseInteger(buf, '|', 4);
		info.pet.maxhp = NET_ParseInteger(buf, '|', 5);
		info.pet.maxmp = NET_ParseInteger(buf, '|', 6);
		info.pet.detail.points_endurance = NET_ParseInteger(buf, '|', 7);
		info.pet.detail.points_strength = NET_ParseInteger(buf, '|', 8);
		info.pet.detail.points_defense = NET_ParseInteger(buf, '|', 9);
		info.pet.detail.points_agility = NET_ParseInteger(buf, '|', 10);
		info.pet.detail.points_magical = NET_ParseInteger(buf, '|', 11);
		info.pet.loyality = NET_ParseInteger(buf, '|', 12);
		info.pet.skill_count = NET_ParseInteger(buf, '|', 13);
		if (0 == NET_ParseDelimeter(buf, '|', 14, 255, name))
		{
			NET_EscapeStringEx(name);
			info.pet.realname = boost::locale::conv::to_utf<char>(name, "GBK");
		}
		info.pet.image_id = NET_ParseInteger(buf, '|', 15);
		info.pet.detail.value_attack = NET_ParseInteger(buf, '|', 16);
		info.pet.detail.value_defensive = NET_ParseInteger(buf, '|', 17);
		info.pet.detail.value_agility = NET_ParseInteger(buf, '|', 18);
		info.pet.detail.value_spirit = NET_ParseInteger(buf, '|', 19);
		info.pet.detail.value_recovery = NET_ParseInteger(buf, '|', 20);
		info.pet.detail.element_earth = NET_ParseInteger(buf, '|', 21);
		info.pet.detail.element_water = NET_ParseInteger(buf, '|', 22);
		info.pet.detail.element_fire = NET_ParseInteger(buf, '|', 23);
		info.pet.detail.element_wind = NET_ParseInteger(buf, '|', 24);

		CGA_NotifyTradeStuffs(info);
	}
}

void __cdecl NewNET_ParseTradePetPackets(int a1, int index, const char *buf)
{
	g_CGAService.NewNET_ParseTradePetPackets(a1, index, buf);
}

void CGAService::NewNET_ParseTradePetSkillPackets(int a1, int index, const char *buf)
{
	NET_ParseTradePetSkillPackets(a1, index, buf);

	WriteLog("NewNET_ParseTradePetSkillPackets\n");

	WriteLog2(buf);

	cga_trade_stuff_info_t info(TRADE_STUFFS_PETSKILL);

	char name[256];

	info.petskills.index = index;

	for (int i = 0; i < 10; ++i)
	{
		if (1 != NET_ParseDelimeter(buf, '|', i + 1, 255, name))
		{
			NET_EscapeStringEx(name);

			info.petskills.skills.emplace_back(boost::locale::conv::to_utf<char>(name, "GBK"));
		}
	}

	CGA_NotifyTradeStuffs(info);
}

void __cdecl NewNET_ParseTradePetSkillPackets(int a1, int index, const char *buf)
{
	g_CGAService.NewNET_ParseTradePetSkillPackets(a1, index, buf);
}

void CGAService::NewNET_ParseTradeGoldPackets(int a1, int gold)
{
	WriteLog("NewNET_ParseTradeGoldPackets\n");

	NET_ParseTradeGoldPackets(a1, gold);

	cga_trade_stuff_info_t info(TRADE_STUFFS_GOLD);

	info.gold = gold;

	CGA_NotifyTradeStuffs(info);
}

void __cdecl NewNET_ParseTradeGoldPackets(int a1, int gold)
{
	g_CGAService.NewNET_ParseTradeGoldPackets(a1, gold);
}

void CGAService::NewNET_ParseTradePlayers(int a1, const char *src, char *src2)
{
	WriteLog("NewNET_ParseTradePlayers\n");

	NET_ParseTradePlayers(a1, src, src2);

	char buf[256];
	int numplayers = 0;
	int position = 1;

	cga_player_menu_items_t players;

	for (int i = 0; i < 10; ++i)
	{
		if (numplayers >= 10)
			break;
		if (NET_ParseDelimeter(src, '|', position, 255, buf) == 1)
			break;
		NET_EscapeStringEx(buf);
		
		std::string playername = boost::locale::conv::to_utf<char>(buf, "GBK");

		if (NET_ParseDelimeter(src2, '|', position, 255, buf) == 1)
			break;
		NET_EscapeStringEx(buf);

		int playerlevel = atoi(buf);
		
		players.emplace_back(
			playername,
			playerlevel,
			i
		);

		++position;
		++numplayers;
	}

	m_player_menu_type = PLAYER_MENU_TRADE;

	CGA_NotifyPlayerMenu(players);
}

void __cdecl NewNET_ParseTradePlayers(int a1, const char *src, char *src2)
{
	g_CGAService.NewNET_ParseTradePlayers(a1, src, src2);
}

void CGAService::NewNET_ParseHealPlayers(int a1, const char *src)
{
	WriteLog("NewNET_ParseHealPlayers %s\n", src);

	NET_ParseHealPlayers(a1, src);

	char buf[256];
	int numplayers = 0;
	int position = 1;

	cga_player_menu_items_t players;

	std::string playername;

	playername = boost::locale::conv::to_utf<char>((*g_playerBase)->name, "GBK");

	players.emplace_back(
		playername,
		0,
		numplayers
	);

	numplayers++;

	for (int i = 0; i < 5; ++i)
	{
		if (g_team_player_base[i].valid && 
			g_team_player_base[i].unit_id != (*g_playerBase)->unitid)
		{
			playername = boost::locale::conv::to_utf<char>(g_team_player_base[i].name, "GBK");
			if (playername.empty())
			{
				if (g_team_player_base[i].actor) {
					playername = boost::locale::conv::to_utf<char>((char *)g_team_player_base[i].actor + 196, "GBK");
				}

				/*for (int j = 0; j < *g_unit_count; ++j)
				{
					if (g_map_units[j].valid && g_map_units[j].unit_id == g_team_player_base[i].unit_id)
					{
						playername = boost::locale::conv::to_utf<char>(g_map_units[j].unit_name, "GBK");
						break;
					}
				}*/
			}

			players.emplace_back(
				playername,
				1,
				numplayers
			);
			numplayers++;
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		if (numplayers >= 10)
			break;
		if (NET_ParseDelimeter(src, '|', position, 255, buf) == 1)
			break;
		NET_EscapeStringEx(buf);

		playername = boost::locale::conv::to_utf<char>(buf, "GBK");

		int type = NET_ParseInteger(src, '|', position);
		if (type == 1)
			break;

		players.emplace_back(
			playername,
			type ? 3 : 2,
			numplayers
		);

		++position;
		++numplayers;
	}

	m_player_menu_type = PLAYER_MENU_HEAL;

	CGA_NotifyPlayerMenu(players);
}

void __cdecl NewNET_ParseHealPlayers(int a1, const char *src)
{
	g_CGAService.NewNET_ParseHealPlayers(a1, src);
}

void CGAService::NewNET_ParseHealUnits(int a1, const char *src)
{
	WriteLog("NewNET_ParseHealUnits\n");

	NET_ParseHealUnits(a1, src);

	char buf[128];
	int numunits = 0;
	std::string unitname;

	cga_unit_menu_items_t units;

	if (g_heal_player_menu_type[*g_heal_player_menu_select])
	{
		int i = 0;
		int position = 1;
		while (i < 6)
		{
			if (NET_ParseDelimeter(src, '|', position++, 127, buf) == 1)
				break;
			if (*g_heal_menu_type != 1 || i)
			{
				NET_EscapeStringEx(buf);
				unitname = boost::locale::conv::to_utf<char>(buf, "GBK");
				int level = NET_ParseInteger(src, '|', position++);
				int health = NET_ParseInteger(src, '|', position++);
				int hp = NET_ParseInteger(src, '|', position++);
				int maxhp = NET_ParseInteger(src, '|', position++);
				int mp = NET_ParseInteger(src, '|', position++);
				int maxmp = NET_ParseInteger(src, '|', position++);
				int color = 0;
				switch (g_heal_player_menu_type[*g_heal_player_menu_select])
				{
				case 1:
					if (i)
						color = 5;
					else
						color = 1;
					break;
				case 2:
					if (i)
						color = 6;
					else
						color = 2;
					break;
				case 3:
					color = 3;
					break;
				}

				units.emplace_back(
					unitname,
					level,
					health,
					hp,
					maxhp,
					mp,
					maxmp,
					color,
					numunits
				);
				numunits++;
			}
			else
			{
				position += 6;
				++numunits;
			}
		}
	}
	else 
	{
		if (*g_heal_menu_type != 1)
		{
			unitname = boost::locale::conv::to_utf<char>((*g_playerBase)->name, "GBK");
			units.emplace_back(
				unitname,
				(*g_playerBase)->level,
				(*g_playerBase)->health,
				(*g_playerBase)->hp.decode(),
				(*g_playerBase)->maxhp.decode(),
				(*g_playerBase)->mp.decode(),
				(*g_playerBase)->maxmp.decode(),
				0,
				numunits
			);
			numunits++;
		}
		for (int i = 0; i < 5; ++i)
		{
			if (g_pet_base[i].level)
			{
				unitname = boost::locale::conv::to_utf<char>(g_pet_base[i].name, "GBK");
				if (unitname.empty())
					unitname = boost::locale::conv::to_utf<char>(g_pet_base[i].realname, "GBK");
				units.emplace_back(
					unitname,
					g_pet_base[i].level,
					g_pet_base[i].health,
					g_pet_base[i].hp.decode(),
					g_pet_base[i].maxhp.decode(),
					g_pet_base[i].mp.decode(),
					g_pet_base[i].maxmp.decode(),
					4,
					numunits
				);
				numunits++;
			}
		}
	}

	m_unit_menu_type = UNIT_MENU_HEAL;

	CGA_NotifyUnitMenu(units);
}

void __cdecl NewNET_ParseHealUnits(int a1, const char *src)
{
	g_CGAService.NewNET_ParseHealUnits(a1, src);
}

void CGAService::NewNET_ParseItemPlayers(int a1, const char *src)
{
	WriteLog("NewNET_ParseItemPlayers\n");

	NET_ParseItemPlayers(a1, src);

	char buf[256];
	int numplayers = 0;
	int position = 1;

	cga_player_menu_items_t players;

	std::string playername;

	playername = boost::locale::conv::to_utf<char>((*g_playerBase)->name, "GBK");

	players.emplace_back(
		playername,
		0,
		numplayers
	);

	numplayers++;

	for (int i = 0; i < 5; ++i)
	{
		if (g_team_player_base[i].valid &&
			g_team_player_base[i].unit_id != (*g_playerBase)->unitid)
		{
			playername = boost::locale::conv::to_utf<char>(g_team_player_base[i].name, "GBK");

			if (playername.empty())
			{
				if (g_team_player_base[i].actor) {
					playername = boost::locale::conv::to_utf<char>((char *)g_team_player_base[i].actor + 196, "GBK");
				}
				/*for (int j = 0; j < *g_unit_count; ++j)
				{
					if (g_map_units[j].valid && g_map_units[j].unit_id == g_team_player_base[i].unit_id)
					{
						playername = boost::locale::conv::to_utf<char>(g_map_units[j].unit_name, "GBK");
						break;
					}
				}*/
			}

			players.emplace_back(
				playername,
				1,
				numplayers
			);
			numplayers++;
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		if (numplayers >= 10)
			break;
		if (NET_ParseDelimeter(src, '|', position, 255, buf) == 1)
			break;
		NET_EscapeStringEx(buf);

		playername = boost::locale::conv::to_utf<char>(buf, "GBK");

		int type = NET_ParseInteger(src, '|', position);
		if (type == 1)
			break;

		players.emplace_back(
			playername,
			type ? 3 : 2,
			numplayers
		);

		++position;
		++numplayers;
	}

	m_player_menu_type = PLAYER_MENU_ITEM;

	CGA_NotifyPlayerMenu(players);
}

void __cdecl NewNET_ParseItemPlayers(int a1, const char *src)
{
	g_CGAService.NewNET_ParseItemPlayers(a1, src);
}

void CGAService::NewNET_ParseItemUnits(int a1, const char *src)
{
	WriteLog("NewNET_ParseItemUnits\n");

	NET_ParseItemUnits(a1, src);

	char buf[128];
	int numunits = 0;
	std::string unitname;

	cga_unit_menu_items_t units;

	if (g_item_player_menu_type[10 * (*g_item_player_select)])
	{
		int i = 0;
		int position = 1;
		while (i < 6)
		{
			if (NET_ParseDelimeter(src, '|', position++, 127, buf) == 1)
				break;
			
			NET_EscapeStringEx(buf);
			unitname = boost::locale::conv::to_utf<char>(buf, "GBK");
			int level = NET_ParseInteger(src, '|', position++);
			int health = NET_ParseInteger(src, '|', position++);
			int hp = NET_ParseInteger(src, '|', position++);
			int maxhp = NET_ParseInteger(src, '|', position++);
			int mp = NET_ParseInteger(src, '|', position++);
			int maxmp = NET_ParseInteger(src, '|', position++);
			int color = 0;
			switch (g_heal_player_menu_type[*g_heal_player_menu_select])
			{
			case 1:
				if (i)
					color = 5;
				else
					color = 1;
				break;
			case 2:
				if (i)
					color = 6;
				else
					color = 2;
				break;
			case 3:
				color = 3;
				break;
			}

			units.emplace_back(
				unitname,
				level,
				health,
				hp,
				maxhp,
				mp,
				maxmp,
				color,
				numunits
			);
			numunits++;
		}
	}
	else
	{
		unitname = boost::locale::conv::to_utf<char>((*g_playerBase)->name, "GBK");
		units.emplace_back(
			unitname,
			(*g_playerBase)->level,
			(*g_playerBase)->health,
			(*g_playerBase)->hp.decode(),
			(*g_playerBase)->maxhp.decode(),
			(*g_playerBase)->mp.decode(),
			(*g_playerBase)->maxmp.decode(),
			0,
			numunits
		);
		numunits++;

		for (int i = 0; i < 5; ++i)
		{
			if (g_pet_base[i].level)
			{
				unitname = boost::locale::conv::to_utf<char>(g_pet_base[i].name, "GBK");
				if(unitname.empty())
					unitname = boost::locale::conv::to_utf<char>(g_pet_base[i].realname, "GBK");
				units.emplace_back(
					unitname,
					g_pet_base[i].level,
					g_pet_base[i].health,
					g_pet_base[i].hp.decode(),
					g_pet_base[i].maxhp.decode(),
					g_pet_base[i].mp.decode(),
					g_pet_base[i].maxmp.decode(),
					4,
					numunits
				);
				numunits++;
			}
		}
	}

	m_unit_menu_type = UNIT_MENU_ITEM;

	CGA_NotifyUnitMenu(units);
}

void __cdecl NewNET_ParseItemUnits(int a1, const char *src)
{
	g_CGAService.NewNET_ParseItemUnits(a1, src);
}

bool CGAService::ParseIsKnockout(const char *buf)
{
	return false;

	char *p = (char *)buf + 2;

	while (p)
	{
		p = strstr(p, "SKL|");
		if (p)
		{
			p += 4;
			char *src = strstr(p, "|a");
			char *dst = strstr(p, "|d");
			if (src && dst)
			{
				int srcpos = -1, dstpos = -1;
				sscanf(src + 2, "%X", &srcpos);
				sscanf(dst + 2, "%X", &dstpos);

				if (dstpos >= 0 && dstpos < 20 && m_battle_units[dstpos].exist 
					&& dstpos == *g_btl_player_pos)//0 == strcmp(m_battle_units[dstpos].name, (*g_playerBase)->name))
				{
					char temp[32];
					sprintf(temp, "|d%X|", dstpos);
					char *flags = dst + strlen(temp);
					int flagsv = -1;
					sscanf(flags, "%X", &flagsv);
					if (flagsv != -1 && (flagsv & 0x20060) == 0x20060) {
						WriteLog("knockout %X\n", flagsv);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CGAService::ParseIsEscape(const char *buf)
{
	char *p = (char *)buf + 2;

	while (p)
	{
		p = strstr(p, "ESC|");
		if (p)
		{
			p += 4;
			char *pos = strstr(p, "|e");
			char *sta = strstr(p, "|f");
			if (pos && sta)
			{
				int srcpos = -1, status = -1;
				sscanf(pos + 2, "%X", &srcpos);
				sscanf(sta + 2, "%X", &status);

				if (srcpos >= 0 && srcpos < 20 && m_battle_units[srcpos].exist
					&& srcpos == *g_btl_player_pos)//0 == strcmp(m_battle_units[srcpos].name, (*g_playerBase)->name) && status == 1)
				{
					WriteLog("escaped %X\n", srcpos);
					return true;
				}
			}
		}
	}
	return false;
}

void CGAService::NewNET_ParseBattlePackets(int a1, const char *buf)
{
	WriteLog("NewNET_ParseBattlePackets %s\n", buf);

	if (*buf == 'M')
	{
		if (m_btl_highspeed_enable)
		{
			if (strstr(buf, "END|"))//battle end
			{
				NET_ParseBattlePackets(a1, "M|END|");

				CGA_NotifyBattleAction(FL_BATTLE_ACTION_END);
			}
			else
			{
				if (ParseIsKnockout(buf) || ParseIsEscape(buf))
				{
					NET_ParseBattlePackets(a1, "M|END|");
					CGA_NotifyBattleAction(FL_BATTLE_ACTION_END);
					return;
				}

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
	WriteLog("NewNET_ParseChatMsg u=%d, buf=%s, color=%d, size=%d\n", unitid, buf, color, size);

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

void CGAService::NewNET_ParseSysMsg(int a1, const char *buf)
{
	char temp[4096];
	strcpy(temp, buf);
	NET_EscapeStringEx(temp);
	cga_chat_msg_t msg(-1, boost::locale::conv::to_utf<char>(temp, "GBK"), 0, 0);
	CGA_NotifyChatMsg(msg);

	WriteLog("NewNET_ParseSysMsg %s\n", temp);

	NET_ParseSysMsg(a1, buf);
}

void __cdecl NewNET_ParseSysMsg(int a1, const char *buf)
{
	g_CGAService.NewNET_ParseSysMsg(a1, buf);
}

void CGAService::NewNET_ParseReadyTrade()
{
	WriteLog("NewNET_ParseConfirmTrade\n");

	NET_ParseReadyTrade();

	CGA_NotifyTradeState(TRADE_STATE_READY);
}

void __cdecl NewNET_ParseReadyTrade()
{
	g_CGAService.NewNET_ParseReadyTrade();
}


void CGAService::NewNET_ParseConfirmTrade(int a1, int a2)
{
	WriteLog("NewNET_ParseConfirmTrade\n");

	NET_ParseConfirmTrade(a1, a2);

	if(a2 == 1)
		CGA_NotifyTradeState(TRADE_STATE_CONFIRM);
	else if (a2 == 0)
		CGA_NotifyTradeState(TRADE_STATE_CANCEL);
	else if (a2 == 2)
		CGA_NotifyTradeState(TRADE_STATE_SUCCEED);
}

void __cdecl NewNET_ParseConfirmTrade(int a1, int a2)
{
	g_CGAService.NewNET_ParseConfirmTrade(a1, a2);
}

void __cdecl NewNET_ParseMeetEnemy(int a1, int a2, int a3)
{
	g_CGAService.NET_ParseMeetEnemy(a1, a2, a3);

	WriteLog("NewNET_ParseMeetEnemy %d %d %d\n", a1, a2, a3);
}

void __cdecl NewR_DrawText(int a1)
{
	g_CGAService.R_DrawText(a1);
	g_CGAService.DrawCustomText();
}

void CGAService::NewMove_Player()
{
	(*g_playerBase)->move_speed = m_move_speed;

	auto x = g_player_xpos->decode();
	auto y = g_player_ypos->decode();

	if (*g_disable_move == 0 &&
		*g_map_xf == x * 64.0 &&
		*g_map_yf == y * 64.0)
	{
		DWORD xy = ((x & 0xFFFF) << 16) | (y & 0xFFFF);
		bool bAddToHistory = false;
		if (!m_move_history.empty())
		{
			if (xy != m_move_history.back())
			{
				bAddToHistory = true;
			}
		}
		else
		{
			bAddToHistory = true;
		}

		if (bAddToHistory)
		{
			if (m_move_history.size() > 128)
				m_move_history.pop_front();

			m_move_history.push_back(xy);
		}
	}

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

int __cdecl NewUI_HandleForgetSkillMouseEvent(int index, char flags)
{
	if (g_CGAService.m_ui_forget_skill_index != -1) 
	{
		flags = 2;
		index = g_CGAService.m_ui_forget_skill_index + 0x45;
		g_CGAService.m_ui_forget_skill_index = -1;

		return g_CGAService.UI_HandleForgetSkillMouseEvent(index, flags);
	}

	return g_CGAService.UI_HandleForgetSkillMouseEvent(index, flags);
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

		for (int i = 0; i < 6; ++i)
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

		if (m_work_acceleration)
		{
			*g_work_accelerate = 1;
			g_work_accelerate_percent[0] = m_work_acceleration;
		}

		int result = UI_HandleCraftItemButtonMouseEvent(index, flags);

		if (m_work_acceleration)
		{
			*g_work_accelerate = backup_acc;
			g_work_accelerate_percent[0] = backup_percent;
		}

		return result;
	}

	return UI_HandleCraftItemButtonMouseEvent(index, flags);
}

int __cdecl NewUI_HandleCraftItemButtonMouseEvent(int index, char flags)
{
	return g_CGAService.NewUI_HandleCraftItemButtonMouseEvent(index, flags);
}

int __cdecl NewUI_PlaySwitchAnim(int a1, char a2, float a3)
{
	if (g_CGAService.m_ui_noswitchanim)
		return 1;

	return g_CGAService.UI_PlaySwitchAnim(a1, a2, a3);
}

void CGAService::NewUI_OpenTradeDialog(const char *playerName, int playerLevel)
{
	UI_OpenTradeDialog(playerName, playerLevel);

	auto name = boost::locale::conv::to_utf<char>(playerName, "GBK");

	cga_trade_dialog_t dlg(name, playerLevel);

	CGA_NotifyTradeDialog(dlg);
}

void __cdecl NewUI_OpenTradeDialog(const char *playerName, int playerLevel)
{
	g_CGAService.NewUI_OpenTradeDialog(playerName, playerLevel);
}

void __cdecl NewActor_SetAnimation(void *actor, int anim, int a3)
{
	auto v3 = *(void **)((char *)actor + 12);
	if (v3 == NULL || IsBadReadPtr(v3, 0x38))//crash fix
		return;

	g_CGAService.Actor_SetAnimation(actor, anim, a3);
}

int __cdecl NewIsMapObjectEntrance(int xpos, int ypos)
{
	if(g_CGAService.m_move_checkwarp)
		return g_CGAService.IsMapObjectEntrance(xpos, ypos);

	return 0;
}

void CGAService::NewNET_WritePrepareCraftItemPacket_cgitem(int a1, int a2)
{
	if (m_work_immediate == true)
	{
		*g_craft_done_tick = 0;
	}
	else
	{
		NET_WritePrepareCraftItemPacket_cgitem(a1, a2);
	}
}

void __cdecl NewNET_WritePrepareCraftItemPacket_cgitem(int a1, int a2)
{
	g_CGAService.NewNET_WritePrepareCraftItemPacket_cgitem(a1, a2);
}

int __cdecl NewGetBattleUnitDistance(void *a1, float a2, float a3)
{
	if (g_CGAService.m_ui_noswitchanim)
		return 0;

	return g_CGAService.GetBattleUnitDistance(a1, a2, a3);
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
		/*g_item_menu_players = CONVERT_GAMEVAR(item_menu_player_t *, 0x94C2F8);
		g_item_menu_player_count = CONVERT_GAMEVAR(int *, 0x8E3C38);
		g_item_menu_units = CONVERT_GAMEVAR(item_menu_unit_t *, 0x946870);
		g_item_menu_unit_count = CONVERT_GAMEVAR(int *, 0x94BAC8);
		g_player_menu_clickable = CONVERT_GAMEVAR(char *, 0x8C3909);
		g_unit_menu_clickable = CONVERT_GAMEVAR(char *, 0x8B9187);*/

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
		UI_HandleLogoutMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0x40E0);
		//UI_PlayerMenuSelect = CONVERT_GAMEVAR(int(__cdecl **)(int, int), 0);//TODO
		//UI_UnitMenuSelect = CONVERT_GAMEVAR(int(__cdecl **)(int, int), 0);//TODO
		//UI_PlayerMenuShowup = CONVERT_GAMEVAR(int(__cdecl *)(int), 0x1A190);
		//UI_PlayerMenuMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0);//TODO
		//UI_UnitMenuShowup = CONVERT_GAMEVAR(int(__cdecl *)(int), 0x8B5F0);
		//UI_UnitMenuMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0x8B7C0);
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
		g_npcdlg_item_base = CONVERT_GAMEVAR(npcdlg_item_info_t *, 0x8D3198);//ok
		g_player_name = CONVERT_GAMEVAR(char *, 0xBDB998);//ok
		g_pet_base = CONVERT_GAMEVAR(pet_t *, 0xE109F8);//ok
		g_player_remain_points = CONVERT_GAMEVAR(int *, 0x8E3C50);//ok
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
		g_btl_petskill_allowbit = CONVERT_GAMEVAR(int *, 0x23F9BC);//ok
		g_btl_skill_allowbit = CONVERT_GAMEVAR(int *, 0x23FA28);//ok
		g_btl_weapon_allowbit = CONVERT_GAMEVAR(int *, 0x23FA9C);//ok
		g_btl_player_status = CONVERT_GAMEVAR(int *, 0x23FA44);//ok
		g_btl_petid = CONVERT_GAMEVAR(int *, 0x23F9C0);//ok
		g_btl_action_done = CONVERT_GAMEVAR(int *, 0x23F924);//ok
		g_btl_skill_performed = CONVERT_GAMEVAR(int *, 0x23F9F8);//ok
		g_btl_round_endtick = CONVERT_GAMEVAR(unsigned int *, 0x23F92C);//ok
		g_btl_unit_base = CONVERT_GAMEVAR(char *, 0x235798);//ok

		g_net_socket = CONVERT_GAMEVAR(int *, 0xBD7108);//ok

		g_item_base = CONVERT_GAMEVAR(item_t *, 0x8C70B0);//ok
		g_item_info_base = CONVERT_GAMEVAR(item_info_t *, 0xE12DCC);//ok
		g_trade_item_base = CONVERT_GAMEVAR(item_t *, 0x8BF2C0);//ok

		g_logback = CONVERT_GAMEVAR(int *, 0xB12AA4);//ok
		g_item_cur_using = CONVERT_GAMEVAR(int *, 0x8CEFFC);//ok

		g_trade_item_array = CONVERT_GAMEVAR(char *, 0x8CF6B0);
		g_trade_pet_array = CONVERT_GAMEVAR(char *, 0x8BE8BC);
		g_trade_gold = CONVERT_GAMEVAR(int *, 0x938570);

		g_unit_count = CONVERT_GAMEVAR(int *, 0x23FB0C);//ok
		g_map_units = CONVERT_GAMEVAR(map_unit_t *, 0x240008);//ok

		g_npc_dialog_show = CONVERT_GAMEVAR(int *, 0x8E4A98);//ok
		g_npc_dialog_type = CONVERT_GAMEVAR(int *, 0x2277FC);//ok
		g_npc_dialog_option = CONVERT_GAMEVAR(int *, 0x9386E4);//ok
		g_npc_dialog_dlgid = CONVERT_GAMEVAR(int *, 0x8CF9F0);//ok
		g_npc_dialog_npcid = CONVERT_GAMEVAR(int *, 0x8CF6A0);//ok

		g_work_basedelay = CONVERT_GAMEVAR(int *, 0x8B4190);//ok
		g_disable_move = CONVERT_GAMEVAR(short *, 0x801144);//ok
		g_is_moving = CONVERT_GAMEVAR(int *, 0x8AB2F4);//ok
		g_do_switch_map = CONVERT_GAMEVAR(int *, 0x8AB2F8);//ok
		g_switch_map_frames = CONVERT_GAMEVAR(int *, 0x8AB2FC);//ok
		g_switch_map_type = CONVERT_GAMEVAR(int *, 0x8AB300);//ok

		g_map_x_bottom = CONVERT_GAMEVAR(short *, 0x819ED4);//ok
		g_map_y_bottom = CONVERT_GAMEVAR(short *, 0x819ED0);//ok
		g_map_x_size = CONVERT_GAMEVAR(short *, 0x801134);//ok
		g_map_y_size = CONVERT_GAMEVAR(short *, 0x819EE4);//ok
		g_map_collision_table = CONVERT_GAMEVAR(short *, 0x5D1148);//ok
		g_map_index1 = CONVERT_GAMEVAR(int *, 0x81B344);
		g_map_index2 = CONVERT_GAMEVAR(int *, 0x5D1130);
		g_map_index3 = CONVERT_GAMEVAR(int *, 0x80114C);
		g_map_object_table = CONVERT_GAMEVAR(short *, 0x835370);

		g_healing_skill_index = CONVERT_GAMEVAR(int *, 0x8E481C);//ok
		g_healing_subskill_index = CONVERT_GAMEVAR(int *, 0x8E3C44);//ok
		g_ui_craftdialog_additem_count = CONVERT_GAMEVAR(char *, 0xF12880-0x400000);//ok

		g_heal_player_menu_type = CONVERT_GAMEVAR(int *, 0x8E3C54);//ok
		g_heal_player_menu_select = CONVERT_GAMEVAR(int *, 0x929D48);//ok
		g_heal_menu_type = CONVERT_GAMEVAR(int *, 0x92DA00);//ok
		g_item_player_menu_type = CONVERT_GAMEVAR(short *, 0x92F638);//ok
		g_item_player_select = CONVERT_GAMEVAR(int *, 0x9298B0);//ok

		g_craft_step = CONVERT_GAMEVAR(int *, 0xCBF15E - 0x400000);//ok
		g_work_accelerate = CONVERT_GAMEVAR(char *, 0x63FFFE - 0x400000);//ok;
		g_work_accelerate_percent = CONVERT_GAMEVAR(char *, 0x627E8C - 0x400000);//ok;
		g_craft_done_tick = CONVERT_GAMEVAR(int *, 0xCCF858 - 0x400000);//ok
		g_mutex = CONVERT_GAMEVAR(HANDLE *, 0x5D0D00);//ok
		g_resolution_width = CONVERT_GAMEVAR(int *, 0x2091EC);//ok
		g_resolution_height = CONVERT_GAMEVAR(int *, 0x2091F0);//ok

		g_is_in_team = CONVERT_GAMEVAR(short *, 0xE109F0);//ok
		g_team_flags = CONVERT_GAMEVAR(short *, 0xE12DB2);//ok
		g_team_player_base = CONVERT_GAMEVAR(team_player_t *, 0xCC25B0);//ok
		g_title_table = CONVERT_GAMEVAR(char *, 0xCBFFF0);//ok

		g_enableflags = CONVERT_GAMEVAR(short *, 0xE12DBC);//ok

		Sys_CheckModify = CONVERT_GAMEVAR(char(__cdecl *)(const char *), 0x1BD030);//ok
		COMMON_PlaySound = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int), 0x1B1570);//ok
		BATTLE_PlayerAction = CONVERT_GAMEVAR(void(__cdecl *)(), 0xD83A0);//ok
		NET_ParseTradeItemsPackets = CONVERT_GAMEVAR(void(__cdecl *)(int, const char *), 0x183CD0);//ok
		NET_ParseTradePetPackets = CONVERT_GAMEVAR(void(__cdecl *)(int, int, const char *), 0x183D00);//ok
		NET_ParseTradePetSkillPackets = CONVERT_GAMEVAR(void(__cdecl *)(int, int, const char *), 0x183D30);//ok
		NET_ParseTradeGoldPackets = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0x183CA0);//ok
		NET_ParseTradePlayers = CONVERT_GAMEVAR(void(__cdecl *)(int , const char *, char *), 0x183BE0);//ok
		NET_ParseHealPlayers = CONVERT_GAMEVAR(void (__cdecl *)(int , const char *), 0x183B10);
		NET_ParseHealUnits = CONVERT_GAMEVAR(void(__cdecl *)(int, const char *), 0x183B40);
		NET_ParseItemPlayers = CONVERT_GAMEVAR(void (__cdecl *)(int , const char *), 0x183B70);
		NET_ParseItemUnits = CONVERT_GAMEVAR(void(__cdecl *)(int, const char *), 0x183BA0);
		NET_ParseMeetEnemy = CONVERT_GAMEVAR(void(__cdecl *)(int a1, int a2, int a3), 0x181FB0);

		NET_ParseBattlePackets = CONVERT_GAMEVAR(void(__cdecl *)(int, const char *), 0x1822A0);//ok
		NET_ParseWorkingResult = CONVERT_GAMEVAR(void(__cdecl *)(int , int , int , const char *), 0x181140);//ok
		NET_ParseChatMsg = CONVERT_GAMEVAR(void(__cdecl *)(int, int, const char *, int, int), 0x1814F0);//ok
		NET_ParseSysMsg = CONVERT_GAMEVAR(void(__cdecl *)(int a1, const char *buf), 0x181AA0);//ok
		NET_ParseReadyTrade = CONVERT_GAMEVAR(void(__cdecl *)(), 0x183C70);
		NET_ParseConfirmTrade = CONVERT_GAMEVAR(void(__cdecl *)(int a1, int a2), 0x183DB0);

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
		NET_WriteJoinTeamPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int), 0x1888B0);//ok
		NET_WriteKickTeamPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int), 0x1912E0);//ok
		NET_WriteTradePacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int), 0x188F10);//ok
		//trade arg2=itemlist 8|0|10|1, arg3=petlist 3|0|4|1 arg4=gold
		NET_WriteTraceAddItemPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, const char *, const char *, int), 0x188FB0);//ok
		NET_WriteTradeConfirmPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int), 0x1890A0);//ok
		NET_WriteTradeRefusePacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int), 0x189050);//ok
		NET_WriteExchangeCardPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int), 0x187E90);//ok
		NET_WritePKPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int), 0x187540);//ok
		NET_WriteOpenHealDialog_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0x188AA0);//ok
		NET_WriteMoveItemPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int), 0x187B30);
		NET_WriteRequestDownloadMapPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int , int , int , int), 0x1873D0);
		NET_WritePrepareCraftItemPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0x189E20);
		NET_WriteDropPetPacket_cgitem = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int), 0x187A90);
		
		Move_Player = CONVERT_GAMEVAR(void(__cdecl *)(), 0x98280);//ok
		UI_HandleLogbackMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xD2BF0);//ok
		UI_HandleLogoutMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xD2CD0);//ok
		CL_MoveItemEx = CONVERT_GAMEVAR(void(__cdecl *)(int), 0xC0EC0);//ok
		NPC_ShowDialogInternal = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int, int, const char *), 0xDE390);//ok
		NPC_ClickDialog = CONVERT_GAMEVAR(int(__cdecl *)(int, int, int, char), 0xDEC00);//ok
		NPC_ClickExchangeDialog = CONVERT_GAMEVAR(void(__cdecl *)(int), 0xE2880);//ok
		UI_OpenGatherDialog = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0xBDB30);//ok
		UI_OpenCraftDialog = CONVERT_GAMEVAR(void(__cdecl *)(int, int, int), 0xB5E10);//ok
		UI_OpenAssessDialog = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0xB5FF0);//ok
		UI_HandleMiniDialogMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xE22E0);//ok
		UI_IsMouseInRect = CONVERT_GAMEVAR(int(__cdecl *)(int, int, int, int, int), 0x174160);//ok
		UI_HandleLearnSkillConfirmMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0x111F20);//ok
		UI_HandleForgetSkillMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0x113130);//ok
		UI_HandleEnablePlayerFlagsMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xA9320);//ok
		UI_HandleCraftItemSlotMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xBACA0);//ok
		UI_HandleCraftItemButtonMouseEvent = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xB9BC0);//ok		
		IsMapObjectEntrance = CONVERT_GAMEVAR(decltype(IsMapObjectEntrance), 0x99800);//ok
		UI_PlaySwitchAnim = CONVERT_GAMEVAR(int(__cdecl *)(int, char, float), 0x1A07F0);//ok
		UI_HandleEnableFlags = CONVERT_GAMEVAR(int(__cdecl *)(int, char), 0xA9320);//ok
		UI_SelectHealPlayer = CONVERT_GAMEVAR(int(__cdecl *)(int, const char *), 0xBF800);//ok
		UI_SelectItemPlayer = CONVERT_GAMEVAR(int(__cdecl *)(int, const char *), 0xC5280);//ok
		UI_SelectTradePlayer = CONVERT_GAMEVAR(int(__cdecl *)(int, const char *), 0x1479F0);//ok
		UI_SelectHealUnit = CONVERT_GAMEVAR(int(__cdecl *)(int), 0xBFB50);
		UI_SelectItemUnit = CONVERT_GAMEVAR(int(__cdecl *)(int), 0xC5450);
		UI_SelectTradeAddStuffs = CONVERT_GAMEVAR(int(__cdecl *)(int a1, char a2), 0xFA8F6);
		UI_OpenTradeDialog = CONVERT_GAMEVAR(void(__cdecl *)(const char *, int), 0x148840);
		SYS_ResetWindow = CONVERT_GAMEVAR(void(__cdecl *)(), 0x93E10);
		format_mapname = CONVERT_GAMEVAR(void(__cdecl *)(char *, int , int , int ), 0x95920);
		BuildMapCollisionTable = CONVERT_GAMEVAR(decltype(BuildMapCollisionTable), 0x9E670);
		Actor_SetAnimation = CONVERT_GAMEVAR(void(__cdecl *)(void *, int , int), 0x4FCA0);
		GetBattleUnitDistance = CONVERT_GAMEVAR(int(__cdecl *)(void *a1, float a2, float a3), 0x1A680);
	}

	m_hFont = CreateFontW(16, 0, 0, 0, FW_THIN, false, false, false,
		CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, L"ËÎÌו");

	m_btl_showhpmp_enable = false;
	m_btl_highspeed_enable = false;

	for (int i = 0; i < 20; ++i)
		m_battle_units[i].exist = false;

	m_btl_effect_flags = 0;

	if (IsInGame() && GetWorldStatus() == 10) {
		char *battle_buffer = g_btl_buffers + 4096 * ((*g_btl_buffer_index == 0) ? 3 : (*g_btl_buffer_index - 1));
		ParseBattleUnits(battle_buffer, strlen(battle_buffer));
	}

	m_move_to = 0;
	m_move_to_x = 0;
	m_move_to_y = 0;
	m_move_speed = 100;
	m_move_checkwarp = true;

	m_ui_minidialog_loop = false;
	m_ui_minidialog_loop_index = -1;
	m_ui_minidialog_click_index = -1;
	m_ui_learn_skill_confirm = -1;
	m_ui_forget_skill_index = -1;
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
	m_work_immediate = false;
	m_ui_noswitchanim = false;
	m_player_menu_type = 0;
	m_unit_menu_type = 0;

	DetourTransactionBegin();
	DetourAttach(&(void *&)BATTLE_PlayerAction, ::NewBATTLE_PlayerAction);
	DetourAttach(&(void *&)NET_ParseTradeItemsPackets, ::NewNET_ParseTradeItemsPackets);
	DetourAttach(&(void *&)NET_ParseTradePetPackets, ::NewNET_ParseTradePetPackets);
	DetourAttach(&(void *&)NET_ParseTradePetSkillPackets, ::NewNET_ParseTradePetSkillPackets);
	DetourAttach(&(void *&)NET_ParseTradeGoldPackets, ::NewNET_ParseTradeGoldPackets);
	DetourAttach(&(void *&)NET_ParseTradePlayers, ::NewNET_ParseTradePlayers);
	DetourAttach(&(void *&)NET_ParseHealPlayers, ::NewNET_ParseHealPlayers);
	DetourAttach(&(void *&)NET_ParseHealUnits, ::NewNET_ParseHealUnits);
	DetourAttach(&(void *&)NET_ParseItemPlayers, ::NewNET_ParseItemPlayers);
	DetourAttach(&(void *&)NET_ParseItemUnits, ::NewNET_ParseItemUnits);
	DetourAttach(&(void *&)NET_ParseBattlePackets, ::NewNET_ParseBattlePackets);
	DetourAttach(&(void *&)NET_ParseWorkingResult, ::NewNET_ParseWorkingResult);
	DetourAttach(&(void *&)NET_ParseChatMsg, ::NewNET_ParseChatMsg);
	DetourAttach(&(void *&)NET_ParseSysMsg, ::NewNET_ParseSysMsg);
	DetourAttach(&(void *&)NET_ParseReadyTrade, ::NewNET_ParseReadyTrade);
	DetourAttach(&(void *&)NET_ParseConfirmTrade, ::NewNET_ParseConfirmTrade);
	DetourAttach(&(void *&)NET_ParseMeetEnemy, ::NewNET_ParseMeetEnemy);
	DetourAttach(&(void *&)R_DrawText, NewR_DrawText);
	DetourAttach(&(void *&)Move_Player, ::NewMove_Player);
	DetourAttach(&(void *&)NPC_ShowDialogInternal, ::NewNPC_ShowDialogInternal);
	DetourAttach(&(void *&)Sys_CheckModify, ::NewSys_CheckModify);
	DetourAttach(&(void *&)UI_HandleMiniDialogMouseEvent, ::NewUI_HandleMiniDialogMouseEvent);
	DetourAttach(&(void *&)UI_IsMouseInRect, ::NewUI_IsMouseInRect);
	DetourAttach(&(void *&)UI_HandleLearnSkillConfirmMouseEvent, ::NewUI_HandleLearnSkillConfirmMouseEvent);
	DetourAttach(&(void *&)UI_HandleForgetSkillMouseEvent, ::NewUI_HandleForgetSkillMouseEvent);
	DetourAttach(&(void *&)UI_HandleEnablePlayerFlagsMouseEvent, ::NewUI_HandleEnablePlayerFlagsMouseEvent);
	DetourAttach(&(void *&)UI_HandleCraftItemSlotMouseEvent, ::NewUI_HandleCraftItemSlotMouseEvent);
	DetourAttach(&(void *&)UI_HandleCraftItemButtonMouseEvent, ::NewUI_HandleCraftItemButtonMouseEvent);
	DetourAttach(&(void *&)UI_PlaySwitchAnim, ::NewUI_PlaySwitchAnim);
	DetourAttach(&(void *&)UI_OpenTradeDialog, ::NewUI_OpenTradeDialog);
	DetourAttach(&(void *&)Actor_SetAnimation, ::NewActor_SetAnimation);
	DetourAttach(&(void *&)IsMapObjectEntrance, ::NewIsMapObjectEntrance);
	DetourAttach(&(void *&)NET_WritePrepareCraftItemPacket_cgitem, ::NewNET_WritePrepareCraftItemPacket_cgitem);
	DetourAttach(&(void *&)GetBattleUnitDistance, ::NewGetBattleUnitDistance);
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
	/*DetourTransactionBegin();
	DetourDetach(&(void *&)BATTLE_PlayerAction, ::NewBATTLE_PlayerAction);
	DetourDetach(&(void *&)NET_ParseBattlePackets, ::NewNET_ParseBattlePackets);
	DetourDetach(&(void *&)NET_ParseWorkingResult, ::NewNET_ParseWorkingResult);
	DetourDetach(&(void *&)NET_ParseChatMsg, ::NewNET_ParseChatMsg);
	DetourDetach(&(void *&)NET_ParseSysMsg, ::NewNET_ParseSysMsg);
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
	DetourDetach(&(void *&)UI_PlaySwitchAnim, ::NewUI_PlaySwitchAnim);
	DetourTransactionCommit();*/

	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

const int s_UnitPosToXY[][2] =
{
	{ 480, 370 },{ 480 + 60, 370 - 50 },{ 480 - 60, 370 + 50 },{ 480 + 120, 370 - 100 },{ 480 - 120, 330 + 100 },//370 too large
	{ 420, 327 },{ 420 + 60, 327 - 50 },{ 420 - 60, 327 + 50 },{ 420 + 120, 327 - 100 },{ 420 - 120, 317 + 100 },
	{ 158, 188 },{ 158 + 60, 188 - 50 },{ 158 - 60, 188 + 50 },{ 158 + 120, 188 - 100 },{ 180 - 120, 188 + 100 },//158 too small
	{ 210, 240 },{ 210 + 60, 240 - 50 },{ 210 - 60, 240 + 50 },{ 210 + 120, 240 - 100 },{ 210 - 120, 240 + 100 },
};

void CGAService::DrawCustomText()
{
	if (m_btl_showhpmp_enable)
	{
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

	if (m_btl_showhpmp_enable)
	{
		if (GetWorldStatus() == 9 && GetGameStatus() == 3)
		{
			LPDIRECTDRAWSURFACE pSurface = GetDirectDrawBackSurface();
			HDC hDC;
			if (S_OK == pSurface->GetDC(&hDC))
			{
				SetBkMode(hDC, TRANSPARENT);
				SelectObject(hDC, g_CGAService.m_hFont);

				int x = 640-64;
				int y = 480-64;

				char buf[32];
				sprintf(buf, "(%d, %d)", *g_move_xdest, *g_move_ydest);
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

				pSurface->ReleaseDC(hDC);
			}
		}
	}
}

void CGAService::ParseBattleUnits(const char *buf, size_t len)
{
	for (int i = 0; i < 20; ++i)
		m_battle_units[i].exist = false;

	m_btl_effect_flags = NET_ParseHexInteger(buf, '|', 2);

	int position = 3;

	while (1)
	{
		int pos = NET_ParseHexInteger(buf, '|', position);

		if (pos < 0 || pos > 19)
			break;

		NET_ParseDelimeter(buf, '|', position + 1, 31, m_battle_units[pos].name);
		
		m_battle_units[pos].pos = pos;
		m_battle_units[pos].modelid = NET_ParseHexInteger(buf, '|', position + 2);
		m_battle_units[pos].level = NET_ParseHexInteger(buf, '|', position + 4);
		m_battle_units[pos].curhp = NET_ParseHexInteger(buf, '|', position + 5);
		m_battle_units[pos].maxhp = NET_ParseHexInteger(buf, '|', position + 6);
		m_battle_units[pos].curmp = NET_ParseHexInteger(buf, '|', position + 7);
		m_battle_units[pos].maxmp = NET_ParseHexInteger(buf, '|', position + 8);
		m_battle_units[pos].flags = NET_ParseHexInteger(buf, '|', position + 9);
		m_battle_units[pos].exist = true;
		position += 12;
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
	info->souls = (*g_playerBase)->souls;
	info->level = (*g_playerBase)->level;
	info->gold = (*g_playerBase)->gold;
	info->unitid = (*g_playerBase)->unitid;
	info->punchclock = (*g_playerBase)->punchclock;
	info->usingpunchclock = (*g_playerBase)->using_punchclock;
	info->petid = *g_pet_id;
	info->name = boost::locale::conv::to_utf<char>(g_player_name, "GBK");
	info->job = boost::locale::conv::to_utf<char>(g_job_name, "GBK");
	for (size_t i = 0; i < 96; ++i)
	{
		if (*(g_title_table + 28 * i) != 0)
			info->titles.emplace_back(boost::locale::conv::to_utf<char>((g_title_table + 28 * i), "GBK"));
		else
			break;
	}
	info->detail.points_remain = *g_player_remain_points;
	info->detail.points_endurance = (*g_playerBase)->points_endurance;
	info->detail.points_strength = (*g_playerBase)->points_strength;
	info->detail.points_defense = (*g_playerBase)->points_defense;
	info->detail.points_agility = (*g_playerBase)->points_agility;
	info->detail.points_magical = (*g_playerBase)->points_magical;
	info->detail.value_attack = (*g_playerBase)->value_attack;
	info->detail.value_defensive = (*g_playerBase)->value_defensive;
	info->detail.value_agility = (*g_playerBase)->value_agility;
	info->detail.value_spirit = (*g_playerBase)->value_spirit;
	info->detail.value_recovery = (*g_playerBase)->value_recovery;
	info->detail.resist_poison = (*g_playerBase)->resist_poison;
	info->detail.resist_sleep = (*g_playerBase)->resist_sleep;
	info->detail.resist_medusa = (*g_playerBase)->resist_medusa;
	info->detail.resist_drunk = (*g_playerBase)->resist_drunk;
	info->detail.resist_chaos = (*g_playerBase)->resist_chaos;
	info->detail.resist_forget = (*g_playerBase)->resist_forget;
	info->detail.fix_critical = (*g_playerBase)->fix_critical;
	info->detail.fix_strikeback = (*g_playerBase)->fix_strikeback;
	info->detail.fix_accurancy = (*g_playerBase)->fix_accurancy;
	info->detail.element_earth = (*g_playerBase)->element_earth;
	info->detail.element_water = (*g_playerBase)->element_water;
	info->detail.element_fire = (*g_playerBase)->element_fire;
	info->detail.element_wind = (*g_playerBase)->element_wind;

	info->manu_endurance = (*g_playerBase)->manu_endurance;
	info->manu_skillful = (*g_playerBase)->manu_skillful;
	info->manu_intelligence = (*g_playerBase)->manu_intelligence;
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
	if (skillid < 0 || skillid >= 15)
		return false;

	return g_skill_base[skillid].name[0] ? true : false;
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
		info->slotsize = g_skill_base[index].slotsize;
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

	for (int i = 0; i < 15; ++i)
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
				i,
				g_skill_base[i].slotsize
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
		info->available = g_skill_base[index].sub[stage].available ? true : false;
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
					g_skill_base[index].sub[i].level,
					g_skill_base[index].sub[i].available ? true : false
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
		info->loyality = g_pet_base[index].value_loyality;
		info->battle_flags = g_pet_base[index].battle_flags;
		info->index = index;
		info->name = boost::locale::conv::to_utf<char>(g_pet_base[index].name, "GBK");
		info->realname = boost::locale::conv::to_utf<char>(g_pet_base[index].realname, "GBK");

		info->detail.points_remain = g_pet_base[index].points_remain;
		info->detail.points_endurance = g_pet_base[index].points_endurance;
		info->detail.points_strength = g_pet_base[index].points_strength;
		info->detail.points_defense = g_pet_base[index].points_defense;
		info->detail.points_agility = g_pet_base[index].points_agility;
		info->detail.points_magical = g_pet_base[index].points_magical;
		info->detail.value_attack = g_pet_base[index].value_attack;
		info->detail.value_defensive = g_pet_base[index].value_defensive;
		info->detail.value_agility = g_pet_base[index].value_agility;
		info->detail.value_spirit = g_pet_base[index].value_spirit;
		info->detail.value_recovery = g_pet_base[index].value_recovery;
		info->detail.resist_poison = g_pet_base[index].resist_poison;
		info->detail.resist_sleep = g_pet_base[index].resist_sleep;
		info->detail.resist_medusa = g_pet_base[index].resist_medusa;
		info->detail.resist_drunk = g_pet_base[index].resist_drunk;
		info->detail.resist_chaos = g_pet_base[index].resist_chaos;
		info->detail.resist_forget = g_pet_base[index].resist_forget;
		info->detail.fix_critical = g_pet_base[index].fix_critical;
		info->detail.fix_strikeback = g_pet_base[index].fix_strikeback;
		info->detail.fix_accurancy = g_pet_base[index].fix_accurancy;
		info->detail.fix_dodge = g_pet_base[index].fix_dodge;
		info->detail.element_earth = g_pet_base[index].element_earth;
		info->detail.element_water = g_pet_base[index].element_water;
		info->detail.element_fire = g_pet_base[index].element_fire;
		info->detail.element_wind = g_pet_base[index].element_wind;
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
				g_pet_base[i].value_loyality,
				g_pet_base[i].battle_flags,
				i,
				boost::locale::conv::to_utf<char>(g_pet_base[i].name, "GBK"),
				boost::locale::conv::to_utf<char>(g_pet_base[i].realname, "GBK"),
				cga_playerpet_detail_info_t(
					g_pet_base[i].points_remain,
					g_pet_base[i].points_endurance,
					g_pet_base[i].points_strength,
					g_pet_base[i].points_defense,
					g_pet_base[i].points_agility,
					g_pet_base[i].points_magical,
					g_pet_base[i].value_attack,
					g_pet_base[i].value_defensive,
					g_pet_base[i].value_agility,
					g_pet_base[i].value_spirit,
					g_pet_base[i].value_recovery,
					g_pet_base[i].resist_poison,
					g_pet_base[i].resist_sleep,
					g_pet_base[i].resist_medusa,
					g_pet_base[i].resist_drunk,
					g_pet_base[i].resist_chaos,
					g_pet_base[i].resist_forget,
					g_pet_base[i].fix_critical,
					g_pet_base[i].fix_strikeback,
					g_pet_base[i].fix_accurancy,
					g_pet_base[i].fix_dodge,
					g_pet_base[i].element_earth,
					g_pet_base[i].element_water,
					g_pet_base[i].element_fire,
					g_pet_base[i].element_wind
				)
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
			//info->attr = boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].attr, "GBK");
			//info->attr2 = boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].attr2, "GBK");
			std::string attrs;
			for (int j = 0; j < 8; ++j) {
				if ((*g_playerBase)->iteminfos[itempos].attrs[j][0])
				{
					if (j != 0)
						attrs += "\n";
					attrs += boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].attrs[j], "GBK");
				}
			}
			info->attr = attrs;
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
			std::string attrs;
			for (int j = 0; j < 8; ++j) {
				if ((*g_playerBase)->iteminfos[itempos].attrs[j][0])
				{
					if (j != 0)
						attrs += "\n";
					attrs += boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].attrs[j], "GBK");
				}
			}
			info->emplace_back(
				boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].name, "GBK"),
				attrs,
				std::string(),//boost::locale::conv::to_utf<char>((*g_playerBase)->iteminfos[itempos].attr, "GBK"),
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

	if (GetWorldStatus() != 9 || GetGameStatus() != 3)
		return false;

	if (*g_move_xspeed != 0 || *g_move_yspeed != 0)
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

bool CGAService::WM_DropPet(int petpos)
{
	if (!IsInGame())
		return false;

	if (GetWorldStatus() != 9 || GetGameStatus() != 3)
		return false;

	if (*g_move_xspeed != 0 || *g_move_yspeed != 0)
		return false;

	if (!IsPetValid(petpos))
		return false;

	if (m_game_type == cg_item_6000)
		NET_WriteDropPetPacket_cgitem(*g_net_socket, g_player_xpos->decode(), g_player_ypos->decode(), petpos);
	//else
		//NET_WritePacket1(*g_net_buffer, *g_net_socket, net_header_dropitem, g_player_xpos->decode(), g_player_ypos->decode(), itempos);

	return true;
}

bool CGAService::DropPet(int petpos)
{
	return SendMessageA(g_MainHwnd, WM_CGA_DROP_PET, petpos, 0) ? true : false;
}

bool CGAService::WM_MoveItem(cga_move_item_t *mov)
{
	if (!IsInGame())
		return false;

	if (GetWorldStatus() != 9 || GetGameStatus() != 3)
		return false;

	if (*g_move_xspeed != 0 || *g_move_yspeed != 0)
		return false;

	if (!IsItemValid(mov->itempos))
		return false;

	if (mov->itempos >= 100 || mov->dstpos >= 100)
	{
		char buffer[64];
		sprintf(buffer, "I\\z%d\\z%d\\z%d", mov->itempos, mov->dstpos, mov->count);

		if (m_game_type == cg_item_6000)
			NET_WritePacket3_cgitem(*g_net_socket,
				g_player_xpos->decode(), g_player_ypos->decode(),
				*g_npc_dialog_dlgid, *g_npc_dialog_npcid, 0, buffer);
		//else
			//NET_WritePacket1(*g_net_buffer, *g_net_socket, net_header_dropitem, g_player_xpos->decode(), g_player_ypos->decode(), itempos);
	}
	else
	{
		NET_WriteMoveItemPacket_cgitem(*g_net_socket, mov->itempos, mov->dstpos, mov->count);
	}

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

std::tuple<int, int, int> CGAService::GetMapIndex()
{
	return std::tuple<int, int, int>(*g_map_index1, *g_map_index2, *g_map_index3);
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

void CGAService::WM_GetMapName(std::string *name)
{
	try
	{
		*name = boost::locale::conv::to_utf<char>(g_map_name, "GBK");
	}
	catch (std::exception &e)
	{
		*name = "";
	}
}

std::string CGAService::GetMapName()
{
	std::string name;

	/*
	SendMessageA(g_MainHwnd, WM_CGA_GET_MAP_NAME, (WPARAM)&name, 0);*/

	char mapname[32];

	strncpy(mapname, g_map_name, 31);
	mapname[31] = 0;

	try
	{
		name = boost::locale::conv::to_utf<char>(mapname, "GBK");
	}
	catch (std::exception &e)
	{
		name = "";
	}

	return name;
}

cga_map_units_t CGAService::GetMapUnits()
{
	cga_map_units_t units;

	SendMessageA(g_MainHwnd, WM_CGA_GET_MAP_UNITS, (WPARAM)&units, 0);

	return units;
}

void CGAService::WalkTo(int x, int y)
{
	if (g_player_xpos->decode() == x && g_player_ypos->decode() == y)
		return;

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

void CGAService::WM_GetMoveHistory(std::vector<DWORD> *v)
{
	v->reserve(m_move_history.size());
	for (auto itor = m_move_history.rbegin(); itor != m_move_history.rend(); ++itor)
	{
		v->emplace_back(*itor);
	}
}

std::vector<DWORD> CGAService::GetMoveHistory(void)
{
	std::vector<DWORD> v;
	SendMessageA(g_MainHwnd, WM_CGA_GET_MOVE_HISTORY, (WPARAM)&v, NULL);
	return v;
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
		u->flags = m_battle_units[pos].flags;
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
				m_battle_units[i].flags,
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

cga_battle_context_t CGAService::GetBattleContext()
{
	cga_battle_context_t ctx(
		*g_btl_round_count,
		*g_btl_player_pos,
		*g_btl_player_status,
		*g_btl_skill_performed,
		*g_btl_skill_allowbit,
		*g_btl_petskill_allowbit,
		*g_btl_weapon_allowbit,
		*g_btl_petid,
		m_btl_effect_flags
		);

	return ctx;
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

	WriteLog("BattleNormalAttack %d\n", target);

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
	if (!g_skill_base[skillpos].name[0])
	{
		WriteLog("skill %d not valid\n", skillpos);
		return false;
	}
	if (!(*g_btl_skill_allowbit & (1 << skillpos)))//skill not allowed!!!
	{
		WriteLog("skill %d not allowed\n", skillpos);
		return false;
	}
	if (g_skill_base[skillpos].level < skilllv)
	{
		WriteLog("skill lv%d not enough\n", skilllv);
		return false;
	}

	WriteLog("skill %d %d\n", skillpos, skilllv);

	if (g_skill_base[skillpos].sub[skilllv].available == 0)
	{
		WriteLog("skill lv%d not available, downgrade\n", skilllv);
		bool available = false;
		for (int lv = skilllv - 1; lv >= 0; --lv)
		{
			if (g_skill_base[skillpos].sub[lv].available == 1)
			{
				skilllv = lv;
				available = true;
				break;
			}
		}

		if (!available)
		{
			WriteLog("skill lv%d not available\n", skilllv);
			return false;
		}
	}

	if (g_skill_base[skillpos].sub[skilllv].cost > m_battle_units[*g_btl_player_pos].curmp)
	{
		bool available = false;
		for (int lv = skilllv - 1; lv >= 0; --lv)
		{
			if (g_skill_base[skillpos].sub[lv].cost <= m_battle_units[*g_btl_player_pos].curmp)
			{
				skilllv = lv;
				available = true;
				break;
			}
		}

		if (!available)
		{
			WriteLog("skill lv%d no enough mp\n", skilllv);
			return false;
		}
	}

	WriteLog("fixed skill %d %d\n", skillpos, skilllv);

	char buf[32];
	sprintf(buf, "S|%X|%X|%X", skillpos, skilllv, target);
	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, buf);
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, buf);
	*g_btl_action_done = 1;
	COMMON_PlaySound(57, 320, 240);

	WriteLog("BattleSkillAttack %d %d %d\n", skillpos, skilllv, target);

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

	if (skillpos == -1 || skillpos == 0xFF)
	{
		char buf[32];
		sprintf(buf, "W|FF");
		if (m_game_type == cg_item_6000)
			NET_WriteBattlePacket_cgitem(*g_net_socket, buf);
		else
			NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, buf);
		*g_btl_action_done = 1;
		COMMON_PlaySound(57, 320, 240);

		WriteLog("BattlePetSkillAttack FF\n");
		return true;
	}

	if (!(*g_btl_petskill_allowbit & (1 << skillpos)))//skill not allowed!!!
	{
		WriteLog("pet skill %d not allowed\n", skillpos);
		return false;
	}
	//if (g_pet_base[*g_btl_petid].skills[skillpos].cost > m_battle_units[*g_btl_player_pos].curmp)
	//	return false;
	char buf[32];
	sprintf(buf, "W|%X|%X", skillpos, target);
	if (m_game_type == cg_item_6000)
		NET_WriteBattlePacket_cgitem(*g_net_socket, buf);
	else
		NET_WritePacket(*g_net_buffer, *g_net_socket, net_header_battle, buf);
	*g_btl_action_done = 1;
	COMMON_PlaySound(57, 320, 240);

	WriteLog("BattlePetSkillAttack %d %d\n", skillpos, target);

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

	if (*g_world_status_cgitem == 10)
	{
		UI_HandleLogbackMouseEvent(8, 2);
	}
	else
	{
		if (m_game_type == cg_item_6000)
			NET_WriteLogbackPacket_cgitem(*g_net_socket);
		else if (m_game_type == cg_se_3000)
			NET_WriteLogbackPacket_cgse(*g_net_buffer, *g_net_socket, net_header_logback);

		COMMON_PlaySound(60, 320, 240);
		*g_logback = 1;
	}
}

void CGAService::LogOut()
{
	SendMessage(g_MainHwnd, WM_CGA_LOG_OUT, 0, 0);
}

void CGAService::WM_LogOut()
{
	if (!IsInGame())
		return;

	UI_HandleLogoutMouseEvent(9, 2);
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

	if (*g_npc_dialog_type == 18)
	{
		if (option == 0)
		{
			m_ui_forget_skill_index = index;
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

	if (*g_npc_dialog_type != 7 && *g_npc_dialog_type != 6 && *g_npc_dialog_type != 22 && *g_npc_dialog_type != 20)
	{
		//WriteLog("type mismatch %d\n", *g_npc_dialog_type);
		return false;
	}

	if (*g_npc_dialog_type == 22 || *g_npc_dialog_type == 20) 
	{
		for (size_t i = 0; i < items->size(); ++i)
		{
			if (items->at(i).itemid != 0 && (*g_playerBase)->iteminfos[items->at(i).itempos].item_id != items->at(i).itemid)
			{
				//WriteLog("id mismatch %d %d\n", items->at(i).itemid, (*g_playerBase)->iteminfos[items->at(i).itempos].item_id);
				return false;
			}
			bool allow = false;
			for (size_t j = 0; j < 20; ++j)
			{
				if (g_npcdlg_item_base[j].valid && g_npcdlg_item_base[j].itempos == items->at(i).itempos)
				{
					//WriteLog("j=%d g_npcdlg_item_base[j].itempos=%d\n", j, g_npcdlg_item_base[j].itempos);
					allow = true;
					break;
				}
			}
			if (allow)
			{
				sprintf(buf, "%d", items->at(i).itempos);
				buffer += buf;
				if (i != items->size() - 1)
					buffer += "\\z";
			}
		}
	}
	else
	{
		for (size_t i = 0; i < items->size(); ++i)
		{
			if (items->at(i).itemid != 0 && (*g_playerBase)->iteminfos[items->at(i).itempos].item_id != items->at(i).itemid)
			{
				//WriteLog("id mismatch %d %d %d\n", items->at(i).itemid, (*g_playerBase)->iteminfos[items->at(i).itempos].item_id, items->at(i).itemid);
				return false;
			}
			if ((*g_playerBase)->iteminfos[items->at(i).itempos].count != 0 && (*g_playerBase)->iteminfos[items->at(i).itempos].count < items->at(i).count) {
				//WriteLog("count mismatch %d %d\n", (*g_playerBase)->iteminfos[items->at(i).itempos].count, items->at(i).count);
				return false;
			}
			sprintf(buf, "%d\\z%d", items->at(i).itempos, items->at(i).count);
			buffer += buf;
			if (i != items->size() - 1)
				buffer += "\\z";
		}
	}

	if (buffer.empty())
		return false;

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

bool CGAService::WM_PlayerMenuSelect(int menuindex, const char *menustring)
{
	if (m_player_menu_type == PLAYER_MENU_HEAL)
	{
		m_player_menu_type = 0;
		return UI_SelectHealPlayer(menuindex, menustring) ? true : false;
	}
	else if (m_player_menu_type == PLAYER_MENU_ITEM)
	{
		m_player_menu_type = 0;
		return UI_SelectItemPlayer(menuindex, menustring) ? true : false;
	}
	else if (m_player_menu_type == PLAYER_MENU_TRADE)
	{
		m_player_menu_type = 0;
		return UI_SelectTradePlayer(menuindex, menustring) ? true : false;
	}
	return false;
}

bool CGAService::PlayerMenuSelect(int menuindex, const std::string &menustring)
{
	return SendMessageA(g_MainHwnd, WM_CGA_PLAYER_MENU_SELECT, (WPARAM)menuindex, (LPARAM)menustring.c_str()) ? true : false;
}

bool CGAService::WM_UnitMenuSelect(int menuindex)
{
	if (m_unit_menu_type == UNIT_MENU_HEAL)
	{
		m_unit_menu_type = 0;
		return UI_SelectHealUnit(menuindex) ? true : false;
	}
	else if (m_unit_menu_type == UNIT_MENU_ITEM)
	{
		m_unit_menu_type = 0;
		return UI_SelectItemUnit(menuindex) ? true : false;
	}
	return false;
}

bool CGAService::UnitMenuSelect(int menuindex)
{
	return SendMessageA(g_MainHwnd, WM_CGA_UNIT_MENU_SELECT, (WPARAM)menuindex, 0) ? true : false;
}

inline void GetActorPosition(void *actor, int &x, int &y)
{
	x = *(int *)((char *)actor + 4 * (*(unsigned short *)((char *)actor + 624)) + 544);
	y = *(int *)((char *)actor + 4 * (*(unsigned short *)((char *)actor + 624)) + 584);
}

void CGAService::WM_GetMapUnits(cga_map_units_t *units)
{
	if (!IsInGame())
		return;

	for (int i = 0; i < *g_unit_count; ++i)
	{
		if (g_map_units[i].valid == 0)
			continue;

		int xpos = g_map_units[i].xpos.decode(), ypos = g_map_units[i].ypos.decode();
		
		if (g_map_units[i].actor && g_map_units[i].type == 8)
		{
			int x, y;
			GetActorPosition(g_map_units[i].actor, x, y);
			if (x != 0 && y != 0)
			{
				xpos = x;
				ypos = y;
			}
		}

		units->emplace_back(
			g_map_units[i].valid,
			g_map_units[i].type,
			g_map_units[i].unit_id,
			g_map_units[i].model_id,
			xpos,
			ypos,
			g_map_units[i].item_count,
			g_map_units[i].injury,
			g_map_units[i].level,
			g_map_units[i].flags,
			boost::locale::conv::to_utf<char>(g_map_units[i].unit_name, "GBK"),
			boost::locale::conv::to_utf<char>(g_map_units[i].nick_name, "GBK"),
			boost::locale::conv::to_utf<char>(g_map_units[i].title_name, "GBK"),
			boost::locale::conv::to_utf<char>(g_map_units[i].item_name, "GBK")
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
	if (percent > 0)
	{
		m_work_acceleration = percent;
	}
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
	for (int i = 0; i < 5; ++i)
	{
		m_ui_craftdialog_additem[i] = -1;
		if (!skill_craft.mats[i].name[0])
			continue;
		bool found_this_mat = false;
		int desireditemid = skill_craft.mats[i].itemid;
		int desiredcount = skill_craft.mats[i].count;
		for (size_t j = 0; j < 5; ++j)
		{
			int itempos = craft->itempos[j];
			if (itempos >= 8 && IsItemValid(itempos))
			{
				if (desireditemid == (*g_playerBase)->iteminfos[itempos].item_id)
				{
					int itemcount = (*g_playerBase)->iteminfos[itempos].count;
					if (!itemcount)
						itemcount = 1;
					if (itemcount >= desiredcount)
					{
						m_ui_craftdialog_additem[m_ui_craftdialog_additemcount] = itempos - 8;
						++m_ui_craftdialog_additemcount;
						found_this_mat = true;
						break;
					}
				}
			}
		}
		if (!found_this_mat)
			return false;
	}

	int itempos = craft->itempos[5];
	if (itempos >= 8 && IsItemValid(itempos))
	{
		m_ui_craftdialog_additem[m_ui_craftdialog_additemcount] = itempos - 8;
		++m_ui_craftdialog_additemcount;
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
	int offsetX = x - *g_map_x_bottom;
	int offsetY = y - *g_map_y_bottom;

	return (offsetX >= 0 && offsetX < *g_map_x_size && offsetY >= 0 && offsetY < *g_map_y_size && (g_map_collision_table[offsetX + offsetY * (*g_map_x_size)] != 1)) ? true : false;

	//return SendMessageA(g_MainHwnd, WM_CGA_IS_MAP_CELL_PASSABLE, x, y) ? true : false;
}

void CGAService::WM_GetMapCollisionTable(bool loadall, cga_map_cells_t *cells)
{
	if (loadall)
	{
		char mapname[1024];
		format_mapname(mapname, *g_map_index1, *g_map_index2, *g_map_index3);
		auto fp = fopen(mapname, "rb");
		if (fp)
		{
			fseek(fp, 12, SEEK_SET);
			int xsize = 0, ysize = 0;
			fread(&xsize, 4, 1, fp);
			fread(&ysize, 4, 1, fp);

			if (xsize > 0 && ysize > 0 && xsize < 10000 && ysize < 10000)
			{
				short *celldata = (short *)malloc(2 * xsize * ysize);
				short *collisiondata = (short *)malloc(2 * xsize * ysize);
				short *objectdata = (short *)malloc(2 * xsize * ysize);
				int bufferSize = std::max(2*51*51, 2 * xsize * ysize);
				short *collisiontable = (short *)malloc(bufferSize);

				memset(collisiontable, 0, bufferSize);

				fseek(fp, 20, SEEK_SET);
				fread(celldata, 2 * xsize * ysize, 1, fp);

				fseek(fp, 20 + 2 * xsize * ysize, SEEK_SET);
				fread(collisiondata, 2 * xsize * ysize, 1, fp);
	
				fseek(fp, 20 + 2 * xsize * ysize + 2 * xsize * ysize, SEEK_SET);
				fread(objectdata, 2 * xsize * ysize, 1, fp);
				
				BuildMapCollisionTable(0, 0, xsize, ysize, celldata, collisiondata, objectdata, collisiontable);

				cells->x_bottom = 0;
				cells->y_bottom = 0;
				cells->x_size = xsize;
				cells->y_size = ysize;

				cells->cell.resize(xsize * ysize);
				memcpy(cells->cell.data(), collisiontable, 2 * xsize * ysize);

				free(celldata);
				free(collisiondata);
				free(objectdata);
				free(collisiontable);

				fclose(fp);

				return;
			}

			fclose(fp);
		}
	}

	cells->x_bottom = *g_map_x_bottom;
	cells->y_bottom = *g_map_y_bottom;
	cells->x_size = *g_map_x_size;
	cells->y_size = *g_map_y_size;
	cells->cell.resize(cells->x_size*cells->y_size);
	memcpy(cells->cell.data(), g_map_collision_table, 2 * cells->x_size*cells->y_size);
}

cga_map_cells_t CGAService::GetMapCollisionTable(bool loadall)
{
	cga_map_cells_t cells;

	SendMessageA(g_MainHwnd, WM_CGA_GET_COLLISION_TABLE, loadall ? 1 : 0, (LPARAM)&cells);

	return cells;
}

void CGAService::WM_GetMapObjectTable(bool loadall, cga_map_cells_t *cells)
{
	if (loadall)
	{
		char mapname[1024];
		format_mapname(mapname, *g_map_index1, *g_map_index2, *g_map_index3);
		auto fp = fopen(mapname, "rb");
		if (fp)
		{
			fseek(fp, 12, SEEK_SET);
			int xsize = 0, ysize = 0;
			fread(&xsize, 4, 1, fp);
			fread(&ysize, 4, 1, fp);

			if (xsize > 0 && ysize > 0 && xsize < 10000 && ysize < 10000)
			{
				fseek(fp, 20 + 2 * xsize * ysize + 2 * xsize * ysize, SEEK_SET);
				
				int bufferSize = std::max(2 * 51 * 51, 2 * xsize * ysize);

				short *buffer = (short *)malloc(bufferSize);
				memset(buffer, 0, bufferSize);

				fread(buffer, 2 * xsize * ysize, 1, fp);

				cells->x_bottom = 0;
				cells->y_bottom = 0;
				cells->x_size = xsize;
				cells->y_size = ysize;

				cells->cell.resize(xsize * ysize);
				memcpy(cells->cell.data(), buffer, 2 * xsize * ysize);

				free(buffer);

				fclose(fp);

				return;
			}

			fclose(fp);
		}
	}

	cells->x_bottom = *g_map_x_bottom;
	cells->y_bottom = *g_map_y_bottom;
	cells->x_size = *g_map_x_size;
	cells->y_size = *g_map_y_size;
	cells->cell.resize(cells->x_size*cells->y_size);
	memcpy(cells->cell.data(), g_map_object_table, 2 * cells->x_size*cells->y_size);
}

cga_map_cells_t CGAService::GetMapObjectTable(bool loadall)
{
	cga_map_cells_t cells;

	SendMessageA(g_MainHwnd, WM_CGA_GET_OBJECT_TABLE, loadall ? 1 : 0, (LPARAM)&cells);

	return cells;
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

bool CGAService::DoRequest(int request_type)
{
	return SendMessageA(g_MainHwnd, WM_CGA_DO_REQUEST, request_type, 0) ? true : false;
}

bool CGAService::WM_DoRequest(int request_type)
{
	if (!IsInGame())
		return false;

	if (GetWorldStatus() != 9 || GetGameStatus() != 3)
		return false;

	if (request_type == REQUEST_TYPE_JOINTEAM) {
		if (*g_is_in_team)
			return false;

		NET_WriteJoinTeamPacket_cgitem(*g_net_socket, g_player_xpos->decode(), g_player_ypos->decode(), 1);
	}
	else if (request_type == REQUEST_TYPE_LEAVETEAM) {
		if (!(*g_is_in_team))
			return false;
		//if(!(*g_team_flags & 100))
		//	return false;

		NET_WriteJoinTeamPacket_cgitem(*g_net_socket, g_player_xpos->decode(), g_player_ypos->decode(), 0);
	}
	else if (request_type == REQUEST_TYPE_KICKTEAM) {
		if (!(*g_is_in_team))
			return false;

		NET_WriteKickTeamPacket_cgitem(*g_net_socket);
	}
	else if (request_type == REQUEST_TYPE_TRADE) {
		NET_WriteTradePacket_cgitem(*g_net_socket);
	}
	else if (request_type == REQUEST_TYPE_EXCAHNGECARD) {
		NET_WriteExchangeCardPacket_cgitem(*g_net_socket, g_player_xpos->decode(), g_player_ypos->decode());
	}
	else if (request_type == REQUEST_TYPE_PK) {
		NET_WritePKPacket_cgitem(*g_net_socket, g_player_xpos->decode(), g_player_ypos->decode());
	}
	else if (request_type == REQUEST_TYPE_TRADE_CONFIRM) {
		NET_WriteTradeConfirmPacket_cgitem(*g_net_socket);
	}
	else if (request_type == REQUEST_TYPE_TRADE_REFUSE) {
		NET_WriteTradeRefusePacket_cgitem(*g_net_socket);
	}
	else {
		return false;
	}

	return true;
}

bool CGAService::EnableFlags(int type, bool enable)
{
	return SendMessageA(g_MainHwnd, WM_CGA_ENABLE_FLAGS, type, enable ? 1 : 0) ? true : false;
}

bool CGAService::WM_EnableFlags(int type, bool enable)
{
	if (!IsInGame())
		return false;

	if (GetWorldStatus() != 9 || GetGameStatus() != 3)
		return false;

	if (type == ENABLE_FLAG_JOINTEAM) {
		if(((*g_enableflags) & PLAYER_ENABLE_FLAGS_JOINTEAM) && !enable)
			UI_HandleEnableFlags(19, 2);
		else if (!((*g_enableflags) & PLAYER_ENABLE_FLAGS_JOINTEAM) && enable)
			UI_HandleEnableFlags(19, 2);
	}
	else if (type == ENABLE_FLAG_TRADE) {
		if (((*g_enableflags) & PLAYER_ENABLE_FLAGS_TRADE) && !enable)
			UI_HandleEnableFlags(21, 2);
		else if (!((*g_enableflags) & PLAYER_ENABLE_FLAGS_TRADE) && enable)
			UI_HandleEnableFlags(21, 2);
	}
	else if (type == ENABLE_FLAG_CARD) {
		if (((*g_enableflags) & PLAYER_ENABLE_FLAGS_CARD) && !enable)
			UI_HandleEnableFlags(20, 2);
		else if (!((*g_enableflags) & PLAYER_ENABLE_FLAGS_CARD) && enable)
			UI_HandleEnableFlags(20, 2);
	}
	else if (type == ENABLE_FLAG_PK) {
		if (((*g_enableflags) & PLAYER_ENABLE_FLAGS_PK) && !enable)
			UI_HandleEnableFlags(17, 2);
		else if (!((*g_enableflags) & PLAYER_ENABLE_FLAGS_PK) && enable)
			UI_HandleEnableFlags(17, 2);
	}
	else if (type == ENABLE_FLAG_TEAMCHAT) {
		if (((*g_enableflags) & PLAYER_ENABLE_FLAGS_TEAMCHAT) && !enable)
			UI_HandleEnableFlags(18, 2);
		else if (!((*g_enableflags) & PLAYER_ENABLE_FLAGS_TEAMCHAT) && enable)
			UI_HandleEnableFlags(18, 2);
	}
	else if (type == ENABLE_FLAG_FAMILY) {
		if (((*g_enableflags) & PLAYER_ENABLE_FLAGS_FAMILY) && !enable)
			UI_HandleEnableFlags(22, 2);
		else if (!((*g_enableflags) & PLAYER_ENABLE_FLAGS_FAMILY) && enable)
			UI_HandleEnableFlags(22, 2);
	}
	else {
		return false;
	}

	return true;
}

void CGAService::TradeAddStuffs(cga_sell_items_t items, cga_sell_pets_t pets, int gold)
{
	cga_trade_stuff_t stuff;

	stuff.items = items;
	stuff.pets = pets;
	stuff.gold = gold;

	SendMessageA(g_MainHwnd, WM_CGA_TRADE_ADD_STUFFS, (WPARAM)&stuff, 0);
}

void CGAService::WM_TradeAddStuffs(cga_trade_stuff_t *stuffs)
{
	std::string itembuffer, petbuffer;
	char buf[32];

	for (size_t i = 0; i < stuffs->items.size(); ++i)
	{
		sprintf(buf, "%d|%d", stuffs->items.at(i).itempos, i);
		itembuffer += buf;
		if (i != stuffs->items.size() - 1)
			itembuffer += "|";
	}

	for (size_t i = 0; i < stuffs->pets.size(); ++i)
	{
		sprintf(buf, "%d|%d", stuffs->pets.at(i), i);
		petbuffer += buf;
		if (i != stuffs->pets.size() - 1)
			petbuffer += "|";
	}

	NET_WriteTraceAddItemPacket_cgitem(*g_net_socket, itembuffer.c_str(), petbuffer.c_str(), stuffs->gold);
	/*for (size_t i = 0; i < 20; ++i)
		g_trade_item_array[i] = -1;
	
	for (size_t i = 0; i < stuffs->items.size() && i < 20; ++i)
		g_trade_item_array[i] = stuffs->items.at(i).itempos;

	for (size_t i = 0; i < 5; ++i)
		g_trade_pet_array[i] = -1;

	for (size_t i = 0; i < stuffs->pets.size() && i < 5; ++i)
		g_trade_pet_array[i] = stuffs->pets.at(i);

	*g_trade_gold = stuffs->gold;

	UI_SelectTradeAddStuffs(2, 2);*/
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
	SendMessageA(g_MainHwnd, WM_CGA_GET_CRAFTS_INFO, skill_index, (LPARAM)&info);
	return info;
}

void CGAService::WM_GetTeamPlayerInfo(cga_team_players_t *info)
{
	if (!IsInGame())
		return;

	if (!*g_is_in_team)
		return;

	for (int i = 0; i < 5; ++i)
	{
		if (g_team_player_base[i].valid)
		{
			std::string name;

			if (g_team_player_base[i].unit_id == (*g_playerBase)->unitid)
				name = boost::locale::conv::to_utf<char>((*g_playerBase)->name, "GBK");
			else if (g_team_player_base[i].actor)
				name = boost::locale::conv::to_utf<char>((char *)g_team_player_base[i].actor + 196, "GBK");

			info->emplace_back(
				g_team_player_base[i].unit_id,
				g_team_player_base[i].hp,
				g_team_player_base[i].mp,
				g_team_player_base[i].maxhp,
				name
			);
		}
	}
}

cga_team_players_t CGAService::GetTeamPlayerInfo(void)
{
	cga_team_players_t info;
	SendMessageA(g_MainHwnd, WM_CGA_GET_TEAM_PLAYERS_INFO, (WPARAM)&info, 0);
	return info;
}

void CGAService::WM_FixMapWarpStuck(int type)
{
	if (type == 0)
	{
		WriteLog("FixMapWarpStuck type 0\n");

		*g_disable_move = 0;
		*g_game_status_cgitem = 1;
	}
	else if (type == 1)
	{
		WriteLog("FixMapWarpStuck type 1\n");

		if (IsMapObjectEntrance(g_player_xpos->decode(), g_player_ypos->decode()))
		{
			*g_do_switch_map = 1;
			*g_switch_map_frames = 1;
			*g_switch_map_type = 1;
			*g_is_moving = 1;
		}
	}
}

void CGAService::FixMapWarpStuck(int type)
{
	if (type == 2)
	{
		m_move_checkwarp = true;
	}
	else if (type == 3)
	{
		m_move_checkwarp = false;
	}
	else
	{
		SendMessageA(g_MainHwnd, WM_CGA_FIX_WARP_STUCK, (WPARAM)type, 0);
	}
}

void CGAService::SetNoSwitchAnim(bool enable)
{
	m_ui_noswitchanim = enable;
}

void CGAService::SetImmediateDoneWork(bool enable)
{
	m_work_immediate = enable;
}

void CGAService::WM_SetWindowResolution(int w, int h)
{
	*g_resolution_width = w;
	*g_resolution_height = h;
	SYS_ResetWindow();
}

void CGAService::SetWindowResolution(int w, int h)
{
	SendMessageA(g_MainHwnd, WM_CGA_SET_WINDOW_RESOLUTION, w, h);
}

void CGAService::WM_RequestDownloadMap(int xbottom, int ybottom, int xsize, int ysize)
{
	NET_WriteRequestDownloadMapPacket_cgitem(*g_net_socket, *g_map_index1, *g_map_index3, xbottom, ybottom, xsize, ysize);
}

void CGAService::RequestDownloadMap(int xbottom, int ybottom, int xsize, int ysize)
{
	SendMessageA(g_MainHwnd, WM_CGA_REQUEST_DOWNLOAD_MAP, xbottom & 0xffff | ((xsize & 0xffff) << 16), ybottom & 0xffff | ((ysize & 0xffff) << 16));
}