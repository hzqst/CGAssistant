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
		polcn
	};

	typedef struct cga_pers_desc_s
	{
		cga_pers_desc_s()
		{
			changeBits = 0;
			sellIcon = 0;
			buyIcon = 0;
			wantIcon = 0;
		}
		cga_pers_desc_s(int a0, int a1, const std::string &a2, int a3, const std::string &a4, int a5, const std::string &a6, const std::string &a7)
		{
			changeBits = a0;
			sellIcon = a1;
			sellString = a2;
			buyIcon = a3;
			buyString = a4;
			wantIcon = a5;
			wantString = a6;
			descString = a7;
		}

		cga_pers_desc_s(int a0, int a1, const char *a2, int a3, const char *a4, int a5, const char *a6, const char *a7)
		{
			changeBits = a0;
			sellIcon = a1;
			sellString = a2;
			buyIcon = a3;
			buyString = a4;
			wantIcon = a5;
			wantString = a6;
			descString = a7;
		}

		int changeBits;
		int sellIcon;
		std::string sellString;
		int buyIcon;
		std::string buyString;
		int wantIcon;
		std::string wantString;
		std::string descString;
	}cga_pers_desc_t;

	typedef struct cga_playerpet_detail_info_s
	{
		cga_playerpet_detail_info_s()
		{
			points_remain = 0;
			points_endurance = 0;
			points_strength = 0;
			points_defense = 0;
			points_agility = 0;
			points_magical = 0;
			value_attack = 0;
			value_defensive = 0;
			value_agility = 0;
			value_spirit = 0;
			value_recovery = 0;
			resist_poison = 0;
			resist_sleep = 0;
			resist_medusa = 0;
			resist_drunk = 0;
			resist_chaos = 0;
			resist_forget = 0;
			fix_critical = 0;
			fix_strikeback = 0;
			fix_accurancy = 0;
			fix_dodge = 0;
			element_earth = 0;
			element_water = 0;
			element_fire = 0;
			element_wind = 0;
		}
		cga_playerpet_detail_info_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10,
			int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20,
			int a21, int a22, int a23, int a24, int a25) :
			points_remain(a1), points_endurance(a2), points_strength(a3), points_defense(a4), points_agility(a5), points_magical(a6),
			value_attack(a7), value_defensive(a8), value_agility(a9), value_spirit(a10), value_recovery(a11),
			resist_poison(a12), resist_sleep(a13), resist_medusa(a14), resist_drunk(a15), resist_chaos(a16), resist_forget(a17),
			fix_critical(a18), fix_strikeback(a19), fix_accurancy(a20), fix_dodge(a21),
			element_earth(a22), element_water(a23), element_fire(a24), element_wind(a25)
		{

		}
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
		int element_earth;
		int element_water;
		int element_fire;
		int element_wind;
	}cga_playerpet_detail_info_t;

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
			souls = 0;
			level = 0;
			gold = 0;
			unitid = 0;
			petid = -1;
			punchclock = 0;
			usingpunchclock = false;
			petriding = false;
			manu_endurance = 0;
			manu_skillful = 0;
			manu_intelligence = 0;
			value_charisma = 0;
		}

		cga_player_info_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, bool a16, bool a17, std::string &a18, std::string &a19, const cga_playerpet_detail_info_t &a20, int a21, int a22, int a23, int a24)
			: hp(a1), maxhp(a2), mp(a3), maxmp(a4), xp(a5), maxxp(a6), health(a7), souls(a8), level(a9), gold(a10), unitid(a11), petid(a12), direction(a13), battle_position(a14), punchclock(a15), usingpunchclock(a16), petriding(a17), name(a18), job(a19), detail(a20), manu_endurance(a21), manu_skillful(a22), manu_intelligence(a23), value_charisma(a24)
		{

		}

		int hp;
		int maxhp;
		int mp;
		int maxmp;
		int xp;
		int maxxp;
		int health;
		int souls;
		int level;
		int gold;
		int unitid;
		int petid;
		int direction;
		int battle_position;
		int punchclock;
		bool usingpunchclock;
		bool petriding;
		std::string name;
		std::string job;
		std::vector<std::string> titles;
		cga_playerpet_detail_info_t detail;
		cga_pers_desc_t persdesc;
		int manu_endurance;
		int manu_skillful;
		int manu_intelligence;
		int value_charisma;
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
			loyality = 0;
			battle_flags = 0;
			state = 0;
			index = 0;
		}
		cga_pet_info_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, 
			int a11, int a12, int a13, int a14, std::string &a15, std::string &a16, const cga_playerpet_detail_info_t &a17)
			: hp(a1), maxhp(a2), mp(a3), maxmp(a4), xp(a5), maxxp(a6), health(a7), level(a8), flags(a9), race(a10), 
			loyality(a11), battle_flags(a12), state(a13), index(a14), name(a15), realname(a16), detail(a17)
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
		int loyality;
		int battle_flags;
		int state;
		int index;
		std::string name;
		std::string realname;
		cga_playerpet_detail_info_t detail;
	}cga_pet_info_t;

	typedef std::vector<cga_pet_info_t> cga_pets_info_t;

	typedef struct cga_trade_pet_info_s
	{
		cga_trade_pet_info_s()
		{
			level = 0;
			race = 0;
			maxhp = 0;
			maxmp = 0;
			loyality = 0;
			skill_count = 0;
			image_id = 0;
			index = 0;
		}
		cga_trade_pet_info_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, 
			std::string &a9, std::string &a10, const cga_playerpet_detail_info_t &a11)
			: level(a1), race(a2), maxhp(a3), maxmp(a4), loyality(a5), skill_count(a6), image_id(a7), index(a8), 
			name(a9), realname(a10), detail(a11)
		{

		}
		int level;
		int race;
		int maxhp;
		int maxmp;
		int loyality;
		int skill_count;
		int image_id;
		int index;
		std::string name;
		std::string realname;
		cga_playerpet_detail_info_t detail;
	}cga_trade_pet_info_t;

	typedef struct cga_trade_pet_skill_info_s
	{
		cga_trade_pet_skill_info_s()
		{
			index = 0;
		}
		cga_trade_pet_skill_info_s(int a1)
		{
                        index = a1;
		}
		int index;
		std::vector<std::string> skills;
	}cga_trade_pet_skill_info_t;

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
			slotsize = 0;
		}
		cga_skill_info_s(std::string &a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10)
                        : name(a1), lv(a2), maxlv(a3), xp(a4), maxxp(a5), skill_id(a6), type(a7), pos(a8), index(a9), slotsize(a10)
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
		int slotsize;
	}cga_skill_info_t;

	typedef std::vector<cga_skill_info_t> cga_skills_info_t;

	typedef struct cga_subskill_info_s
	{
		cga_subskill_info_s()
		{
			cost = 0;
			flags = 0;
			level = 0;
			available = false;
		}
		cga_subskill_info_s(std::string &a1, std::string &a2, int a3, int a4, int a5, bool a6)
			: name(a1), info(a2), cost(a3), flags(a4), level(a5), available(a6)
		{

		}
		std::string name;
		std::string info;
		int cost;
		int flags;
		int level;
		bool available;
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
		cga_item_info_s(std::string &a1, std::string &a2, std::string &a3, int a4, int a5, int a6, int a7, int a8, bool a9)
			: name(a1), attr(a2), info(a3), itemid(a4), count(a5), pos(a6), level(a7), type(a8), assessed(a9)
		{
		}
		std::string name;
		std::string attr;
		std::string info;
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
			flags = 0;
			pos = 0;
		}

		cga_battle_unit_s(std::string &a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9) :
			name(a1), modelid(a2), level(a3), curhp(a4), maxhp(a5), curmp(a6), maxmp(a7), flags(a8), pos(a9)
		{

		}
		std::string name;
		int modelid;
		int level;
		int curhp;
		int maxhp;
		int curmp;
		int maxmp;
		int flags;
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
			injury = 0;
			icon = 0;
			level = 0;
		}
		cga_map_unit_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, std::string &a12, std::string &a13, std::string &a14, std::string &a15)
			: valid(a1), type(a2), unit_id(a3), model_id(a4), xpos(a5), ypos(a6), item_count(a7), injury(a8), icon(a9), level(a10), flags(a11), unit_name(a12), nick_name(a13), title_name(a14), item_name(a15)
		{

		}
		int valid;// if 0 invalid
		int type;
		int unit_id;
		int model_id;
		int xpos;
		int ypos;
		int item_count;
		int injury;
		int icon;
		int level;
		int flags;
		std::string unit_name;
		std::string nick_name;
		std::string title_name;
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

	typedef std::vector<int> cga_sell_pets_t;

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
			for (int i = 0; i < 6; ++i)
				itempos[i] = -1;
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
		int itempos[6];
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
			available = false;
		}
		cga_craft_info_s(int a1, int a2, int a3, int a4, int a5, std::string &a6, std::string &a7, bool a8) : id(a1), cost(a2), level(a3), itemid(a4), index(a5), name(a6), info(a7), available(a8)
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
		bool available;
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

	typedef struct cga_trade_stuff_s
	{
		cga_trade_stuff_s()
		{
			gold = 0;
		}

		cga_sell_items_t items;
		cga_sell_pets_t pets;
		int gold;
	}cga_trade_stuff_t;

	typedef struct cga_trade_stuff_info_s
	{
		cga_trade_stuff_info_s()
		{
			gold = 0;
			type = 0;
		}
		cga_trade_stuff_info_s(int a1) : type (a1)
		{
			gold = 0;
		}

		cga_items_info_t items;
		cga_trade_pet_info_t pet;
		cga_trade_pet_skill_info_t petskills;
		int gold;
		int type;
	}cga_trade_stuff_info_t;

	typedef struct cga_team_player_s
	{
		cga_team_player_s()
		{
			unit_id = 0;
			hp = 0;
			maxhp = 0;
			mp = 0;
			maxmp = 0;
			xpos = 0;
			ypos = 0;
		}
		cga_team_player_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, const std::string &a8) : unit_id(a1), hp(a2), maxhp(a3), mp(a4), maxmp(a5), xpos(a6), ypos(a7), name(a8)
		{

		}
		int unit_id;
		int hp;
		int maxhp;
		int mp;
		int maxmp;
		int xpos;
		int ypos;
		std::string name;
	}cga_team_player_t;

	typedef std::vector<cga_team_player_t> cga_team_players_t;

	typedef struct cga_map_cells_s
	{
		cga_map_cells_s()
		{
			x_bottom = 0;
			y_bottom = 0;
			x_size = 0;
			y_size = 0;
		}
		cga_map_cells_s(int a1, int a2, int a3, int a4, short *a5) : x_bottom(a1), y_bottom(a2), x_size(a3), y_size(a4)
		{
			cell.resize(a3 * a4);
			memcpy(cell.data(), a5, sizeof(short) * a3 * a4);
		}
		int x_bottom;
		int y_bottom;
		int x_size;
		int y_size;
		std::vector<short> cell;
	}cga_map_cells_t;

	typedef struct cga_trade_dialog_s
	{
		cga_trade_dialog_s()
		{
			level = 0;
		}
		cga_trade_dialog_s(std::string &a1, int a2)
			: name(a1), level(a2)
		{
		}
		std::string name;
		int level;
	}cga_trade_dialog_t;

	typedef struct cga_battle_context_s
	{
		cga_battle_context_s() {


		}
		cga_battle_context_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9) :
			round_count(a1), player_pos(a2), player_status(a3), skill_performed(a4), skill_allowbit(a5), petskill_allowbit(a6), weapon_allowbit(a7), petid(a8), effect_flags(a9)
		{

		}
		int round_count;
		int player_pos;
		int player_status;
		int skill_performed;
		int skill_allowbit;
		int petskill_allowbit;
		int weapon_allowbit;
		int petid;
		int effect_flags;
	}cga_battle_context_t;

	typedef struct cga_download_map_s
	{
		cga_download_map_s()
		{
			index1 = 0;
			index3 = 0;
			xbase = 0;
			ybase = 0;
			xtop = 0;
			ytop = 0;
		}
		cga_download_map_s(int a1, int a2, int a3, int a4, int a5, int a6)
			: index1(a1), index3(a2), xbase(a3), ybase(a4), xtop(a5), ytop(a6)
		{
		}
		int index1;
		int index3;
		int xbase;
		int ybase;
		int xtop;
		int ytop;
	}cga_download_map_t;

	typedef struct cga_conn_state_s
	{
		cga_conn_state_s()
		{
			state = 0;
		}
		cga_conn_state_s(int a1, const std::string &a2)
			: state(a1), msg(a2)
		{
		}
		int state;
		std::string msg;
	}cga_conn_state_t;

	typedef struct cga_sys_time_s
	{
		cga_sys_time_s()
		{
			years = 0;
			month = 0;
			days = 0;
			hours = 0;
			mins = 0;
			secs = 0;
			local_time = 0;
			server_time = 0;
		}

		cga_sys_time_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) : years(a1), month(a2), days(a3), hours(a4), mins(a5), secs(a6), local_time(a7), server_time(a8)
		{

		}

		int years;
		int month;
		int days;
		int hours;
		int mins;
		int secs;
		int local_time;
		int server_time;
	}cga_sys_time_t;

	typedef struct CGAShare_s
	{
		int ProcessId;
		int ThreadId;
		int hWnd;
		int Port;
		int GameType;
	}CGAShare_t;

	typedef struct cga_game_data_s 
	{
		int reserved;
	}cga_game_data_t;

