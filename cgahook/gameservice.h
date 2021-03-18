#pragma once

#include "../CGALib/gameinterface.h"
#include "../CGALib/packdata.h"

#include <list>
#include <shellapi.h>

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

	typedef struct ui_anim_s
	{
		int a1;
		int a2;
		int ymove;
		int flags;
		int ypos;
		int counter;
		int state;
		int type;
	}ui_anim_t;

	typedef struct btn_rect_s
	{
		int left;
		int top;
		int width;
		int height;
		int image_idle;
		int image_mouseover;
		int image_unk;
	}btn_rect_t;

	static_assert(sizeof(btn_rect_t) == 28, "Size check");

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
		char attr[8][96];
		char info[8][96];
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
		char attr[8][96];
		char info[8][96];
		char padding2[19];
		int flags;//2=right clickable
		int unk;
	}bank_item_info_t;
	
	static_assert(sizeof(bank_item_info_t) == 0x644, "Size check");

	typedef struct bank_pet_info_s
	{
		short valid; //0x0000 
		short race; //0x0002 
		int imageid; //0x0004 
		int level; //0x0008 
		int padding1; //0x000C 
		int maxhp; //0x0010 
		int padding2; //0x0014 
		int maxmp; //0x0018 
		int points_endurance; //0x001C 
		int points_strength; //0x0020 
		int points_defense; //0x0024 
		int points_agility; //0x0028 
		int points_magical; //0x002C 
		int value_attack; //0x0030 
		int value_defensive; //0x0034 
		int value_agility; //0x0038 
		int value_spirit; //0x003C 
		int value_recovery; //0x0040 
		int value_loyality; //0x0044 
		int element_earth; //0x0048 
		int element_water; //0x004C 
		int element_fire; //0x0050 
		int element_wind; //0x0054 
		int skill_slots; //0x0058 
		int padding3; //0x005C 
		int padding4; //0x0060 
		char skills[9][0x8C]; //0x105CE0C 
		char skill_last[0x84];
		char nickname[17]; //0x105CE0C 
		char originalname[19]; //0x105CE0C 
	}bank_pet_info_t;

	static_assert(sizeof(bank_pet_info_t) == 0x5F8, "Size check");

	typedef struct npcdlg_item_info_s
	{
		char valid;//CD3198
		char name[29];//CD3199
		char attr[1538];//CD31B6
		int image_id;//CD37B8
		int cost;//CD37BC
		int selected;//CD37C0
		int itempos;//CD37C4
		int unk;
		int unk2;//init to 1
		int unk3;
		char unk4;
		char padding[3];
		int unk5;
		char info[20];//CD37DC
	}npcdlg_item_info_t;

	static_assert(sizeof(npcdlg_item_info_t) == 0x658, "Size check");

	//12AB3E0
	typedef struct player_pers_desc_s
	{
		int sellIcon;
		int sellUnk;
		char sellString[256];
		int buyIcon;
		int buyUnk;
		char buyString[256];
		int wantIcon;
		int wantUnk;
		char wantString[256];
		char descString[256];
		char unk[0x410];
	}player_pers_desc_t;

	static_assert(sizeof(player_pers_desc_t) == 0x828, "Size check");

#define PLAYER_ENABLE_FLAGS_PK (1<<2)
#define PLAYER_ENABLE_FLAGS_TEAMCHAT (1<<3)
#define PLAYER_ENABLE_FLAGS_JOINTEAM (1<<0)
#define PLAYER_ENABLE_FLAGS_CARD (1<<4)
#define PLAYER_ENABLE_FLAGS_TRADE (1<<5)
#define PLAYER_ENABLE_FLAGS_FAMILY (1<<6)

#define PET_STATE_READY 1
#define PET_STATE_BATTLE 2
#define PET_STATE_REST 3
#define PET_STATE_WALK 16

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
		int value_charisma;//+172
		int unk3;//+176
		int resist_poison;//+180
		int resist_sleep;//+184
		int resist_medusa;//+188
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
		int special_nick;//+248 //使用第几个称号
		int score;//+252
		char name[17];//+256
		char player_nick[17];//+273
		char what[33];//+290
		char what2[33];//+323
		int skill_maxrows;//+356
		int move_speed;//+360 int unk
		int unk4;//+364
		int image_id;//+368
		int avatar_id;//+372
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
		int battle_position;//+408
		item_info_t iteminfos[40];//+412 = itembase
	}playerbase_t;

	static_assert(offsetof(playerbase_t, iteminfos) == 412, "Size check");

	//size = 148
	typedef struct skill_sub_s
	{
		char name[25];//0
		char info[99];//25
		int cost;//124
		int unk1;//128
		int flags;//132
		int unk3;//136
		int available;//140
		int level;//144
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
		char slotsize;
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
		int resist_sleep;
		int resist_medusa;
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
		short battle_flags;	//100A77A//+12110A2h
		char realname[17];//水龙蜥
		char name[19];
		int unk2;
		int walk;
	}pet_t;

	static_assert(sizeof(pet_t) == 1752, "Size check");

	typedef struct short_pet_s
	{
		short flags;
		short petid;
		int unk2;
	}short_pet_t;

	static_assert(sizeof(short_pet_t) == 8, "Size check");

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
		int flags;
		int pos;
	}battle_unit_t;

	typedef struct move_xxx_s
	{
		int srcpos;
		int dstpos;
		int count;
	}move_xxx_t;

	typedef struct map_unit_s
	{
		short valid;// if 0 invalid 640144
		short type;//+2
		int unit_id;//+4
		int model_id;//+8
		CXorValue xpos;//+12
		CXorValue ypos;//+28
		int level;//1 //+44
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
		char title_name[17];//+184
		char item_name[67];//+201
		int unk17;//+268
		int unk18;//+272 0x1000	//64118
		short flags;//+276 1	//6411C
		short padding2;//+278
		int unk20;		//640120
		void *actor;//+284 640124
		int injury;//+288
		int icon;
		char unk22[20];
	}map_unit_t;

	static_assert(sizeof(map_unit_t) == 0x13c, "Size check");

	typedef struct team_player_s
	{
		short valid;
		short padding;
		int unit_id;
		int maxhp;
		int mp;
		int hp;
		int unk;
		char name[20];
		void *actor;
	}team_player_t;

	static_assert(sizeof(team_player_t) == 0x30, "Size check");

	typedef struct sys_time_s
	{
		int years;
		int month;
		int days;
		int hours;
		int mins;
		int secs;
	}sys_time_t;

	static_assert(sizeof(sys_time_t) == 24, "Size check");

