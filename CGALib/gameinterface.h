#pragma once

#include <string>
#include <functional>
#include <tuple>
#include <vector>

namespace CGA
{
	enum game_type
	{
		cg_se_3000,
		cg_se_6000,
		cg_item_6000,
	};

	typedef struct cga_player_info_s
	{
		cga_player_info_s()
		{
			hp = 0;
			maxhp = 0;
			mp = 0;
			maxmp = 0;
			xp = 0;
			maxxp = 0;
			health = 0;
			level = 0;
			gold = 0;
			unitid = 0;
			petid = -1;
			punchclock = 0;
			usingpunchclock = false;
		}

		cga_player_info_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, std::string &a14, std::string &a15)
			: hp(a1), maxhp(a2), mp(a3), maxmp(a4), xp(a5), maxxp(a6), health(a7), level(a8), gold(a9), unitid(a10), petid(a11), punchclock(a12), usingpunchclock(a13 ? true : false), name(a14), job(a15)
		{

		}

		int hp;
		int maxhp;
		int mp;
		int maxmp;
		int xp;
		int maxxp;
		int health;
		int level;
		int gold;
		int unitid;
		int petid;
		int punchclock;
		bool usingpunchclock;
		std::string name;
		std::string job;
	}cga_player_info_t;

	typedef struct cga_pet_info_s
	{
		cga_pet_info_s()
		{
			hp = 0;
			maxhp = 0;
			mp = 0;
			maxmp = 0;
			xp = 0;
			maxxp = 0;
			health = 0;
			level = 0;
			flags = 0;
			race = 0;
			battle_flags = 0;
			index = 0;
		}
		cga_pet_info_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, std::string &a13, std::string &a14)
			: hp(a1), maxhp(a2), mp(a3), maxmp(a4), xp(a5), maxxp(a6), health(a7), level(a8), flags(a9), race(a10), battle_flags(a11), index(a12), name(a13), realname(a14)
		{

		}
		int hp;
		int maxhp;
		int mp;
		int maxmp;
		int xp;
		int maxxp;
		int health;
		int level;
		int flags;
		int race;
		int battle_flags;
		int index;
		std::string name;
		std::string realname;
	}cga_pet_info_t;

	typedef std::vector<cga_pet_info_t> cga_pets_info_t;

	typedef struct cga_pet_skill_info_s
	{
		cga_pet_skill_info_s()
		{
			cost = 0;
			flags = 0;
			index = 0;
		}
		cga_pet_skill_info_s(std::string &a1, std::string &a2, int a3, int a4, int a5)
			: name(a1), info(a2), cost(a3), flags(a4), index(a5)
		{

		}
		std::string name;
		std::string info;
		int cost;
		int flags;
		int index;
	}cga_pet_skill_info_t;

	typedef std::vector<cga_pet_skill_info_t> cga_pet_skills_info_t;

	typedef struct cga_skill_info_s
	{
		cga_skill_info_s()
		{
			lv = 0;
			maxlv = 0;
			xp = 0;
			maxxp = 0;
			skill_id = 0;
			pos = 0;
			index = 0;
		}
		cga_skill_info_s(std::string &a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
			: name(a1), lv(a2), maxlv(a3), xp(a4), maxxp(a5), skill_id(a6), type(a7), pos(a8), index(a9)
		{
		}
		std::string name;
		int lv;
		int maxlv;
		int xp;
		int maxxp;
		int skill_id;
		int type;
		int pos;
		int index;
	}cga_skill_info_t;

	typedef std::vector<cga_skill_info_t> cga_skills_info_t;

	typedef struct cga_subskill_info_s
	{
		cga_subskill_info_s()
		{
			cost = 0;
			flags = 0;
			level = 0;
		}
		cga_subskill_info_s(std::string &a1, std::string &a2, int a3, int a4, int a5)
			: name(a1), info(a2), cost(a3), flags(a4), level(a5)
		{

		}
		std::string name;
		std::string info;
		int cost;
		int flags;
		int level;
	}cga_subskill_info_t;

	typedef std::vector<cga_subskill_info_t> cga_subskills_info_t;

	typedef struct cga_item_info_s
	{
		cga_item_info_s()
		{
			itemid = 0;
			count = 0;
			pos = 0;
			level = 0;
			type = 0;
			assessed = true;
		}
		cga_item_info_s(std::string &a1, std::string &a2, std::string &a3, std::string &a4, std::string &a5, int a6, int a7, int a8, int a9, int a10, bool a11)
			: name(a1), attr(a2), attr2(a3), info(a4), info2(a5), itemid(a6), count(a7), pos(a8), level(a9), type(a10), assessed(a11)
		{
		}
		std::string name;
		std::string attr;
		std::string attr2;
		std::string info;
		std::string info2;
		int itemid;
		int count;
		int pos;
		int level;
		int type;
		bool assessed;
	}cga_item_info_t;

	typedef std::vector<cga_item_info_t> cga_items_info_t;

	typedef struct cga_battle_unit_s
	{
		cga_battle_unit_s()
		{
			modelid = 0;
			level = 0;
			curhp = 0;
			maxhp = 0;
			curmp = 0;
			maxmp = 0;
			pos = 0;
		}

		cga_battle_unit_s(std::string &a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) :
			name(a1), modelid(a2), level(a3), curhp(a4), maxhp(a5), curmp(a6), maxmp(a7), pos(a8)
		{

		}
		std::string name;
		int modelid;
		int level;
		int curhp;
		int maxhp;
		int curmp;
		int maxmp;
		int pos;
	}cga_battle_unit_t;

	typedef std::vector<cga_battle_unit_t> cga_battle_units_t;

	typedef struct cga_player_menu_item_s
	{
		cga_player_menu_item_s(std::string &a1, int a2, int a3) :  name(a1), color(a2), index(a3) {}
		cga_player_menu_item_s() : color(0), index(0) {}

		std::string name;
		int color;
		int index;
	}cga_player_menu_item_t;

	typedef std::vector<cga_player_menu_item_t> cga_player_menu_items_t;

	typedef struct cga_unit_menu_item_s
	{
		cga_unit_menu_item_s() {
			level = 0;
			health = 0;
			hp = 0;
			maxhp = 0;
			mp = 0;
			maxmp = 0;
			color = 0;
			index = 0;
		}

		cga_unit_menu_item_s(std::string &a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9)  :
			name(a1), level(a2), health(a3), hp(a4), maxhp(a5), mp(a6), maxmp(a7), color(a8), index(a9)
		{
		}

		std::string name;
		int level;
		int health;
		int hp;
		int maxhp;
		int mp;
		int maxmp;
		int color;
		int index;
	}cga_unit_menu_item_t;

	typedef std::vector<cga_unit_menu_item_t> cga_unit_menu_items_t;

	typedef struct cga_npc_dialog_s
	{
		cga_npc_dialog_s()
		{
			type = 0;
			options = 0;
			dialog_id = 0;
			npc_id = 0;
		}
		cga_npc_dialog_s(int a1, int a2, int a3, int a4, std::string &a5)
			: type(a1), options(a2), dialog_id(a3), npc_id(a4), message(a5)
		{

		}
		int type;
		int options;
		int dialog_id;
		int npc_id;
		std::string message;
	}cga_npc_dialog_t;

	typedef struct cga_map_unit_s
	{
		cga_map_unit_s()
		{
			valid = 0;
			type = 0;
			unit_id = 0;
			model_id = 0;
			xpos = 0;
			ypos = 0;
			item_count = 0;
		}
		cga_map_unit_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, std::string &a8, std::string &a9, std::string &a10)
			: valid(a1), type(a2), unit_id(a3), model_id(a4), xpos(a5), ypos(a6), item_count(a7), unit_name(a8), nick_name(a9), item_name(a10)
		{

		}
		int valid;// if 0 invalid
		int type;
		int unit_id;
		int model_id;
		int xpos;
		int ypos;
		int item_count;
		std::string unit_name;
		std::string nick_name;
		std::string item_name;
	}cga_map_unit_t;

	typedef std::vector<cga_map_unit_t> cga_map_units_t;

	typedef struct cga_sell_item_s
	{
		cga_sell_item_s() 
		{
			itemid = 0;
			itempos = 0;
			count = 0;
		}
		cga_sell_item_s(int a1, int a2, int a3) : itemid(a1), itempos(a2), count(a3)
		{
		}
		int itemid;
		int itempos;
		int count;
	}cga_sell_item_t;

	typedef std::vector<cga_sell_item_t> cga_sell_items_t;

	typedef struct cga_buy_item_s
	{
		cga_buy_item_s()
		{
			index = 0;
			count = 0;
		}
		cga_buy_item_s(int a1, int a2) : index(a1), count(a2)
		{
		}
		int index;
		int count;
	}cga_buy_item_t;

	typedef std::vector<cga_buy_item_t> cga_buy_items_t;

	typedef struct cga_craft_item_s
	{
		cga_craft_item_s()
		{
			skill_index = 0;
			subskill_index = 0;
			sub_type = 0;
			memset(itempos, 0, sizeof(itempos));
		}
		cga_craft_item_s(int a1, int a2, int a3) : skill_index(a1), subskill_index(a2), sub_type(a3)
		{
		}
		cga_craft_item_s(int a1, int a2, int a3, int a4[]) : skill_index(a1), subskill_index(a2), sub_type(a3)
		{
			memcpy(itempos, a4, sizeof(itempos));
		}
		int skill_index;
		int subskill_index;
		int sub_type;
		int itempos[5];
	}cga_craft_item_t;

	typedef struct cga_working_result_s
	{
		cga_working_result_s()
		{
			type = -1;
			success = false;
			xp = 0;
			count = 0;
			levelup = false;
			endurance = 0;
			skillful = 0;
			intelligence = 0;
			imgid = 0;
		}
		cga_working_result_s(int a1, bool a2, bool a3, int a4, int a5, int a6, int a7)
			: type(a1), success(a2), levelup(a3), xp(a4), endurance(a5), skillful(a6), intelligence(a7)
		{
		}
		int type;
		bool success;
		bool levelup;
		int xp;
		int count;		
		int endurance;
		int skillful;
		int intelligence;
		union {
			int value;
			int	imgid;
			int status;
		};
		union {
			int value2;
			int	count;
			int unk;
		};
		std::string name;
	}cga_working_result_t;
	
	typedef struct cga_craft_material_s
	{
		cga_craft_material_s()
		{
			itemid = 0;
			count = 0;
		}
		cga_craft_material_s(int a1, int a2, std::string &a3): itemid(a1), count(a2), name(a3)
		{

		}
		int itemid;
		int count;
		std::string name;
	}cga_craft_material_t;

	typedef struct cga_craft_info_s
	{
		cga_craft_info_s()
		{
			id = 0;
			cost = 0;
			level = 0;
			itemid = 0;
			index = -1;
		}
		cga_craft_info_s(int a1, int a2, int a3, int a4, int a5, std::string &a6, std::string &a7) : id(a1), cost(a2), level(a3), itemid(a4), index(a5), name(a6), info(a7)
		{
			
		}
		int id;
		int cost;
		int level;
		int itemid;
		int index;
		std::string name;
		std::string info;
		cga_craft_material_t materials[5];
	}cga_craft_info_t;

	typedef std::vector<cga_craft_info_t> cga_crafts_info_t;

	typedef struct cga_chat_msg_s
	{
		cga_chat_msg_s()
		{
			unitid = 0;
			color = 0;
			size = 0;
		}
		cga_chat_msg_s(int a1, std::string &a2, int a3, int a4)
			: unitid(a1), msg(a2), color(a3), size(a4)
		{
		}
		int unitid;
		std::string msg;
		int color;
		int size;
	}cga_chat_msg_t;

	typedef struct CGAShare_s
	{
		int ProcessId;
		int ThreadId;
		int hWnd;
		int Port;
	}CGAShare_t;

	typedef struct cga_game_data_s 
	{
		int reserved;
	}cga_game_data_t;

