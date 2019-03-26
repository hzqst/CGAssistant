#pragma once

#include "../CGALib/gameinterface.h"
#include "../CGALib/packdata.h"

struct IDirectDraw;
struct IDirectDrawSurface;

namespace CGA
{
#pragma pack(4)

	class CXorValue
	{
	public:
		CXorValue(int value);
		int decode();
		void set(int value);
	public:
		int unknown;
		int key1;
		int key2;
		unsigned char refcount;
	};

	//item base ??? D04820->D04E64(644)
	//item base CC70B0->CC76FC (64c)
	//sizex=0x64C
	typedef struct item_s
	{
		short valid;
		short unk;
		int unk2;
		int count;
		int item_id;
		char name[1596];
	}item_t;

	static_assert(sizeof(item_t) == 0x64C, "Size check");

	//item base 1212DCC, include equipments
	//size=1628 (65C)
	//[0] = hat
	//[1] = cloth
	//[2] = right hand
	//[3] = left hand
	//[4] = foot
	//[5] = decoration 1
	//[6] = decoration 2
	//[7] = crystal
	typedef struct item_info_s
	{
		short valid;
		char name[46];
		char attr[96];
		char attr2[672];
		char info[192];
		char info2[576];
		int flags;//2=right clickable
		int unk;
		int image_id;
		int level;
		int item_id;
		int count;
		int type;
		short double_clickable;
		short unk2;
		short unk3;
		short assess_flags;
		int assessed;//已鉴定
		int unk6;
	}item_info_t;

	static_assert(sizeof(item_info_t) == 0x65C, "Size check");

	typedef struct bank_item_info_s
	{
		short valid;
		short padding;
		int image_id;
		int count;
		char name[29];
		char attr[96];
		char info[672];
		char info2[787];
		int flags;//2=right clickable
		int unk;
	}bank_item_info_t;
	
	static_assert(sizeof(bank_item_info_t) == 0x644, "Size check");

#define PLAYER_ENABLE_FLAGS_PK (1<<2)
#define PLAYER_ENABLE_FLAGS_TEAMCHAT (1<<3)
#define PLAYER_ENABLE_FLAGS_JOINTEAM (1<<0)
#define PLAYER_ENABLE_FLAGS_CARD (1<<4)
#define PLAYER_ENABLE_FLAGS_TRADE (1<<5)
#define PLAYER_ENABLE_FLAGS_FAMILY (1<<6)

	typedef struct playerbase_s
	{
		short race;	//+0
		short padding;//+2
		int level;	//+4
		int unk;	//+8
		int punchclock;//+12
		int using_punchclock;	//+16
		int tickcount;	//+20
		CXorValue hp;//+24
		CXorValue maxhp;//+40
		CXorValue mp;//+56
		CXorValue maxmp;//+72
		CXorValue xp;//+88
		CXorValue maxxp;//+104
		int points_endurance;//+120
		int points_strength;//+124
		int points_defense;//+128
		int points_agility;//+132
		int points_magical;//+136		
		int manu_endurance;//+140
		int manu_skillful;//+144
		int manu_intelligence;//+148
		int value_attack; //+152
		int value_defensive;//+156
		int value_agility;//+160
		int value_spirit;//+164
		int value_recovery;//+168
		int value_enchantment;//+172
		int unk3;//+176
		int resist_poison;//+180
		int resist_hypnosis;//+184
		int resist_petrify;//+188
		int resist_drunk;//+192
		int resist_chaos;//+196
		int resist_forget;//+200
		int fix_critical;//+204
		int fix_strikeback;//+208
		int fix_accurancy;//+212
		int fix_dodge;//+216
		int element_earth;//+220
		int element_water;//+224
		int element_fire;//+228
		int element_wind;//+232
		int health;//+236
		int souls;//+240
		int gold;//+244
		int special_nick;//+248
		int score;//+252
		char name[17];//+256
		char player_nick[17];//+273
		char what[33];//+290
		char what2[33];//+323
		int skill_maxrows;//+356
		int move_speed;//+360 int unk
		int unk4;//+364
		int unk5;//+368
		int unk6;//+372
		int unitid;//+376
		int direction;//+380
		short unk7;//+384
		short unk8;//+386
		char unk9;//+388
		char padding2;
		short unk10;//+390
		int collect_type;//+392
		int enable_flags;//+396
		int unk11;//+400
		int unk12;//+404
		int unk13;//+408
		item_info_t iteminfos[40];//+412 = itembase
	}playerbase_t;