#define TRADE_STUFFS_ITEM 1
#define TRADE_STUFFS_PET 2
#define TRADE_STUFFS_PETSKILL 3
#define TRADE_STUFFS_GOLD 4

#define TRADE_STATE_CANCEL 0
#define TRADE_STATE_READY 1
#define TRADE_STATE_CONFIRM 2
#define TRADE_STATE_SUCCEED 3

#define FL_BATTLE_ACTION_ISPLAYER 1
#define FL_BATTLE_ACTION_ISDOUBLE 2
#define FL_BATTLE_ACTION_ISSKILLPERFORMED 4
#define FL_BATTLE_ACTION_END 8
#define FL_BATTLE_ACTION_BEGIN 16

#define FL_SKILL_SELECT_TARGET 0x1
#define FL_SKILL_SELECT_DEAD 0x2
#define FL_SKILL_TO_PET 0x4
#define FL_SKILL_TO_SELF 0x8
#define FL_SKILL_TO_TEAMMATE 0x10
#define FL_SKILL_TO_ENEMY 0x20
#define FL_SKILL_SINGLE 0x40
#define FL_SKILL_MULTI 0x80
#define FL_SKILL_ALL 0x100
#define FL_SKILL_BOOM 0x200
#define FL_SKILL_FRONT_ONLY 0x400