#pragma pack()

	class CGAService
	{
	public:
		CGAService();
		void Initialize(game_type type);
		void Uninitialize();
		char *GetPlayerName();
		int GetServerIndex();
	public:
		virtual void InitializeGameData(cga_game_data_t);
		virtual bool Connect();

		virtual int IsInGame();
		virtual int GetWorldStatus();
		virtual int GetGameStatus();
		virtual int GetBGMIndex();
		virtual cga_sys_time_t GetSysTime();

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
		virtual cga_pets_info_t GetBankPetsInfo();
		virtual bool IsPetSkillValid(int petid, int skillid);
		virtual cga_pet_skill_info_t GetPetSkillInfo(int petid, int skillid);
		virtual cga_pet_skills_info_t GetPetSkillsInfo(int petid);

		virtual bool IsItemValid(int itempos);
		virtual cga_item_info_t GetItemInfo(int itempos);
		virtual cga_items_info_t GetItemsInfo();
		virtual cga_items_info_t GetBankItemsInfo();
		virtual int GetBankGold();
		virtual bool DropItem(int itempos);
		virtual bool UseItem(int itempos);
		virtual bool MoveItem(int itempos, int dstpos, int count);
		virtual bool MovePet(int itempos, int dstpos);
		virtual bool DropPet(int petpos);
		virtual bool ChangePetState(int petpos, int state);
		virtual bool MoveGold(int gold, int operation);
	
		int GetMouseOrientation(void);
		virtual std::tuple<int, int> GetMouseXY();
		virtual std::tuple<int, int, int> GetMapIndex();
		virtual std::tuple<int, int> GetMapXY();
		virtual std::tuple<float, float> GetMapXYFloat();
		virtual std::tuple<float, float> GetMoveSpeed();
		virtual std::string GetMapName();
		virtual cga_map_units_t GetMapUnits();
		virtual void WalkTo(int x, int y);
		virtual void TurnTo(int x, int y);
		virtual void SetMoveSpeed(int speed);
		virtual std::vector<DWORD> GetMoveHistory(void);
		virtual void LogBack();
		virtual void LogOut();
		virtual bool ClickNPCDialog(int option, int flags);
		virtual bool SellNPCStore(cga_sell_items_t items);
		virtual bool BuyNPCStore(cga_buy_items_t items);
		virtual bool PlayerMenuSelect(int menuindex, const std::string &menustring);
		virtual bool UnitMenuSelect(int menuindex);
		virtual void UpgradePlayer(int attr);
		virtual void UpgradePet(int petindex, int attr);

		virtual bool IsBattleUnitValid(int pos);
		virtual cga_battle_unit_t GetBattleUnit(int pos);
		virtual cga_battle_units_t GetBattleUnits();
		virtual cga_battle_context_t GetBattleContext();
		virtual int GetBattleEndTick();
		virtual void SetBattleEndTick(int msec);
		virtual bool BattleNormalAttack(int target);		
		virtual bool BattleSkillAttack(int skillpos, int skilllv, int target, bool packetOnly);
		virtual bool BattleRebirth();
		virtual bool BattleGuard();
		virtual bool BattleEscape();
		virtual bool BattleExchangePosition();
		virtual bool BattleChangePet(int petid);
		virtual bool BattleUseItem(int itempos, int target);
		virtual bool BattlePetSkillAttack(int skillpos, int target, bool packetOnly);
		virtual bool BattleDoNothing();
		virtual void BattleSetHighSpeedEnabled(bool enable);
		virtual void BattleSetShowHPMPEnabled(bool enable);
		virtual void ChangePersDesc(cga_pers_desc_t desc);
		virtual bool ChangeNickName(std::string str);
		virtual bool ChangeTitleName(int titleId);
		virtual void SayWords(std::string str, int color, int range, int size);
		virtual void SetWorkDelay(int delay);
		virtual void SetWorkAcceleration(int percent);
		virtual bool StartWork(int skill_index, int sub_index);
		virtual bool CraftItem(cga_craft_item_t craft);		
		virtual bool AssessItem(int skill_index, int itempos);
		virtual cga_craft_info_t GetCraftInfo(int skill_index, int subskill_index);
		virtual cga_crafts_info_t GetCraftsInfo(int skill_index);
		virtual bool IsMapCellPassable(int x, int y);
		virtual cga_map_cells_t GetMapCollisionTableRaw(bool loadall);
		virtual cga_map_cells_t GetMapCollisionTable(bool loadall);
		virtual cga_map_cells_t GetMapObjectTable(bool loadall);
		virtual cga_map_cells_t GetMapTileTable(bool loadall);
		virtual bool ForceMove(int dir, bool show);
		virtual bool ForceMoveTo(int x, int y, bool show);
		virtual bool DoRequest(int request_type);
		virtual void TradeAddStuffs(cga_sell_items_t items, cga_sell_pets_t pets, int gold);
		virtual cga_team_players_t GetTeamPlayerInfo(void);
		virtual void FixMapWarpStuck(int type);
		virtual void SetNoSwitchAnim(bool enable);
		virtual void SetImmediateDoneWork(bool enable);
		virtual int GetImmediateDoneWorkState(void);
		virtual bool EnableFlags(int type, bool enable);
		virtual void SetWindowResolution(int w, int h);
		virtual void RequestDownloadMap(int xbottom, int ybottom, int xsize, int ysize);
		virtual double GetNextAnimTickCount();
		virtual int GetCraftStatus();
		virtual bool IsUIDialogPresent(int dialog);

		virtual void LoginGameServer(std::string gid, std::string glt, int serverid, int bigServerIndex, int serverIndex,int character);
	public:
		int *g_server_time;
		int *g_local_time;
		sys_time_t *g_sys_time;
		int *g_new_world_status_cgitem;
		int *g_world_status_cgitem;
		int *g_game_status_cgitem;
		CXorValue *g_world_status;
		CXorValue *g_game_status;
		int *g_bgm_index;
		void **g_pet_riding_stru;
		CXorValue *g_player_xpos;
		CXorValue *g_player_ypos;
		playerbase_t **g_playerBase;
		playerbase_t *g_playerBase_cgitem;
		bank_item_info_t *g_bank_item_base;
		bank_pet_info_t *g_bank_pet_base;
		int *g_bank_gold;
		npcdlg_item_info_t *g_npcdlg_item_base;
		char *g_player_name;
		int *g_player_remain_points;
		pet_t *g_pet_base;
		int *g_pet_id;
		int *g_pet_state;
		short_pet_t *g_short_pet_base;
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
		short *g_disable_move;
		int *g_is_moving;
		int *g_do_switch_map;
		int *g_switch_map_frames;
		int *g_switch_map_type;

		char *g_btl_buffers;
		int *g_btl_buffer_index;
		int *g_btl_round_count;
		int *g_btl_select_skill_level;
		int *g_btl_select_skill_index;
		int *g_btl_select_pet_skill_index;
		int *g_btl_select_pet_skill_state;
		int *g_btl_player_pos;
		int *g_btl_select_action;
		int *g_btl_player_status;
		int *g_btl_petskill_allowbit;
		int *g_btl_skill_allowbit;
		int *g_btl_weapon_allowbit;
		int *g_btl_petid;
		int *g_btl_action_done;
		int *g_btl_select_item_pos;
		int *g_btl_skill_performed;
		unsigned int *g_btl_round_endtick;
		char *g_btl_unit_base;
		ui_anim_t *g_ui_anim_base;
		int **g_ui_battle_skill_dialog;

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
		item_t *g_trade_item_base;

		void *g_pGameBase;

		int *g_logback;
		int *g_item_cur_using;
		char *g_trade_item_array;
		char *g_trade_pet_array;
		int *g_trade_gold;

		char *g_ui_craftdialog_additem_count;
		char *g_work_rebirth;//63FFFE
		char *g_work_accelerate_percent;//627E8C
		int *g_work_basedelay;
		int *g_work_start_tick;
		int *g_craft_status;//CBF15E
		int *g_craft_done_tick;
		int *g_heal_player_menu_type;
		int *g_heal_player_menu_select;
		int *g_heal_menu_type;

		short *g_item_player_menu_type;
		int *g_item_player_select;
		//int *g_ret_address;

		int *g_unit_count;
		map_unit_t *g_map_units;
		int *g_npc_dialog_show;
		int *g_npc_dialog_type;
		int *g_npc_dialog_option;
		int *g_npc_dialog_dlgid;
		int *g_npc_dialog_npcid;

		int *g_petskilldialog_select_index;
		int *g_petskilldialog_select_pet;
		void *g_ui_manager;
		void *g_ui_trade_dialog;

		short *g_map_x_bottom;
		short *g_map_y_bottom;
		short *g_map_x_size;
		short *g_map_y_size;
		short *g_map_collision_table_raw;
		short *g_map_collision_table;
		short *g_map_object_table;
		short *g_map_tile_table;
		int *g_map_index1;
		int *g_map_index2;
		int *g_map_index3;

		int *g_healing_skill_index;
		int *g_healing_subskill_index;

		short *g_is_in_team;
		short *g_team_flags;
		team_player_t *g_team_player_base;
		char *g_title_table;

		short *g_enableflags;

		HANDLE *g_mutex;
		int *g_resolution_width;
		int *g_resolution_height;
		int *g_select_big_server;
		btn_rect_t *g_select_big_server_btn;
		btn_rect_t *g_select_server_btn;
		int *g_last_login_tick;
		double *g_next_anim_tick;
		int *g_create_character_status;

		player_pers_desc_t *g_player_pers_desc;
		player_pers_desc_t *g_pers_desc;
		int *g_avatar_public_state;
		short *g_local_player_index;
	public:
		char(__cdecl *Sys_CheckModify)(const char *a1);
		void(__cdecl *COMMON_PlaySound)(int a1, int a2, int a3);
		void(__cdecl *BATTLE_PlayerAction)();
		int(__cdecl *BATTLE_GetSelectTarget)();
		void(__cdecl *NET_ParseTradeItemsPackets)(int a1, const char *buf);
		void(__cdecl *NET_ParseTradePetPackets)(int a1, int index, const char *buf);
		void(__cdecl *NET_ParseTradePetSkillPackets)(int a1, int index, const char *buf);
		void(__cdecl *NET_ParseTradeGoldPackets)(int a1, int gold);
		void(__cdecl *NET_ParseTradePlayers)(int a1, const char *src, char *src2);
		void(__cdecl *NET_ParseHealPlayers)(int a1, const char *src);
		void(__cdecl *NET_ParseHealUnits)(int a1, const char *src);
		void(__cdecl *NET_ParseItemPlayers)(int a1, const char *src);
		void(__cdecl *NET_ParseItemUnits)(int a1, const char *src);
		void(__cdecl *NET_ParseBattlePackets)(int a1, const char *buf);
		void(__cdecl *NET_ParseWorkingResult)(int a1, int success, int type, const char *buf);
		void(__cdecl *NET_ParseChatMsg)(int a1, int unitid, const char *buf, int a4, int a5);
		void(__cdecl *NET_ParseSysMsg)(int a1, const char *buf);
		void(__cdecl *NET_ParseReadyTrade)();
		void(__cdecl *NET_ParseConfirmTrade)(int a1, int a2);
		void(__cdecl *NET_ParseMeetEnemy)(int a1, int a2, int a3);
		void(__cdecl *NET_ParseDownloadMap)(int sock, int index1, int index3, int xbase, int ybase, int xtop, int ytop, const char *buf);
		void(__cdecl *NET_ParseWarp)(int a1, int index1, int index3, int xsize, int ysize, int xpos, int ypos, int a8, int a9, int a10, int a11, int a12, int warpTimes);
		void(__cdecl *NET_ParseTeamInfo)(int a1, int a2, const char *a3);
		void(__cdecl *NET_ParseTeamState)(int a1, int a2, int a3);
		void(__cdecl *NET_ParseServerBasicInfo)(int a1, int a2, int server_time, int serverIndex, int a5);
		void(__cdecl *NET_ParseLoginResult)(int a1, int a2, const char *a3);
		void(__cdecl *NET_ParseLoginResult2)(int a1, const char *a2, const char *a3);

		void(_cdecl *R_DrawText)(int a1);
		void(__cdecl *Move_Player)();

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
		void(__cdecl *NET_WriteDropGoldPacket_cgitem)(int, int, int, int);
		void(__cdecl *NET_WriteMovePacket_cgitem)(int, int, const char *);
		void(__cdecl *NET_WriteOpenHealDialog_cgitem)(int, int);
		void (__cdecl *NET_WriteWorkPacket_cgitem)(int, int, int, int, const char *);
		void(__cdecl *NET_WriteJoinTeamPacket_cgitem)(int, int, int, int);
		void(__cdecl *NET_WriteTradePacket_cgitem)(int);
		void(__cdecl *NET_WriteKickTeamPacket_cgitem)(int);
		void(__cdecl *NET_WriteTradeAddItemPacket_cgitem)(int, const char *, const char *, int);
		void(__cdecl *NET_WriteTradeConfirmPacket_cgitem)(int);
		void(__cdecl *NET_WriteTradeRefusePacket_cgitem)(int);
		void(__cdecl *NET_WriteExchangeCardPacket_cgitem)(int, int, int);
		void(__cdecl *NET_WritePKPacket_cgitem)(int, int, int);
		void(__cdecl *NET_WriteMoveItemPacket_cgitem)(int, int, int, int);
		void(__cdecl *NET_WriteMovePetPacket_cgitem)(int, int, int, int);
		void (__cdecl *NET_WriteRequestDownloadMapPacket_cgitem)(int a1, int index1, int index3, int xbot, int ybot, int xsize, int ysize);
		void(__cdecl *NET_WritePrepareCraftItemPacket_cgitem)(int, int);
		void(__cdecl *NET_WriteDropPetPacket_cgitem)(int, int, int , int);
		void(__cdecl *NET_WriteWorkMiscPacket_cgitem)(int, int, int, int);
		void(__cdecl *NET_WriteEndBattlePacket_cgitem)(int, int);
		void(__cdecl *NET_WriteChangePetStatePacket_cgitem)(int, int, int, int, int, int);
		void(__cdecl *NET_WriteUpgradePlayerPacket)(int a1, int a2);
		void(__cdecl *NET_WriteUpgradePetPacket)(int a1, int a2, int a3);
		void(__cdecl *NET_WriteChangeNickNamePacket_cgitem)(int a1, const char *a2);
		void(__cdecl *NET_WriteChangePersDescPacket_cgitem)(int a1, int sellIcon, const char *sellString, int buyIcon, const char *buyString, int wantIcon, const char *wantString, const char *descString);
		void(__cdecl *NET_WriteChangeAvatarPublicStatePacket_cgitem)(int a1, int isPublic);
		void(__cdecl *NET_WriteChangeBattlePositionPacket_cgitem)(int a1);
		void(__cdecl *NET_WriteChangeTitleNamePacket_cgitem)(int a1, int titleId);

		void(__cdecl *NPC_ShowDialogInternal)(int type, int options, int dlgid, int objid, const char *message);
		int(__cdecl *NPC_ClickDialog)(int option, int index, int a3, char a4);
		void(__cdecl *NPC_ClickExchangeDialog)(int option);
		int(__cdecl *UI_ClickNPCDialog)(int option, char flags);
		void(__cdecl *UI_OpenGatherDialog)(int skill_index, int sub_index);
		void(__cdecl *UI_OpenCraftDialog)(int skill_index, int sub_index, int sub_type);
		void(__cdecl *UI_OpenAssessDialog)(int skill_index, int sub_index);
		int(__cdecl *UI_HandleMiniDialogMouseEvent)(int widget, char flags);
		int(__cdecl *UI_IsMouseInRect)(int a1, int a2, int a3, int a4, int a5);
		int(__cdecl *UI_ButtonCheckMouse)(btn_rect_t *btn);
		int(__cdecl *UI_HandleSkillDialogCancelButtonMouseEvent)(int index, char flags);
		int(__cdecl *UI_HandleSellDialogCancelButtonMouseEvent)(int index, char flags);
		int(__cdecl *UI_HandleLearnSkillConfirmMouseEvent)(int index, char flags);
		int(__cdecl *UI_HandleForgetSkillMouseEvent)(int index, char flags);
		int(__cdecl *UI_HandleLearnPetSkillCloseButtonMouseEvent)(int index, char flags);
		int(__cdecl *UI_HandleSellDialogCloseButtonMouseEvent)(int index, char flags);
		int(__cdecl *UI_HandleEnablePlayerFlagsMouseEvent)(int index, char flags);
		int(__cdecl *UI_HandleCraftItemSlotMouseEvent)(int a1, char flags);
		int( __cdecl *UI_HandleCraftItemButtonMouseEvent)(int a1, char flags);
		int(__cdecl *UI_HandleLogbackMouseEvent)(int a1, char a2);
		int(__cdecl *UI_HandleLogoutMouseEvent)(int a1, char a2);
		int(__cdecl *IsMapObjectEntrance)(int xpos, int ypos);
		int(__cdecl *UI_PlaySwitchAnim)(int ,char, float);
		int(__cdecl *UI_HandleEnableFlags)(int a1, char a2);
		int(__cdecl *UI_SelectHealPlayer)(int menuindex, const char *menustring);
		int(__cdecl *UI_SelectItemPlayer)(int menuindex, const char *menustring);
		int(__cdecl *UI_SelectTradePlayer)(int menuindex, const char *menustring);
		int(__cdecl *UI_SelectHealUnit)(int menuindex);
		int(__cdecl *UI_SelectItemUnit)(int menuindex);
		int(__cdecl *UI_SelectTradeAddStuffs)(int a1, char a2); 
		int(__cdecl* UI_RemoveTradeItemArray)(int index);
		int(__cdecl* UI_AddTradeItemArray)(int index, int itempos);
		void(__cdecl *UI_OpenTradeDialog)(const char *playerName, int playerLevel);
		void(__cdecl *UI_SelectServer)();
		int(__cdecl *UI_SelectCharacter)(int index, int a2);
		int(__cdecl *UI_IsCharacterPresent)(int index);
		int(__cdecl *UI_ShowMessageBox)(const char *text);
		int(__cdecl *UI_ShowLostConnectionDialog)();
		int(__cdecl *UI_BattleEscape)(int index, char flags);
		int(__cdecl *UI_BattleGuard)(int index, char flags);
		int(__cdecl *UI_BattleExchangePosition)(int index, char flags);
		int(__cdecl *UI_BattleChangePet)(int index, char flags);
		int(__cdecl *UI_BattleWithdrawPet)(int index, char flags);
		int(__cdecl *UI_BattleClickChangePet)(int index, char flags);
		int(__cdecl *UI_BattleRebirth)(int index, char flags);
		int(__cdecl *UI_BattlePetSkill)(int index, char flags);
		int(__cdecl *UI_BattleOpenPetSkillDialog)(int index, char flags);
		int(__cdecl *UI_DisplayAnimFrame)(int index);
		void (__cdecl *UI_DialogShowupFrame)(int dialog);
		void(__cdecl *UI_UpdatePersDesc)(int);
		void(__cdecl *UI_GatherNextWork)(int uicore);
		void(__cdecl *SYS_ResetWindow)();
		void(__cdecl *format_mapname)(char *a1, int index1, int index2, int index3); 
		void(__cdecl *BuildMapCollisionTable)(int xbase, int ybase, int xtop, int ytop, void *celldata, void *collisiondata, void *objectdata, void *collisiontable);
		void(__cdecl *Actor_SetAnimation)(void *actor, int anim, int a3);
		void(__cdecl *Actor_Render)(void *actor, int a2);
		int(__cdecl *GetBattleUnitDistance)(void *a1, float a2, float a3);
		void(__cdecl *SetWorldStatus)(int a1);
		char *(__cdecl *V_strstr)(char *a1, const char *a2);

		//POLCN
		//41CEE0
		int (__fastcall *SendClientLogin)(void *pthis, int, int logincount, const char *acc, int acclen, const char * pwd, int pwdlen, int gametype);
		//40EA00
		int(__fastcall *OnLoginResult)(void *pthis, int dummy, int a1, int result, const char *glt, int gltlength, char **gid_array, int gid_count, int *gid_status_array, int gid_status_count, int *gid_unk1_array, int gid_unk1_count, int *gid_unk2_array, int gid_unk2_count, int *gid_unk3_array, int gid_unk3_count, int a15, int card_point);
		void(__fastcall *LaunchGame)(void *pthis, int);
		void(__fastcall *GoNext)(void *pthis, int);
		//
		void(__fastcall *vce_connect)(void *pthis, int, const char *ipaddr, u_short port);
		void *(__fastcall *vce_manager_initialize)(void *pthis);
		void (__fastcall *vce_manager_loop)(void *pthis); 
		int(__fastcall *CMainDialog_OnInitDialog)(void *pthis, int);
		int(__fastcall *CWnd_ShowWindow)(void *pthis, int, int);
		int(__fastcall *CWnd_MessageBoxA)(void *pthis, LPCSTR, LPCSTR, int);
		int(__fastcall *CDialog_DoModal)(void *pthis, int);
		void *(__fastcall *CWnd_SetFocus)(void *pthis, int);
	
		//476E28
		void *g_AppInstance;
		//476ED4
		void **g_vce_manager;
		void *g_MainCwnd;

		using typeCreateMutexA = decltype(CreateMutexA);

		typeCreateMutexA *pfnCreateMutexA;

		using typeRegisterHotKey = decltype(RegisterHotKey);

		typeRegisterHotKey *pfnRegisterHotKey;

		using typeSetActiveWindow = decltype(SetActiveWindow);

		typeSetActiveWindow *pfnSetActiveWindow;

		using typeSetFocus = decltype(SetFocus);

		typeSetFocus *pfnSetFocus;

		using typeSetForegroundWindow = decltype(SetForegroundWindow);

		typeSetForegroundWindow *pfnSetForegroundWindow;

		using typeSleep = decltype(Sleep);

		typeSleep *pfnSleep;

		using typeCreateProcessA = decltype(CreateProcessA);

		typeCreateProcessA *pfnCreateProcessA;

		using typeShell_NotifyIconA = decltype(Shell_NotifyIconA);

		typeShell_NotifyIconA *pfnShell_NotifyIconA;

		void NewBATTLE_PlayerAction();
		void NewNET_ParseTradeItemsPackets(int a1, const char *buf);
		void NewNET_ParseTradePetPackets(int a1, int index, const char *buf);
		void NewNET_ParseTradePetSkillPackets(int a1, int index, const char *buf);
		void NewNET_ParseTradeGoldPackets(int a1, int gold);
		void NewNET_ParseTradePlayers(int a1, const char *src, char *src2);
		void NewNET_ParseHealPlayers(int a1, const char *src);
		void NewNET_ParseHealUnits(int a1, const char *src);
		void NewNET_ParseItemPlayers(int a1, const char *src);
		void NewNET_ParseItemUnits(int a1, const char *src);
		void NewNET_ParseBattlePackets(int a1, const char *buf);
		void NewNET_ParseWorkingResult(int a1, int success, int type, const char *buf);
		void NewNET_ParseChatMsg(int a1, int unitid, const char *buf, int color, int size);
		void NewNET_ParseSysMsg(int a1, const char *buf);
		void NewNET_ParseReadyTrade();
		void NewNET_ParseConfirmTrade(int a1, int a2);
		void NewNET_ParseTeamState(int a1, int a2, int a3);
		
		VOID NewSleep(_In_ DWORD dwMilliseconds);

		void NewMove_Player();
		void NewNPC_ShowDialogInternal(int type, int options, int dlgid, int objid, const char *message);
		int NewUI_IsMouseInRect(int a1, int a2, int a3, int a4, int a5);
		int NewUI_HandleMiniDialogMouseEvent(int widget, char flags);
		int NewUI_HandleEnablePlayerFlagsMouseEvent(int index, char flags);
		int NewUI_HandleCraftItemSlotMouseEvent(int index, char flags);
		int NewUI_HandleCraftItemButtonMouseEvent(int index, char flags);
		void NewUI_OpenTradeDialog(const char *playerName, int playerLevel);
		void NewNET_WritePrepareCraftItemPacket_cgitem(int a1, int a2);
		void NewNET_WriteWorkPacket_cgitem(int a1, int skill, int a3, int a4, const char *buf);
		void NewUI_SelectServer();
		int NewUI_ButtonCheckMouse(btn_rect_t *btn);
		int NewUI_SelectCharacter(int index, int a2);
		int NewUI_ShowMessageBox(const char *text);
		int NewUI_ShowLostConnectionDialog();
		void NewNET_ParseDownloadMap(int sock, int index1, int index3, int xbase, int ybase, int xtop, int ytop, const char *buf);
		void NewNET_ParseWarp(int a1, int index1, int index3, int xsize, int ysize, int xpos, int ypos, int a8, int a9, int a10, int a11, int a12, int warpTimes);
		/*int NewUI_BattleEscape(int index, char flags);
		int NewUI_BattleGuard(int index, char flags);
		int NewUI_BattleExchangePosition(int index, char flags);
		int NewUI_BattleChangePet(int index, char flags);
		int NewUI_BattleWithdrawPet(int index, char flags);
		int NewUI_BattleClickChangePet(int index, char flags);
		int NewUI_BattleRebirth(int index, char flags);
		int NewUI_BattlePetSkill(int index, char flags);
		int NewUI_BattleOpenPetSkillDialog(int index, char flags);*/
		int NewUI_DisplayAnimFrame(int index);
		void NewUI_DialogShowupFrame(int dialog);
		int NewUI_PlaySwitchAnim(int a1, char a2, float a3);
		void NewUI_GatherNextWork(int uicore);
		int NewUI_SelectTradeAddStuffs(int a1, char a2);

		void ParseGatheringResult(int success, const char *buf);
		void ParseHealingResult(int success, const char *buf);
		void ParseAssessingResult(int success, const char *buf);
		void ParseCraftingResult(int success, const char *buf);

		IDirectDraw *GetDirectDraw();
		IDirectDrawSurface *GetDirectDrawBackSurface();
		void ParseBattleUnits(const char *buf);
		void DrawCustomText();
		int IsItemTypeAssessable(int type);

		void AddAllTradeItems(void);
		bool WM_BattleNormalAttack(int target);
		bool WM_BattleSkillAttack(int skillpos, int skilllv, int target);
		bool WM_BattleGuard();
		bool WM_BattleEscape();
		bool WM_BattleExchangePosition();
		bool WM_BattleChangePet(int petid);
		bool WM_BattleUseItem(int itempos, int target);
		bool WM_BattlePetSkillAttack(int skillpos, int target);
		bool WM_BattleDoNothing();
		bool WM_BattleRebirth();
		void WM_LogBack();
		void WM_LogOut();
		bool WM_DropPet(int petpos);
		bool WM_ChangePetState(int petpos, int state);
		bool WM_DropItem(int itempos);
		bool WM_UseItem(int itempos);
		bool WM_MoveItem(move_xxx_t *mov);
		bool WM_MovePet(move_xxx_t *mov);
		bool WM_MoveGold(int gold, int opt);
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
		void WM_GetBankPetsInfo(cga_pets_info_t *info);
		void WM_GetPetSkillInfo(int petid, int skillid, cga_pet_skill_info_t *info);
		void WM_GetPetSkillsInfo(int petid, cga_pet_skills_info_t *info);
		void WM_GetItemInfo(int itempos, cga_item_info_t *info);
		void WM_GetItemsInfo(cga_items_info_t *info);
		void WM_GetBankItemsInfo(cga_items_info_t *info);
		void WM_GetBattleUnit(int pos, cga_battle_unit_t *u);
		void WM_GetBattleUnits(cga_battle_units_t *u);
		void WM_GetMapUnits(cga_map_units_t *units);
		void WM_GetMapName(std::string *name);
		void WM_SayWords(const char *str, int color, int range, int size);
		bool WM_StartWork(int skill_index, int sub_index);
		bool WM_CraftItem(cga_craft_item_t *craft);
		bool WM_AssessItem(int skill_index, int itempos);
		bool WM_IsMapCellPassable(int x, int y);
		bool WM_ForceMove(int dir, bool show);
		bool WM_ForceMoveTo(int x, int y, bool show);
		void WM_GetCraftInfo(cga_craft_item_t *craft, cga_craft_info_t *info);
		void WM_GetCraftsInfo(int skill_index, cga_crafts_info_t *info);
		bool WM_DoRequest(int request_type);
		void WM_TradeAddStuffs(cga_trade_stuff_t *stuffs);
		void WM_GetTeamPlayerInfo(cga_team_players_t *info);
		void WM_FixMapWarpStuck(int type);
		void WM_GetMoveHistory(std::vector<DWORD> *v);
		bool WM_EnableFlags(int type, bool enable);
		bool WM_PlayerMenuSelect(int menuindex, const char *menustring);
		bool WM_UnitMenuSelect(int menuindex);
		void WM_SetWindowResolution(int w, int h);
		void WM_RequestDownloadMap(int xbottom, int ybottom, int xsize, int ysize);
		void WM_GetMapCollisionTableRaw(bool loadall, cga_map_cells_t *cells);
		void WM_GetMapCollisionTable(bool loadall, cga_map_cells_t *cells);
		void WM_GetMapObjectTable(bool loadall, cga_map_cells_t *cells);
		void WM_GetMapTileTable(bool loadall, cga_map_cells_t *cells);
		void WM_SendClientLogin(const char *acc, const char *pwd, int gametype);
		void WM_UpgradePlayer(int attr);
		void WM_UpgradePet(int petid, int attr);
		bool WM_ChangeNickName(const char *name);
		void WM_ChangePersDesc(cga_pers_desc_t *input);
		bool WM_ChangeTitleName(int titleId);

		bool m_initialized;
		bool m_btl_highspeed_enable;
		bool m_btl_showhpmp_enable;
		bool m_btl_double_action;
		bool m_btl_pet_skill_packet_send;
		struct
		{
			bool isdelay;
			int a1;
			char buf[4096];
			ULONG lasttick;
		}m_btl_delayanimpacket;
		int m_btl_effect_flags;
		battle_unit_t m_battle_units[20];
		int m_move_to;
		int m_move_to_x, m_move_to_y;
		std::list<DWORD> m_move_history;
		int m_move_speed;
		bool m_move_checkwarp;
		bool m_ui_minidialog_loop;
		int m_ui_minidialog_loop_index;
		int m_ui_minidialog_click_index;
		int m_ui_dialog_cancel;
		int m_ui_learn_skill_confirm;
		int m_ui_forget_skill_index;
		int m_desired_player_enable_flags;
		int m_change_player_enable_flags;
		bool m_ui_craftdialog_click_begin;
		bool m_ui_craftdialog_loop;
		int m_ui_craftdialog_loop_index;
		int m_ui_craftdialog_click_index;
		int m_ui_craftdialog_additem[6];
		int m_ui_craftdialog_additemcount;
		int m_work_acceleration;
		bool m_ui_noswitchanim;
		int m_player_menu_type;
		int m_unit_menu_type;
		bool m_work_immediate;
		int m_work_immediate_state;
		int m_work_basedelay_enforced;

		bool m_ui_selectserver_loop;
		int m_ui_selectbigserver_click_index;
		int m_ui_selectserver_click_index;
		int m_ui_selectcharacter_click_index;
		bool m_ui_auto_login;

		int m_run_game_pid;
		int m_run_game_tid;

		int m_ui_battle_action;
		struct 
		{
			int change_petid;
			int select_skill_index;
			bool select_skill_ok;
			int select_target;
		}m_ui_battle_action_param;
		HANDLE m_ui_battle_hevent;

		bool m_trade_add_all_stuffs;

		game_type m_game_type;

		HFONT m_hFont;
		ULONG_PTR m_ImageBase;
		ULONG m_ImageSize;

		char m_fakeCGSharedMem[1024];
	};
}