#define FL_BATTLE_ACTION_ISPLAYER 1
#define FL_BATTLE_ACTION_ISDOUBLE 2
#define FL_BATTLE_ACTION_ISSKILLPERFORMED 4

#define FL_SKILL_SINGLE 0x40
#define FL_SKILL_MULTI 0x80
#define FL_SKILL_ALL 0x100
#define FL_SKILL_BOOM 0x200

#define PLAYER_ENABLE_FLAG_PK 0
#define PLAYER_ENABLE_FLAG_TEAMCHAT 1
#define PLAYER_ENABLE_FLAG_JOINTEAM 2
#define PLAYER_ENABLE_FLAG_CARD 3
#define PLAYER_ENABLE_FLAG_TRADE 4
#define PLAYER_ENABLE_FLAG_FAMILY 5

#define WORK_TYPE_CRAFTING 1
#define WORK_TYPE_ASSESSING 2
#define WORK_TYPE_HEALING 3
#define WORK_TYPE_GATHERING 4

	class CGAInterface
	{
	public:
		virtual bool IsConnected() = 0;
		virtual void Disconnect() = 0;
		virtual bool Connect(int port) = 0;
		virtual bool Initialize(cga_game_data_t &data) = 0;

		virtual bool IsInGame(int &ingame) = 0;
		virtual bool GetWorldStatus(int &status) = 0;
		virtual bool GetGameStatus(int &status) = 0;
		virtual bool GetPlayerInfo(cga_player_info_t &info) = 0;
		virtual bool SetPlayerFlagEnabled(int index, bool enable) = 0;
		virtual bool IsPlayerFlagEnabled(int index, bool &enable) = 0;

		virtual bool IsPetValid(int petid, bool &valid) = 0;
		virtual bool GetPetInfo(int petid, cga_pet_info_t &info) = 0;
		virtual bool GetPetsInfo(cga_pets_info_t &info) = 0;
		virtual bool IsPetSkillValid(int petid, int skillid, bool &valid) = 0;
		virtual bool GetPetSkillInfo(int petid, int skillid, cga_pet_skill_info_t &skill) = 0;
		virtual bool GetPetSkillsInfo(int petid, cga_pet_skills_info_t &skills) = 0;

		virtual bool IsSkillValid(int skillid, bool &valid) = 0;
		virtual bool GetSkillInfo(int skillid, cga_skill_info_t &info) = 0;
		virtual bool GetSkillsInfo(cga_skills_info_t &info) = 0;
		virtual bool GetSubSkillInfo(int id, int stage, cga_subskill_info_t &info) = 0;
		virtual bool GetSubSkillsInfo(int id, cga_subskills_info_t &info) = 0;

		virtual bool IsItemValid(int itempos, bool &valid) = 0;
		virtual bool GetItemInfo(int itempos, cga_item_info_t &info) = 0;
		virtual bool GetItemsInfo(cga_items_info_t &info) = 0;
		virtual bool GetBankItemsInfo(cga_items_info_t &info) = 0;
		virtual bool DropItem(int itempos, bool &result) = 0;
		virtual bool MoveItem(int itempos, int dstpos, int count, bool &result) = 0;
		virtual bool GetMapXY(int &x, int &y) = 0;
		virtual bool GetMapXYFloat(float &x, float &y) = 0;
		virtual bool GetMoveSpeed(float &x, float &y) = 0;
		virtual bool GetMapName(std::string &name) = 0;
		virtual bool GetMapUnits(cga_map_units_t &units) = 0;

		virtual bool WalkTo(int x, int y) = 0;
		virtual bool TurnTo(int x, int y) = 0;
		virtual bool SetMoveSpeed(int speed) = 0;
		virtual bool ForceMove(int dir, bool show, bool &result) = 0;
		virtual bool ForceMoveTo(int x, int y, bool show, bool &result) = 0;
		virtual bool IsMapCellPassable(int x, int y, bool &result) = 0;
		virtual bool LogBack() = 0;
		virtual bool LogOut() = 0;
		virtual bool SayWords(std::string &str, int, int, int) = 0;

		virtual bool UseItem(int itempos, bool &result) = 0;
        virtual bool ClickNPCDialog(int option, int index, bool &result) = 0;
		virtual bool SellNPCStore(cga_sell_items_t &items, bool &result) = 0;
		virtual bool BuyNPCStore(cga_buy_items_t &items, bool &result) = 0;
		virtual bool PlayerMenuSelect(int menuindex, bool &result) = 0;
		virtual bool UnitMenuSelect(int menuindex, bool &result) = 0;

		virtual bool IsBattleUnitValid(int pos, bool &valid) = 0;
		virtual bool GetBattleUnit(int pos, cga_battle_unit_t &unit) = 0;
		virtual bool GetBattleUnits(cga_battle_units_t &units) = 0;
		virtual bool GetBattleRoundCount(int &count) = 0;
		virtual bool GetBattlePlayerPosition(int &pos) = 0;
		virtual bool GetBattlePlayerStatus(int &status) = 0;
		virtual bool GetBattlePetId(int &petid) = 0;

		virtual bool BattleNormalAttack(int target, bool &result) = 0;
		virtual bool BattleSkillAttack(int skillpos, int skilllv, int target, bool &result) = 0;
		virtual bool BattleDefense(bool &result) = 0;
		virtual bool BattleEscape(bool &result) = 0;
		virtual bool BattleExchangePosition(bool &result) = 0;
		virtual bool BattleChangePet(int petid, bool &result) = 0;
		virtual bool BattleUseItem(int itempos, int target, bool &result) = 0;
		virtual bool BattlePetSkillAttack(int skillpos, int target, bool &result) = 0;
		virtual bool BattleSetHighSpeedEnabled(bool enable) = 0;
		virtual bool BattleSetShowHPMPEnabled(bool enable) = 0;
		virtual bool GetBattleEndTick(int &msec) = 0;
		virtual bool SetBattleEndTick(int msec) = 0;
		virtual bool SetWorkDelay(int delay) = 0;
		virtual bool SetWorkAcceleration(int percent) = 0;
		virtual bool StartWork(int skill_index, int sub_index, bool &result) = 0;
		virtual bool CraftItem(cga_craft_item_t &craft, bool &result) = 0;
		virtual bool AssessItem(int skill_index, int itempos, bool &result) = 0;
		virtual bool GetCraftInfo(int skill_index, int sub_index, cga_craft_info_t &info) = 0;
		virtual bool GetCraftsInfo(int skill_index, cga_crafts_info_t &info) = 0;

		virtual bool RegisterServerShutdownNotify(const std::function<void(int)> &callback) = 0;
		virtual bool RegisterBattleActionNotify(const std::function<void(int)> &callback) = 0;
		virtual bool RegisterPlayerMenuNotify(const std::function<void(cga_player_menu_items_t)> &callback) = 0;
		virtual bool RegisterUnitMenuNotify(const std::function<void(cga_unit_menu_items_t)> &callback) = 0;
		virtual bool RegisterNPCDialogNotify(const std::function<void(cga_npc_dialog_t)> &callback) = 0;
		virtual bool RegisterWorkingResultNotify(const std::function<void(cga_working_result_t)> &callback) = 0;
		virtual bool RegisterChatMsgNotify(const std::function<void(cga_chat_msg_t)> &callback) = 0;
	};

	extern "C" CGAInterface *CreateInterface();

}