#define FL_DEBUFF_SLEEP 0x20
#define FL_DEBUFF_MEDUSA 0x40
#define FL_DEBUFF_DRUNK 0x80
#define FL_DEBUFF_CHAOS 0x100
#define FL_DEBUFF_FORGET 0x200
#define FL_DEBUFF_POISON 0x400
#define FL_DEBUFF_ANY (FL_DEBUFF_SLEEP | FL_DEBUFF_MEDUSA | FL_DEBUFF_DRUNK | FL_DEBUFF_CHAOS | FL_DEBUFF_FORGET | FL_DEBUFF_POISON)

#define ENABLE_FLAG_PK 0
#define ENABLE_FLAG_TEAMCHAT 1
#define ENABLE_FLAG_JOINTEAM 2
#define ENABLE_FLAG_CARD 3
#define ENABLE_FLAG_TRADE 4
#define ENABLE_FLAG_FAMILY 5
#define ENABLE_FLAG_AVATAR_PUBLIC 100
#define ENABLE_FLAG_BATTLE_POSITION 101

#define WORK_TYPE_CRAFTING 1
#define WORK_TYPE_ASSESSING 2
#define WORK_TYPE_HEALING 3
#define WORK_TYPE_GATHERING 4
#define WORK_TYPE_PET 5