	static_assert(offsetof(playerbase_t, iteminfos) == 412, "Size check");

	//size = 148
	typedef struct skill_sub_s
	{
		char name[25];
		char info[99];
		int cost;
		int unk1;
		int flags;
		int unk3;
		int available;
		int level;
	}skill_sub_t;

	static_assert(sizeof(skill_sub_t) == 148, "Size check");

	typedef struct skill_craft_mat_s
	{
		int itemid;//-1 if none
		int count;//-1 if none
		char name[32];
	}skill_craft_mat_t;

	static_assert(sizeof(skill_craft_mat_t) == 40, "Size check");

	typedef struct skill_craft_s
	{
		int subskill_id;
		int cost;
		int level;
		int available;
		int itemid;
		char name[29];
		char info[59];
		skill_craft_mat_t mats[5];
	}skill_craft_t;

	static_assert(sizeof(skill_craft_t) == 308, "Size check");

	//size=0x49FC
	typedef struct skill_s
	{
		char name[25];
		char valid;
		char padding[2];
		int level;
		int maxlevel;
		int xp;
		int maxxp;
		int type;//1=制造 2=鉴定 3=治疗 4=采集
		int skill_id;//255 =???, 225=伐木
		int unk3;
		int pos;
		//offset=60
		skill_sub_t sub[11];
		;//offset=1688
		skill_craft_t craft[50];
		char padding2[1852];
	}skill_t;

	static_assert(sizeof(skill_t) == 0x49FC, "Size check");

	//size=140
	typedef struct pet_skill_s
	{
		short valid;//0=无效 1=有效
		short unk;
		short unk2;
		short flags;
		char name[25];
		char info[99];
		int cost;
		int unk3;
	}pet_skill_t;

	static_assert(sizeof(pet_skill_t) == 140, "Size check");

	//size=1752 for se_3000
	typedef struct pet_s
	{
		int flags;
		short race;
		short padding;
		int level;
		CXorValue hp;
		CXorValue maxhp;
		CXorValue mp;
		CXorValue maxmp;
		CXorValue xp;
		CXorValue maxxp;
		int points_remain;
		int points_endurance;
		int points_strength;
		int points_defense;
		int points_agility;
		int points_magical;
		int value_attack;
		int value_defensive;
		int value_agility;
		int value_spirit;
		int value_recovery;
		int value_loyality;
		int resist_poison;
		int resist_hypnosis;
		int resist_petrify;
		int resist_drunk;
		int resist_chaos;
		int resist_forget;
		int fix_critical;
		int fix_strikeback;
		int fix_accurancy;
		int fix_dodge;
		int element_earth;	//地0~100
		int element_water;	//水0~100
		int element_fire;	//火0~100
		int element_wind;	//风0~100
		int health;			//健康 0=绿 100=红
		pet_skill_t skills[10];
		char unk[90];
		short battle_flags;	//100A77A
		char realname[17];//水龙蜥
		char name[19];
		int unk2;
		int unk3;
	}pet_t;

	static_assert(sizeof(pet_t) == 1752, "Size check");

	//size=20
	typedef struct item_menu_player_s
	{
		short color;
		char name[18];
	}item_menu_player_t;

	static_assert(sizeof(item_menu_player_t) == 20, "Size check");

	//size=52
	typedef struct item_menu_unit_s
	{
		char name[20];
		int level;
		int health;
		int hp;
		int maxhp;
		int mp;
		int maxmp;
		int color;
		int index;
	}item_menu_unit_t;

	static_assert(sizeof(item_menu_unit_t) == 52, "Size check");

	typedef struct unit_menu_s
	{
		char *name;
		int level;
		int hp;
		int maxhp;
		int mp;
		int maxmp;
		unsigned char health;
		unsigned char color;
		char padding[2];
	}unit_menu_t;

	static_assert(sizeof(unit_menu_t) == 28, "Size check");

	//for internal usage
	typedef struct battle_unit_s
	{
		bool exist;
		char name[32];
		int modelid;
		int level;
		int curhp;
		int maxhp;
		int curmp;
		int maxmp;
		int pos;
	}battle_unit_t;