#define WM_CGA_BATTLE_NORMALATTACK WM_USER+10000
#define WM_CGA_BATTLE_SKILLATTACK WM_USER+10001
#define WM_CGA_BATTLE_GUARD WM_USER+10002
#define WM_CGA_BATTLE_ESCAPE WM_USER+10003
#define WM_CGA_BATTLE_USEITEM WM_USER+10004
#define WM_CGA_BATTLE_PETSKILLATTACK WM_USER+10005
#define WM_CGA_DROP_ITEM WM_USER+10006
#define WM_CGA_USE_ITEM WM_USER+10007
#define WM_CGA_MOVE_ITEM WM_USER+10008
#define WM_CGA_MOVE_PET WM_USER+10009
#define WM_CGA_MOVE_GOLD WM_USER+10010
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
#define WM_CGA_DO_REQUEST WM_USER+10042
#define WM_CGA_TRADE_ADD_STUFFS WM_USER+10043
#define WM_CGA_GET_TEAM_PLAYERS_INFO WM_USER+10044
#define WM_CGA_FIX_WARP_STUCK WM_USER+10045
#define WM_CGA_GET_MOVE_HISTORY WM_USER+10046
#define WM_CGA_ENABLE_FLAGS WM_USER+10047
#define WM_CGA_PLAYER_MENU_SELECT WM_USER+10048
#define WM_CGA_UNIT_MENU_SELECT WM_USER+10049
#define WM_CGA_SET_WINDOW_RESOLUTION WM_USER+10050
#define WM_CGA_REQUEST_DOWNLOAD_MAP WM_USER+10051
#define WM_CGA_GET_COLLISION_TABLE_RAW WM_USER+10052
#define WM_CGA_GET_COLLISION_TABLE WM_USER+10053
#define WM_CGA_GET_OBJECT_TABLE WM_USER+10054
#define WM_CGA_GET_TILE_TABLE WM_USER+10055
#define WM_CGA_GET_MAP_NAME WM_USER+10056
#define WM_CGA_DROP_PET WM_USER+10057
#define WM_CGA_CHANGE_PET_STATE WM_USER+10058
#define WM_CGA_LOGIN_GAME_SERVER WM_USER+10059
#define WM_CGA_BATTLE_DONOTHING WM_USER+10060
#define WM_CGA_BATTLE_REBIRTH WM_USER+10061
#define WM_CGA_UPGRADE_PLAYER WM_USER+10062
#define WM_CGA_UPGRADE_PET WM_USER+10063
#define WM_CGA_GET_BANK_PETS_INFO WM_USER+10064
#define WM_CGA_CHANGE_NICK_NAME WM_USER+10065
#define WM_CGA_CHANGE_TITLE_NAME WM_USER+10066
#define WM_CGA_CHANGE_PERS_DESC WM_USER+10067

#define CGA_PORT_BASE 4396