#define REQUEST_TYPE_PK 1
#define REQUEST_TYPE_JOINTEAM 3
#define REQUEST_TYPE_EXCAHNGECARD 4
#define REQUEST_TYPE_TRADE 5
#define REQUEST_TYPE_KICKTEAM 11
#define REQUEST_TYPE_LEAVETEAM 12
#define REQUEST_TYPE_TRADE_CONFIRM 13
#define REQUEST_TYPE_TRADE_REFUSE 14
#define REQUEST_TYPE_TEAM_CHAT 15
#define REQUEST_TYPE_REBIRTH_ON 16
#define REQUEST_TYPE_REBIRTH_OFF 17

#define PLAYER_MENU_HEAL 1
#define PLAYER_MENU_ITEM 2
#define PLAYER_MENU_TRADE 3

#define UNIT_MENU_HEAL 1
#define UNIT_MENU_ITEM 2

#define MOVE_GOLD_TOBANK 1
#define MOVE_GOLD_FROMBANK 2
#define MOVE_GOLD_DROP 3

#define UI_DIALOG_TRADE 1
#define UI_DIALOG_BATTLE_SKILL 2

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
		virtual bool GetBGMIndex(int &index) = 0;
		virtual bool GetSysTime(cga_sys_time_t &t) = 0;
		virtual bool GetPlayerInfo(cga_player_info_t &info) = 0;
		virtual bool SetPlayerFlagEnabled(int index, bool enable) = 0;
		virtual bool IsPlayerFlagEnabled(int index, bool &enable) = 0;

		virtual bool IsPetValid(int petid, bool &valid) = 0;
		virtual bool GetPetInfo(int petid, cga_pet_info_t &info) = 0;
		virtual bool GetPetsInfo(cga_pets_info_t &info) = 0;
		virtual bool GetBankPetsInfo(cga_pets_info_t &info) = 0;
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
		virtual bool GetBankGold(int &gold) = 0;
		virtual bool MoveItem(int itempos, int dstpos, int count, bool &result) = 0;
		virtual bool MovePet(int itempos, int dstpos, bool &result) = 0;
		virtual bool MoveGold(int gold, int opt, bool &result) = 0;
		virtual bool DropItem(int itempos, bool &result) = 0;
		virtual bool DropPet(int petpos, bool &result) = 0;
		virtual bool ChangePetState(int petpos, int state, bool &result) = 0;
		virtual bool GetMapIndex(int &index1, int &index2, int &index3) = 0;
		virtual bool GetMapXY(int &x, int &y) = 0;
		virtual bool GetMapXYFloat(float &x, float &y) = 0;
		virtual bool GetMoveSpeed(float &x, float &y) = 0;
		virtual bool GetMapName(std::string &name) = 0;
		virtual bool GetMapUnits(cga_map_units_t &units) = 0;
		virtual bool GetMapCollisionTable(bool loadall, cga_map_cells_t &cells) = 0;
		virtual bool GetMapCollisionTableRaw(bool loadall, cga_map_cells_t &cells) = 0;
		virtual bool GetMapObjectTable(bool loadall, cga_map_cells_t &cells) = 0;
		virtual bool GetMapTileTable(bool loadall, cga_map_cells_t &cells) = 0;

		virtual bool WalkTo(int x, int y) = 0;
		virtual bool TurnTo(int x, int y) = 0;
		virtual bool SetMoveSpeed(int speed) = 0;
		virtual bool ForceMove(int dir, bool show, bool &result) = 0;
		virtual bool ForceMoveTo(int x, int y, bool show, bool &result) = 0;
		virtual bool IsMapCellPassable(int x, int y, bool &result) = 0;
		virtual bool LogBack() = 0;
		virtual bool LogOut() = 0;
		virtual bool SayWords(std::string &str, int, int, int) = 0;
		virtual bool ChangeNickName(std::string &str, bool &result) = 0;
		virtual bool ChangeTitleName(int titleId, bool &result) = 0;
		virtual bool ChangePersDesc(CGA::cga_pers_desc_t &desc) = 0;

		virtual bool UseItem(int itempos, bool &result) = 0;
        virtual bool ClickNPCDialog(int option, int index, bool &result) = 0;
		virtual bool SellNPCStore(cga_sell_items_t &items, bool &result) = 0;
		virtual bool BuyNPCStore(cga_buy_items_t &items, bool &result) = 0;
		virtual bool PlayerMenuSelect(int menuindex, std::string &menustring, bool &result) = 0;
		virtual bool UnitMenuSelect(int menuindex, bool &result) = 0;
		virtual bool UpgradePlayer(int attr) = 0;
		virtual bool UpgradePet(int petid, int attr) = 0;

		virtual bool IsBattleUnitValid(int pos, bool &valid) = 0;
		virtual bool GetBattleUnit(int pos, cga_battle_unit_t &unit) = 0;
		virtual bool GetBattleUnits(cga_battle_units_t &units) = 0;
		virtual bool GetBattleContext(cga_battle_context_t &ctx) = 0;

		virtual bool BattleNormalAttack(int target, bool &result) = 0;
		virtual bool BattleSkillAttack(int skillpos, int skilllv, int target, bool packetOnly, bool &result) = 0;
		virtual bool BattleRebirth(bool &result) = 0;
		virtual bool BattleGuard(bool &result) = 0;
		virtual bool BattleEscape(bool &result) = 0;
		virtual bool BattleExchangePosition(bool &result) = 0;
		virtual bool BattleChangePet(int petid, bool &result) = 0;
		virtual bool BattleUseItem(int itempos, int target, bool &result) = 0;
		virtual bool BattlePetSkillAttack(int skillpos, int target, bool packetOnly, bool &result) = 0;
		virtual bool BattleDoNothing(bool &result) = 0;
		virtual bool BattleSetHighSpeedEnabled(bool enable) = 0;
		virtual bool BattleSetShowHPMPEnabled(bool enable) = 0;
		virtual bool GetBattleEndTick(int &msec) = 0;
		virtual bool SetBattleEndTick(int msec) = 0;
		virtual bool SetWorkDelay(int delay) = 0;
		virtual bool SetWorkAcceleration(int percent) = 0;
		virtual bool SetImmediateDoneWork(bool enable) = 0;
		virtual bool GetImmediateDoneWorkState(int &state) = 0;
		virtual bool StartWork(int skill_index, int sub_index, bool &result) = 0;
		virtual bool CraftItem(cga_craft_item_t &craft, bool &result) = 0;
		virtual bool AssessItem(int skill_index, int itempos, bool &result) = 0;
		virtual bool GetCraftInfo(int skill_index, int sub_index, cga_craft_info_t &info) = 0;
		virtual bool GetCraftsInfo(int skill_index, cga_crafts_info_t &info) = 0;

		virtual bool GetCraftStatus(int &status) = 0;
		virtual bool DoRequest(int request_type, bool &result) = 0;
		virtual bool EnableFlags(int type, bool enable, bool &result) = 0;
		virtual bool TradeAddStuffs(cga_sell_items_t items, cga_sell_pets_t pets, int gold) = 0;
		virtual bool GetTeamPlayerInfo(cga_team_players_t &result) = 0;

		virtual bool FixMapWarpStuck(int type) = 0;
		virtual bool SetNoSwitchAnim(bool enable) = 0;
        virtual bool GetMoveHistory(std::vector<unsigned long> &v) = 0;
		virtual bool SetWindowResolution(int w, int h) = 0;
		virtual bool RequestDownloadMap(int xbottom, int ybottom, int xsize, int ysize) = 0;
		virtual bool GetNextAnimTickCount(double &next_anim_tick) = 0;
		virtual bool LoginGameServer(const std::string &gid, const std::string &glt, int serverid, int bigServerIndex, int serverIndex, int character) = 0;

		virtual bool IsUIDialogPresent(int dialog, bool &result) = 0;

		virtual bool RegisterServerShutdownNotify(const std::function<void(int)> &callback) = 0;
		virtual bool RegisterBattleActionNotify(const std::function<void(int)> &callback) = 0;
		virtual bool RegisterPlayerMenuNotify(const std::function<void(cga_player_menu_items_t)> &callback) = 0;
		virtual bool RegisterUnitMenuNotify(const std::function<void(cga_unit_menu_items_t)> &callback) = 0;
		virtual bool RegisterNPCDialogNotify(const std::function<void(cga_npc_dialog_t)> &callback) = 0;
		virtual bool RegisterWorkingResultNotify(const std::function<void(cga_working_result_t)> &callback) = 0;
		virtual bool RegisterChatMsgNotify(const std::function<void(cga_chat_msg_t)> &callback) = 0;
		virtual bool RegisterTradeStuffsNotify(const std::function<void(cga_trade_stuff_info_t)> &callback) = 0;
		virtual bool RegisterTradeDialogNotify(const std::function<void(cga_trade_dialog_t)> &callback) = 0;
		virtual bool RegisterTradeStateNotify(const std::function<void(int)> &callback) = 0;
		virtual bool RegisterDownloadMapNotify(const std::function<void(cga_download_map_t)> &callback) = 0;
		virtual bool RegisterConnectionStateNotify(const std::function<void(cga_conn_state_t)> &callback) = 0;
	};

	extern "C" CGAInterface *CreateInterface();

}