	typedef struct move_item_s
	{
		int itempos;
		int dstpos;
		int count;
	}cga_move_item_t;

	typedef struct map_unit_s
	{
		short valid;// if 0 invalid
		short type;//+2
		int unit_id;//+4
		int model_id;//+8
		CXorValue xpos;//+12
		CXorValue ypos;//+28
		int unk1;//1 //+44
		int unk2;//0 //+48
		int unk3;//-1? //+52
		int unk4;//0 //+56
		int unk5;//0 //+60
		short unk7; //+64
		short unk8; //+66
		char unk9;//+68
		char padding[3];
		int unk10;//+72
		int unk11;//+76
		int item_count;//+80
		char unit_name[17];//+84
		char nick_name[17];//+101
		char unk13[33];//+118
		char unk14[33];//+151
		char unk15[17];//+184
		char item_name[67];//+201
		int unk17;
		int unk18;//0x1000
		short unk19;//1
		short padding2;
		void *actor;
		short unk20;
		short padding3;
		int unk21;//0x124
		char unk22[24];
	}map_unit_t;

#pragma pack()

	class CGAService
	{
	public:
		CGAService();
		void Initialize(game_type type);
		void Uninitialize();
	public:
		virtual void InitializeGameData(cga_game_data_t);
		virtual bool Connect();

		virtual int IsInGame();
		virtual int GetWorldStatus();
		virtual int GetGameStatus();

		virtual cga_player_info_t GetPlayerInfo();
		virtual void SetPlayerFlagEnabled(int index, bool enable);
		virtual bool IsPlayerFlagEnabled(int index);

		virtual bool IsSkillValid(int index);
		virtual cga_skill_info_t GetSkillInfo(int index);
		virtual cga_skills_info_t GetSkillsInfo();
		virtual cga_subskill_info_t GetSubSkillInfo(int index, int stage);
		virtual cga_subskills_info_t GetSubSkillsInfo(int index);
		virtual bool IsPetValid(int petid);
		virtual cga_pet_info_t GetPetInfo(int index);
		virtual cga_pets_info_t GetPetsInfo();
		virtual bool IsPetSkillValid(int petid, int skillid);
		virtual cga_pet_skill_info_t GetPetSkillInfo(int petid, int skillid);
		virtual cga_pet_skills_info_t GetPetSkillsInfo(int petid);

		virtual bool IsItemValid(int itempos);
		virtual cga_item_info_t GetItemInfo(int itempos);
		virtual cga_items_info_t GetItemsInfo();
		virtual cga_items_info_t GetBankItemsInfo();
		virtual bool DropItem(int itempos);
		virtual bool UseItem(int itempos);
		virtual bool MoveItem(int itempos, int dstpos, int count);

		virtual std::tuple<int, int> GetMapXY();
		virtual std::tuple<float, float> GetMapXYFloat();
		virtual std::tuple<float, float> GetMoveSpeed();
		virtual std::string GetMapName();
		virtual cga_map_units_t GetMapUnits();
		virtual void WalkTo(int x, int y);
		virtual void TurnTo(int x, int y);
		virtual void SetMoveSpeed(int speed);
		virtual void LogBack();
		virtual void LogOut();
		virtual bool ClickNPCDialog(int option, int flags);
		virtual bool SellNPCStore(cga_sell_items_t items);
		virtual bool BuyNPCStore(cga_buy_items_t items);
		virtual bool PlayerMenuSelect(int menuindex);
		virtual bool UnitMenuSelect(int menuindex);

		virtual bool IsBattleUnitValid(int pos);
		virtual cga_battle_unit_t GetBattleUnit(int pos);
		virtual cga_battle_units_t GetBattleUnits();
		virtual int GetBattleRoundCount();
		virtual int GetBattlePlayerPosition();
		virtual int GetBattlePlayerStatus();
		virtual int GetBattleSkillPerformed();
		virtual int GetBattlePetId();
		virtual int GetBattleEndTick();
		virtual void SetBattleEndTick(int msec);
		virtual bool BattleNormalAttack(int target);		
		virtual bool BattleSkillAttack(int skillpos, int skilllv, int target);		
		virtual bool BattleDefense();		
		virtual bool BattleEscape();
		virtual bool BattleExchangePosition();
		virtual bool BattleChangePet(int petid);
		virtual bool BattleUseItem(int itempos, int target);		
		virtual bool BattlePetSkillAttack(int skillpos, int target);
		virtual void BattleSetHighSpeedEnabled(bool enable);
		virtual void BattleSetShowHPMPEnabled(bool enable);

		virtual void SayWords(std::string str, int color, int range, int size);
		virtual void SetWorkDelay(int delay);
		virtual void SetWorkAcceleration(int percent);
		virtual bool StartWork(int skill_index, int sub_index);
		virtual bool CraftItem(cga_craft_item_t craft);		
		virtual bool AssessItem(int skill_index, int itempos);
		virtual cga_craft_info_t GetCraftInfo(int skill_index, int subskill_index);
		virtual cga_crafts_info_t GetCraftsInfo(int skill_index);
		virtual bool IsMapCellPassable(int x, int y);
		virtual bool ForceMove(int dir, bool show);
		virtual bool ForceMoveTo(int x, int y, bool show);
	private:
		int *g_world_status_cgitem;
		int *g_game_status_cgitem;
		CXorValue *g_world_status;
		CXorValue *g_game_status;

		CXorValue *g_player_xpos;
		CXorValue *g_player_ypos;
		playerbase_t **g_playerBase;
		playerbase_t *g_playerBase_cgitem;
		bank_item_info_t *g_bank_item_base;
		char *g_player_name;
		pet_t *g_pet_base;
		int *g_pet_id;
		char *g_job_name;
		skill_t *g_skill_base;
		short *g_map_x;
		short *g_map_y;
		char *g_map_name;
		int *g_move_xdest;
		int *g_move_ydest;
		CXorValue *g_move_destx;
		CXorValue *g_move_desty;
		float *g_move_xspeed;
		float *g_move_yspeed;
		float *g_map_xf;
		float *g_map_yf;
		float *g_map_xf2;
		float *g_map_yf2;
		CXorValue *g_map_xpos;
		CXorValue *g_map_ypos;
		int *g_move_walking;
		int *g_move_turning;
		int *g_mouse_states;
		int *g_is_ingame;
		int *g_work_basedelay;

		char *g_btl_buffers;
		int *g_btl_buffer_index;
		int *g_btl_round_count;
		int *g_btl_player_pos;
		int *g_btl_player_status;
		int *g_btl_skill_allowbit;
		int *g_btl_petid;
		int *g_btl_action_done;
		int *g_btl_skill_performed;
		unsigned int *g_btl_round_endtick;

		char **g_net_buffer;
		int *g_net_socket;

		char *net_header_common;
		char *net_header_walk;
		char *net_header_battle;
		char *net_header_logback;
		char *net_header_useitem;
		char *net_header_dropitem;
		char *net_header_sell;
		char *net_header_sayword;
		char *net_header_secondary;

		//todo
		item_t *g_item_base;
		item_info_t *g_item_info_base;

		void *g_pGameBase;

		int *g_logback;
		int *g_item_cur_using;
		item_menu_player_t *g_item_menu_players;
		int *g_item_menu_player_count;
		item_menu_unit_t *g_item_menu_units;
		int *g_item_menu_unit_count;
		 
		unit_menu_t **g_unit_menu_base;//D38E60
		int *g_unit_menu_count;//CB4140
		int *g_unit_menu_begin;//CE4C34
		char ***g_player_menu_name;//D29734
		char **g_player_menu_color;//CE4AA0
		int *g_player_menu_count;//CE4C28
		int *g_player_menu_begin;//CDBB24
		char *g_player_menu_clickable;
		char *g_unit_menu_clickable;

		char *g_ui_craftdialog_additem_count;
		char *g_work_accelerate;//63FFFE
		char *g_work_accelerate_percent;//627E8C
		int *g_craft_step;//CBF15E
		//int *g_ret_address;

		int *g_unit_count;
		map_unit_t *g_map_units;
		int *g_npc_dialog_show;
		int *g_npc_dialog_type;
		int *g_npc_dialog_option;
		int *g_npc_dialog_dlgid;
		int *g_npc_dialog_npcid;

		short *g_map_x_bottom;
		short *g_map_y_bottom;
		short *g_map_x_size;
		short *g_map_y_size;
		short *g_map_collision_table;

		int *g_healing_skill_index;
		int *g_healing_subskill_index;

		HANDLE *g_mutex;
	public:
		char(__cdecl *Sys_CheckModify)(const char *a1);
		void(__cdecl *COMMON_PlaySound)(int a1, int a2, int a3);
		void(__cdecl *BATTLE_PlayerAction)();
		void(__cdecl *NET_ParseBattlePackets)(int a1, const char *buf);
		void(__cdecl *NET_ParseWorkingResult)(int a1, int success, int type, const char *buf);
		void(__cdecl *NET_ParseChatMsg)(int a1, int unitid, const char *buf, int a4, int a5);

		void(_cdecl *R_DrawText)(int a1);
		void(__cdecl *Move_Player)();
		void(__cdecl *CL_MoveItemEx)(int);//C0EC0
		void(__cdecl *UI_ClickLogout)(int a1, int a2);

		void(__cdecl *NET_WritePacket)(void *net_buffer, int net_socket, const char *header, const char *buf);
		void(__cdecl *NET_WritePacket1)(void *net_buffer, int net_socket, const char *header, int a4, int a5, int a6);
		void(__cdecl *NET_WritePacket2)(void *net_buffer, int net_socket, const char *header, int a4, int a5, int a6, int a7);
		void(__cdecl *NET_WritePacket3)(void *net_buffer, int net_socket, const char *header, int a4, int a5, int a6, int a7, int a8, const char *buf);
		void(__cdecl *NET_WriteSayWords)(void *net_buffer, int net_socket, const char *header, int a4, int a5, const char *message, int color, int range, int size);
		void(__cdecl *NET_WriteLogbackPacket_cgse)(void *, int, const char *);
		
		void(__cdecl *NET_WriteSayWords_cgitem)(int net_socket, int x, int y, const char *message, int color, int range, int size);
		void(__cdecl *NET_WritePacket3_cgitem)(int net_socket, int a4, int a5, int a6, int a7, int a8, const char *buf);
		void(__cdecl *NET_WriteLogbackPacket_cgitem)(int);
		void(__cdecl *NET_WriteUseItemPacket_cgitem)(int, int, int, int, int);
		void(__cdecl *NET_WriteBattlePacket_cgitem)(int, const char *);
		void(__cdecl *NET_WriteDropItemPacket_cgitem)(int, int, int, int);
		void(__cdecl *NET_WriteMovePacket_cgitem)(int, int, const char *);
		void(__cdecl *NET_WriteOpenHealDialog_cgitem)(int, int);
		void (__cdecl *NET_WriteWorkPacket_cgitem)(int, int, int, int, const char *);

		int(__cdecl **UI_PlayerMenuSelect)(int, int );
		int(__cdecl **UI_UnitMenuSelect)(int, int );
		int(__cdecl *UI_PlayerMenuShowup)(int);
		int(__cdecl *UI_UnitMenuShowup)(int);
		int(__cdecl *UI_PlayerMenuMouseEvent)(int, char);
		int(__cdecl *UI_UnitMenuMouseEvent)(int, char);		
		void(__cdecl *NPC_ShowDialogInternal)(int type, int options, int dlgid, int objid, const char *message);
		int(__cdecl *NPC_ClickDialog)(int option, int index, int a3, char a4);
		void(__cdecl *NPC_ClickExchangeDialog)(int option);
		int(__cdecl *UI_ClickNPCDialog)(int option, char flags);
		void(__cdecl *UI_OpenGatherDialog)(int skill_index, int sub_index);
		void(__cdecl *UI_OpenCraftDialog)(int skill_index, int sub_index, int sub_type);
		void(__cdecl *UI_OpenAssessDialog)(int skill_index, int sub_index);
		int(__cdecl *UI_HandleMiniDialogMouseEvent)(int widget, char flags);
		int(__cdecl *UI_IsMouseInRect)(int a1, int a2, int a3, int a4, int a5);
		int(__cdecl *UI_HandleLearnSkillConfirmMouseEvent)(int index, char flags);
		int(__cdecl *UI_HandleEnablePlayerFlagsMouseEvent)(int index, char flags);
		int(__cdecl *UI_HandleCraftItemSlotMouseEvent)(int a1, char flags);
		int( __cdecl *UI_HandleCraftItemButtonMouseEvent)(int a1, char flags);

		void NewBATTLE_PlayerAction();
		void NewNET_ParseBattlePackets(int a1, const char *buf);
		void NewNET_ParseWorkingResult(int a1, int success, int type, const char *buf);
		void NewNET_ParseChatMsg(int a1, int unitid, const char *buf, int color, int size);
		void NewMove_Player();
		void NewUI_PlayerMenuShowup(int a1);
		void NewUI_UnitMenuShowup(int a1);
		int NewUI_PlayerMenuMouseEvent(int widget_index, char flags);
		int NewUI_UnitMenuMouseEvent(int widget_index, char flags);
		void NewNPC_ShowDialogInternal(int type, int options, int dlgid, int objid, const char *message);
		int NewUI_IsMouseInRect(int a1, int a2, int a3, int a4, int a5);
		int NewUI_HandleMiniDialogMouseEvent(int widget, char flags);
		int NewUI_HandleEnablePlayerFlagsMouseEvent(int index, char flags);
		int NewUI_HandleCraftItemSlotMouseEvent(int index, char flags);
		int NewUI_HandleCraftItemButtonMouseEvent(int index, char flags);
		void ParseGatheringResult(int success, const char *buf);
		void ParseHealingResult(int success, const char *buf);
		void ParseAssessingResult(int success, const char *buf);
		void ParseCraftingResult(int success, const char *buf);

		IDirectDraw *GetDirectDraw();
		IDirectDrawSurface *GetDirectDrawBackSurface();
		void ParseBattleUnits(const char *buf, size_t len);
		void DrawBattleInfo();
		int IsItemTypeAssessable(int type);

		bool WM_BattleNormalAttack(int target);
		bool WM_BattleSkillAttack(int skillpos, int skilllv, int target);
		bool WM_BattleDefense();
		bool WM_BattleEscape();
		bool WM_BattleExchangePosition();
		bool WM_BattleChangePet(int petid);
		bool WM_BattleUseItem(int itempos, int target);
		bool WM_BattlePetSkillAttack(int skillpos, int target);
		void WM_LogBack();
		void WM_LogOut();
		bool WM_DropItem(int itempos);
		bool WM_UseItem(int itempos);
		bool WM_MoveItem(cga_move_item_t *mov);
		bool WM_ClickNPCDialog(int option, int flags);
		bool WM_SellNPCStore(cga_sell_items_t *items);
		bool WM_BuyNPCStore(cga_buy_items_t *items);
		void WM_GetPlayerInfo(cga_player_info_t *info);
		void WM_GetSkillInfo(int index, cga_skill_info_t *info);
		void WM_GetSkillsInfo(cga_skills_info_t *info);
		void WM_GetSubSkillInfo(int index, int stage, cga_subskill_info_t *info);
		void WM_GetSubSkillsInfo(int index, cga_subskills_info_t *info);
		void WM_GetPetInfo(int index, cga_pet_info_t *info);
		void WM_GetPetsInfo(cga_pets_info_t *info);
		void WM_GetPetSkillInfo(int petid, int skillid, cga_pet_skill_info_t *info);
		void WM_GetPetSkillsInfo(int petid, cga_pet_skills_info_t *info);
		void WM_GetItemInfo(int itempos, cga_item_info_t *info);
		void WM_GetItemsInfo(cga_items_info_t *info);
		void WM_GetBankItemsInfo(cga_items_info_t *info);
		void WM_GetBattleUnit(int pos, cga_battle_unit_t *u);
		void WM_GetBattleUnits(cga_battle_units_t *u);
		void WM_GetMapUnits(cga_map_units_t *units);
		void WM_SayWords(const char *str, int color, int range, int size);
		bool WM_StartWork(int skill_index, int sub_index);
		bool WM_CraftItem(cga_craft_item_t *craft);
		bool WM_AssessItem(int skill_index, int itempos);
		bool WM_IsMapCellPassable(int x, int y);
		bool WM_ForceMove(int dir, bool show);
		bool WM_ForceMoveTo(int x, int y, bool show);
		void WM_GetCraftInfo(cga_craft_item_t *craft, cga_craft_info_t *info);
		void WM_GetCraftsInfo(int skill_index, cga_crafts_info_t *info);
		bool m_initialized;
		bool m_btl_highspeed_enable;
		bool m_btl_showhpmp_enable;
		bool m_btl_double_action;
		battle_unit_t m_battle_units[20];
		int m_move_to;
		int m_move_to_x, m_move_to_y;
		int m_menu_item_selected;
		int m_menu_item_select_index;
		int m_move_speed;
		bool m_ui_minidialog_loop;
		int m_ui_minidialog_loop_index;
		int m_ui_minidialog_click_index;
		int m_ui_learn_skill_confirm;
		int m_desired_player_enable_flags;
		int m_change_player_enable_flags;
		bool m_ui_craftdialog_click_begin;
		bool m_ui_craftdialog_loop;
		int m_ui_craftdialog_loop_index;
		int m_ui_craftdialog_click_index;
		int m_ui_craftdialog_additem[5];
		int m_ui_craftdialog_additemcount;
		int m_work_acceleration;

		game_type m_game_type;

		HFONT m_hFont;
		ULONG_PTR m_ImageBase;
		ULONG m_ImageSize;
	};
}

#define WM_CGA_BATTLE_NORMALATTACK WM_USER+10000
#define WM_CGA_BATTLE_SKILLATTACK WM_USER+10001
#define WM_CGA_BATTLE_DEFENSE WM_USER+10002
#define WM_CGA_BATTLE_ESCAPE WM_USER+10003
#define WM_CGA_BATTLE_USEITEM WM_USER+10004
#define WM_CGA_BATTLE_PETSKILLATTACK WM_USER+10005
#define WM_CGA_DROP_ITEM WM_USER+10006
#define WM_CGA_USE_ITEM WM_USER+10007
#define WM_CGA_MOVE_ITEM WM_USER+10008
#define WM_CGA_LOG_BACK WM_USER+10011
#define WM_CGA_LOG_OUT WM_USER+10012
#define WM_CGA_GET_MAP_UNITS WM_USER+10013
#define WM_CGA_GET_PET_INFO WM_USER+10014
#define WM_CGA_GET_PETS_INFO WM_USER+10015
#define WM_CGA_GET_SKILL_INFO WM_USER+10016
#define WM_CGA_GET_SKILLS_INFO WM_USER+10017
#define WM_CGA_GET_SUBSKILL_INFO WM_USER+10018
#define WM_CGA_GET_SUBSKILLS_INFO WM_USER+10019
#define WM_CGA_GET_PLAYER_INFO WM_USER+10020
#define WM_CGA_GET_ITEM_INFO WM_USER+10021
#define WM_CGA_GET_ITEMS_INFO WM_USER+10022
#define WM_CGA_GET_BATTLE_UNIT_INFO WM_USER+10023
#define WM_CGA_GET_BATTLE_UNITS_INFO WM_USER+10024
#define WM_CGA_GET_PET_SKILL_INFO WM_USER+10025
#define WM_CGA_GET_PET_SKILLS_INFO WM_USER+10026
#define WM_CGA_CLICK_NPC_DIALOG WM_USER+10027
#define WM_CGA_SELL_NPC_STORE WM_USER+10028
#define WM_CGA_BUY_NPC_STORE WM_USER+10029
#define WM_CGA_SAY_WORDS WM_USER+10030
#define WM_CGA_BATTLE_EXCHANGE_POSITION WM_USER+10031
#define WM_CGA_BATTLE_CHANGE_PET WM_USER+10032
#define WM_CGA_START_WORK WM_USER+10033
#define WM_CGA_IS_MAP_CELL_PASSABLE WM_USER+10034
#define WM_CGA_FORCE_MOVE WM_USER+10035
#define WM_CGA_FORCE_MOVE_TO WM_USER+10036
#define WM_CGA_CRAFT_ITEM WM_USER+10037
#define WM_CGA_GET_CRAFT_INFO WM_USER+10038
#define WM_CGA_GET_CRAFTS_INFO WM_USER+10039
#define WM_CGA_ASSESS_ITEM WM_USER+10040
#define WM_CGA_GET_BANK_ITEMS_INFO WM_USER+10041

#define CGA_PORT_BASE 4396