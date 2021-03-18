#pragma once

#include <msgpack.hpp>

// User defined class template specialization
namespace msgpack {
	MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
		namespace adaptor {
			using namespace CGA;
			//player_info

			template<>
			struct convert<cga_player_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_player_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 26) throw msgpack::type_error();
					v.hp = o.via.array.ptr[0].as<int>();
					v.maxhp = o.via.array.ptr[1].as<int>();
					v.mp = o.via.array.ptr[2].as<int>();
					v.maxmp = o.via.array.ptr[3].as<int>();
					v.xp = o.via.array.ptr[4].as<int>();
					v.maxxp = o.via.array.ptr[5].as<int>();
					v.health = o.via.array.ptr[6].as<int>();
					v.souls = o.via.array.ptr[7].as<int>();
					v.level = o.via.array.ptr[8].as<int>();
					v.gold = o.via.array.ptr[9].as<int>();
					v.unitid = o.via.array.ptr[10].as<int>();
					v.petid = o.via.array.ptr[11].as<int>();
					v.direction = o.via.array.ptr[12].as<int>();
					v.battle_position = o.via.array.ptr[13].as<int>();
					v.punchclock = o.via.array.ptr[14].as<int>();
					v.usingpunchclock = o.via.array.ptr[15].as<bool>();
					v.petriding = o.via.array.ptr[16].as<bool>();
					v.name = o.via.array.ptr[17].as<std::string>();
					v.job = o.via.array.ptr[18].as<std::string>();
					v.titles = o.via.array.ptr[19].as<std::vector<std::string>>();
					v.detail = o.via.array.ptr[20].as<cga_playerpet_detail_info_t>();
					v.persdesc = o.via.array.ptr[21].as<cga_pers_desc_t>();
					v.manu_endurance = o.via.array.ptr[22].as<int>();
					v.manu_skillful = o.via.array.ptr[23].as<int>();
					v.manu_intelligence = o.via.array.ptr[24].as<int>();
					v.value_charisma = o.via.array.ptr[25].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_player_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_player_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(26);
					o.pack(v.hp);
					o.pack(v.maxhp);
					o.pack(v.mp);
					o.pack(v.maxmp);
					o.pack(v.xp);
					o.pack(v.maxxp);
					o.pack(v.health);
					o.pack(v.souls);
					o.pack(v.level);
					o.pack(v.gold);
					o.pack(v.unitid);
					o.pack(v.petid);
					o.pack(v.direction);
					o.pack(v.battle_position);
					o.pack(v.punchclock);
					o.pack(v.usingpunchclock);
					o.pack(v.petriding);
					o.pack(v.name);
					o.pack(v.job);
					o.pack(v.titles);
					o.pack(v.detail);
					o.pack(v.persdesc);
					o.pack(v.manu_endurance);
					o.pack(v.manu_skillful);
					o.pack(v.manu_intelligence);
					o.pack(v.value_charisma);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_player_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_player_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 26;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.hp, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.maxhp, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.mp, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.maxmp, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.xp, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.maxxp, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.health, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.souls, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.level, o.zone);
					o.via.array.ptr[9] = msgpack::object(v.gold, o.zone);
					o.via.array.ptr[10] = msgpack::object(v.unitid, o.zone);
					o.via.array.ptr[11] = msgpack::object(v.petid, o.zone);
					o.via.array.ptr[12] = msgpack::object(v.direction, o.zone);
					o.via.array.ptr[13] = msgpack::object(v.battle_position, o.zone);
					o.via.array.ptr[14] = msgpack::object(v.punchclock, o.zone);
					o.via.array.ptr[15] = msgpack::object(v.usingpunchclock, o.zone);
					o.via.array.ptr[16] = msgpack::object(v.petriding, o.zone);
					o.via.array.ptr[17] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[18] = msgpack::object(v.job, o.zone);
					o.via.array.ptr[19] = msgpack::object(v.titles, o.zone);
					o.via.array.ptr[20] = msgpack::object(v.detail, o.zone);
					o.via.array.ptr[21] = msgpack::object(v.persdesc, o.zone);
					o.via.array.ptr[22] = msgpack::object(v.manu_endurance, o.zone);
					o.via.array.ptr[23] = msgpack::object(v.manu_skillful, o.zone);
					o.via.array.ptr[24] = msgpack::object(v.manu_intelligence, o.zone);
					o.via.array.ptr[25] = msgpack::object(v.value_charisma, o.zone);
				}
			};

			//pet_info

			template<>
			struct convert<cga_pet_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_pet_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 17) throw msgpack::type_error();
					v.hp = o.via.array.ptr[0].as<int>();
					v.maxhp = o.via.array.ptr[1].as<int>();
					v.mp = o.via.array.ptr[2].as<int>();
					v.maxmp = o.via.array.ptr[3].as<int>();
					v.xp = o.via.array.ptr[4].as<int>();
					v.maxxp = o.via.array.ptr[5].as<int>();
					v.health = o.via.array.ptr[6].as<int>();
					v.level = o.via.array.ptr[7].as<int>();
					v.flags = o.via.array.ptr[8].as<int>();
					v.race = o.via.array.ptr[9].as<int>();
					v.loyality = o.via.array.ptr[10].as<int>();
					v.battle_flags = o.via.array.ptr[11].as<int>();
					v.state = o.via.array.ptr[12].as<int>();
					v.index = o.via.array.ptr[13].as<int>();
					v.name = o.via.array.ptr[14].as<std::string>();
					v.realname = o.via.array.ptr[15].as<std::string>();
					v.detail = o.via.array.ptr[16].as<cga_playerpet_detail_info_t>();
					return o;
				}
			};

			template<>
			struct pack<cga_pet_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_pet_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(17);
					o.pack(v.hp);
					o.pack(v.maxhp);
					o.pack(v.mp);
					o.pack(v.maxmp);
					o.pack(v.xp);
					o.pack(v.maxxp);
					o.pack(v.health);
					o.pack(v.level);
					o.pack(v.flags);
					o.pack(v.race);
					o.pack(v.loyality);
					o.pack(v.battle_flags);
					o.pack(v.state);
					o.pack(v.index);
					o.pack(v.name);
					o.pack(v.realname);
					o.pack(v.detail);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_pet_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_pet_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 17;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.hp, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.maxhp, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.mp, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.maxmp, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.xp, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.maxxp, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.health, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.level, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.flags, o.zone);
					o.via.array.ptr[9] = msgpack::object(v.race, o.zone);
					o.via.array.ptr[10] = msgpack::object(v.loyality, o.zone);
					o.via.array.ptr[11] = msgpack::object(v.battle_flags, o.zone);
					o.via.array.ptr[12] = msgpack::object(v.state, o.zone);
					o.via.array.ptr[13] = msgpack::object(v.index, o.zone);
					o.via.array.ptr[14] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[15] = msgpack::object(v.realname, o.zone);
					o.via.array.ptr[16] = msgpack::object(v.detail, o.zone);
				}
			};

			//trade_pet_info

			template<>
			struct convert<cga_trade_pet_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_trade_pet_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 10) throw msgpack::type_error();

					v.level = o.via.array.ptr[0].as<int>();
					v.race = o.via.array.ptr[1].as<int>();
					v.maxhp = o.via.array.ptr[2].as<int>();
					v.maxmp = o.via.array.ptr[3].as<int>();
					v.skill_count = o.via.array.ptr[4].as<int>();
					v.image_id = o.via.array.ptr[5].as<int>();
					v.index = o.via.array.ptr[6].as<int>();
					v.name = o.via.array.ptr[7].as<std::string>();
					v.realname = o.via.array.ptr[8].as<std::string>();
					v.detail = o.via.array.ptr[9].as<cga_playerpet_detail_info_t>();

					return o;
				}
			};

			template<>
			struct pack<cga_trade_pet_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_trade_pet_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(10);
					o.pack(v.level);
					o.pack(v.race);
					o.pack(v.maxhp);
					o.pack(v.maxmp);
					o.pack(v.skill_count);
					o.pack(v.image_id);
					o.pack(v.index);
					o.pack(v.name);
					o.pack(v.realname);
					o.pack(v.detail);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_trade_pet_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_trade_pet_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 10;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.level, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.race, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.maxhp, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.maxmp , o.zone);
					o.via.array.ptr[4] = msgpack::object(v.skill_count, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.image_id, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.index, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.realname, o.zone);
					o.via.array.ptr[9] = msgpack::object(v.detail, o.zone);
				}
			};

			//cga_trade_pet_skill_info_t

			template<>
			struct convert<cga_trade_pet_skill_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_trade_pet_skill_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 2) throw msgpack::type_error();

					v.index = o.via.array.ptr[0].as<int>();
					v.skills = o.via.array.ptr[1].as<std::vector<std::string>>();

					return o;
				}
			};

			template<>
			struct pack<cga_trade_pet_skill_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_trade_pet_skill_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(2);
					o.pack(v.index);
					o.pack(v.skills);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_trade_pet_skill_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_trade_pet_skill_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 2;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.index, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.skills, o.zone);
				}
			};

			//pet_skill_info

			template<>
			struct convert<cga_pet_skill_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_pet_skill_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 5) throw msgpack::type_error();
					v.name = o.via.array.ptr[0].as<std::string>();
					v.info = o.via.array.ptr[1].as<std::string>();
					v.cost = o.via.array.ptr[2].as<int>();
					v.flags = o.via.array.ptr[3].as<int>();
					v.index = o.via.array.ptr[4].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_pet_skill_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_pet_skill_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(5);
					o.pack(v.name);
					o.pack(v.info);
					o.pack(v.cost);
					o.pack(v.flags);
					o.pack(v.index);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_pet_skill_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_pet_skill_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 5;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.info, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.cost, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.flags, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.index, o.zone);
				}
			};

			//skill_info

			template<>
			struct convert<cga_skill_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_skill_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 10) throw msgpack::type_error();
					v.name = o.via.array.ptr[0].as<std::string>();
					v.lv = o.via.array.ptr[1].as<int>();
					v.maxlv = o.via.array.ptr[2].as<int>();
					v.xp = o.via.array.ptr[3].as<int>();
					v.maxxp = o.via.array.ptr[4].as<int>();
					v.skill_id = o.via.array.ptr[5].as<int>();
					v.type = o.via.array.ptr[6].as<int>();
					v.pos = o.via.array.ptr[7].as<int>();
					v.index = o.via.array.ptr[8].as<int>();
					v.slotsize = o.via.array.ptr[9].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_skill_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_skill_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(10);
					o.pack(v.name);
					o.pack(v.lv);
					o.pack(v.maxlv);
					o.pack(v.xp);
					o.pack(v.maxxp);
					o.pack(v.skill_id);
					o.pack(v.type);
					o.pack(v.pos);
					o.pack(v.index);
					o.pack(v.slotsize);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_skill_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_skill_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 10;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.lv, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.maxlv, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.xp, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.maxxp, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.skill_id, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.type, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.pos, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.index, o.zone);
					o.via.array.ptr[9] = msgpack::object(v.slotsize, o.zone);
				}
			};

			//subskill_info

			template<>
			struct convert<cga_subskill_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_subskill_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 6) throw msgpack::type_error();
					v.name = o.via.array.ptr[0].as<std::string>();
					v.info = o.via.array.ptr[1].as<std::string>();
					v.cost = o.via.array.ptr[2].as<int>();
					v.flags = o.via.array.ptr[3].as<int>();
					v.level = o.via.array.ptr[4].as<int>();
					v.available = o.via.array.ptr[5].as<bool>();
					return o;
				}
			};

			template<>
			struct pack<cga_subskill_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_subskill_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(6);
					o.pack(v.name);
					o.pack(v.info);
					o.pack(v.cost);
					o.pack(v.flags);
					o.pack(v.level);
					o.pack(v.available);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_subskill_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_subskill_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 6;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.info, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.cost, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.flags, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.level, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.available, o.zone);
				}
			};

			//item_info

			template<>
			struct convert<cga_item_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_item_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 9) throw msgpack::type_error();
					v.name = o.via.array.ptr[0].as<std::string>();
					v.attr = o.via.array.ptr[1].as<std::string>();
					v.info = o.via.array.ptr[2].as<std::string>();
					v.itemid = o.via.array.ptr[3].as<int>();
					v.count = o.via.array.ptr[4].as<int>();
					v.pos = o.via.array.ptr[5].as<int>();
					v.level = o.via.array.ptr[6].as<int>();
					v.type = o.via.array.ptr[7].as<int>();
					v.assessed = o.via.array.ptr[8].as<bool>();
					return o;
				}
			};

			template<>
			struct pack<cga_item_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_item_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(9);
					o.pack(v.name);
					o.pack(v.attr);
					o.pack(v.info);
					o.pack(v.itemid);
					o.pack(v.count);
					o.pack(v.pos);
					o.pack(v.level);
					o.pack(v.type);
					o.pack(v.assessed);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_item_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_item_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 9;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.attr, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.info, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.itemid, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.count, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.pos, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.level, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.type, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.assessed, o.zone);
				}
			};

			//battle_unit

			template<>
			struct convert<cga_battle_unit_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_battle_unit_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 9) throw msgpack::type_error();
					v.name = o.via.array.ptr[0].as<std::string>();
					v.modelid = o.via.array.ptr[1].as<int>();
					v.level = o.via.array.ptr[2].as<int>();
					v.curhp = o.via.array.ptr[3].as<int>();
					v.maxhp = o.via.array.ptr[4].as<int>();
					v.curmp = o.via.array.ptr[5].as<int>();
					v.maxmp = o.via.array.ptr[6].as<int>();
					v.flags = o.via.array.ptr[7].as<int>();
					v.pos = o.via.array.ptr[8].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_battle_unit_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_battle_unit_t const& v) const {
					// packing member variables as an array.
					o.pack_array(9);
					o.pack(v.name);
					o.pack(v.modelid);
					o.pack(v.level);
					o.pack(v.curhp);
					o.pack(v.maxhp);
					o.pack(v.curmp);
					o.pack(v.maxmp);
					o.pack(v.flags);
					o.pack(v.pos);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_battle_unit_t> {
				void operator()(msgpack::object::with_zone& o, cga_battle_unit_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 9;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.modelid, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.level, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.curhp, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.maxhp, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.curmp, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.maxmp, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.flags, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.pos, o.zone);
				}
			};

			//item_menu_player

			template<>
			struct convert<cga_player_menu_item_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_player_menu_item_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 3) throw msgpack::type_error();
					v.name = o.via.array.ptr[0].as<std::string>();
					v.color = o.via.array.ptr[1].as<int>();
					v.index = o.via.array.ptr[2].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_player_menu_item_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_player_menu_item_t const& v) const {
					// packing member variables as an array.
					o.pack_array(3);
					o.pack(v.name);
					o.pack(v.color);
					o.pack(v.index);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_player_menu_item_t> {
				void operator()(msgpack::object::with_zone& o, cga_player_menu_item_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 3;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.color, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.index, o.zone);
				}
			};

			//item_menu_unit

			template<>
			struct convert<cga_unit_menu_item_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_unit_menu_item_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 9) throw msgpack::type_error();
					v.name = o.via.array.ptr[0].as<std::string>();
					v.level = o.via.array.ptr[1].as<int>();
					v.health = o.via.array.ptr[2].as<int>();
					v.hp = o.via.array.ptr[3].as<int>();
					v.maxhp = o.via.array.ptr[4].as<int>();
					v.mp = o.via.array.ptr[5].as<int>();
					v.maxmp = o.via.array.ptr[6].as<int>();
					v.color = o.via.array.ptr[7].as<int>();
					v.index = o.via.array.ptr[8].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_unit_menu_item_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_unit_menu_item_t const& v) const {
					// packing member variables as an array.
					o.pack_array(9);
					o.pack(v.name);
					o.pack(v.level);
					o.pack(v.health);
					o.pack(v.hp);
					o.pack(v.maxhp);
					o.pack(v.mp);
					o.pack(v.maxmp);
					o.pack(v.color);
					o.pack(v.index);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_unit_menu_item_t> {
				void operator()(msgpack::object::with_zone& o, cga_unit_menu_item_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 9;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.level, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.health, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.hp, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.maxhp, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.mp, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.maxmp, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.color, o.zone);					
					o.via.array.ptr[8] = msgpack::object(v.index, o.zone);
				}
			};

			//cga_npc_dialog_t

			template<>
			struct convert<cga_npc_dialog_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_npc_dialog_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 5) throw msgpack::type_error();
					v.type = o.via.array.ptr[0].as<int>();
					v.options = o.via.array.ptr[1].as<int>();
					v.dialog_id = o.via.array.ptr[2].as<int>();
					v.npc_id = o.via.array.ptr[3].as<int>();
					v.message = o.via.array.ptr[4].as<std::string>();
					return o;
				}
			};

			template<>
			struct pack<cga_npc_dialog_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_npc_dialog_t const& v) const {
					// packing member variables as an array.
					o.pack_array(5);
					o.pack(v.type);
					o.pack(v.options);
					o.pack(v.dialog_id);
					o.pack(v.npc_id);
					o.pack(v.message);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_npc_dialog_t> {
				void operator()(msgpack::object::with_zone& o, cga_npc_dialog_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 5;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.type, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.options, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.dialog_id, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.npc_id, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.message, o.zone);
				}
			};

			//cga_map_unit_t

			template<>
			struct convert<cga_map_unit_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_map_unit_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 15) throw msgpack::type_error();
					v.valid = o.via.array.ptr[0].as<int>();
					v.type = o.via.array.ptr[1].as<int>();
					v.model_id = o.via.array.ptr[2].as<int>();
					v.unit_id = o.via.array.ptr[3].as<int>();
					v.xpos = o.via.array.ptr[4].as<int>();
					v.ypos = o.via.array.ptr[5].as<int>();
					v.item_count = o.via.array.ptr[6].as<int>();
					v.injury = o.via.array.ptr[7].as<int>();
					v.icon = o.via.array.ptr[8].as<int>();
					v.level = o.via.array.ptr[9].as<int>();
					v.flags = o.via.array.ptr[10].as<int>();
					v.unit_name = o.via.array.ptr[11].as<std::string>();
					v.nick_name = o.via.array.ptr[12].as<std::string>();
					v.title_name = o.via.array.ptr[13].as<std::string>();
					v.item_name = o.via.array.ptr[14].as<std::string>();
					return o;
				}
			};

			template<>
			struct pack<cga_map_unit_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_map_unit_t const& v) const {
					// packing member variables as an array.
					o.pack_array(15);
					o.pack(v.valid);
					o.pack(v.type);
					o.pack(v.model_id);
					o.pack(v.unit_id);
					o.pack(v.xpos);
					o.pack(v.ypos);
					o.pack(v.item_count);
					o.pack(v.injury);
					o.pack(v.icon);
					o.pack(v.level);
					o.pack(v.flags);
					o.pack(v.unit_name);
					o.pack(v.nick_name);
					o.pack(v.title_name);
					o.pack(v.item_name);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_map_unit_t> {
				void operator()(msgpack::object::with_zone& o, cga_map_unit_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 15;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.valid, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.type, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.model_id, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.unit_id, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.xpos, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.ypos, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.item_count, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.injury, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.icon, o.zone);
					o.via.array.ptr[9] = msgpack::object(v.level, o.zone);
					o.via.array.ptr[10] = msgpack::object(v.flags, o.zone);
					o.via.array.ptr[11] = msgpack::object(v.unit_name, o.zone);
					o.via.array.ptr[12] = msgpack::object(v.nick_name, o.zone);
					o.via.array.ptr[13] = msgpack::object(v.title_name, o.zone);
					o.via.array.ptr[14] = msgpack::object(v.item_name, o.zone);
				}
			};

			//cga_map_cells_t

			template<>
			struct convert<cga_map_cells_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_map_cells_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 5) throw msgpack::type_error();
					v.x_bottom = o.via.array.ptr[0].as<int>();
					v.y_bottom = o.via.array.ptr[1].as<int>();
					v.x_size = o.via.array.ptr[2].as<int>();
					v.y_size = o.via.array.ptr[3].as<int>();
					v.cell = o.via.array.ptr[4].as<std::vector<short>>();
					return o;
				}
			};

			template<>
			struct pack<cga_map_cells_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_map_cells_t const& v) const {
					// packing member variables as an array.
					o.pack_array(5);
					o.pack(v.x_bottom);
					o.pack(v.y_bottom);
					o.pack(v.x_size);
					o.pack(v.y_size);
					o.pack(v.cell);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_map_cells_t> {
				void operator()(msgpack::object::with_zone& o, cga_map_cells_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 5;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.x_bottom, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.y_bottom, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.x_size, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.y_size, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.cell, o.zone);
				}
			};

			//cga_sell_item_t

			template<>
			struct convert<cga_sell_item_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_sell_item_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 3) throw msgpack::type_error();
					v.itemid = o.via.array.ptr[0].as<int>();
					v.itempos = o.via.array.ptr[1].as<int>();
					v.count = o.via.array.ptr[2].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_sell_item_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_sell_item_t const& v) const {
					// packing member variables as an array.
					o.pack_array(3);
					o.pack(v.itemid);
					o.pack(v.itempos);
					o.pack(v.count);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_sell_item_t> {
				void operator()(msgpack::object::with_zone& o, cga_sell_item_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 3;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.itemid, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.itempos, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.count, o.zone);
				}
			};

			//cga_buy_item_t

			template<>
			struct convert<cga_buy_item_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_buy_item_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 2) throw msgpack::type_error();
					v.index = o.via.array.ptr[0].as<int>();
					v.count = o.via.array.ptr[1].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_buy_item_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_buy_item_t const& v) const {
					// packing member variables as an array.
					o.pack_array(2);
					o.pack(v.index);
					o.pack(v.count);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_buy_item_t> {
				void operator()(msgpack::object::with_zone& o, cga_buy_item_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 2;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.index, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.count, o.zone);
				}
			};

			//cga_craft_item_t

			template<>
			struct convert<cga_craft_item_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_craft_item_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 9) throw msgpack::type_error();
					v.skill_index = o.via.array.ptr[0].as<int>();
					v.subskill_index = o.via.array.ptr[1].as<int>();
					v.sub_type = o.via.array.ptr[2].as<int>();
					for(int i = 0;i < 6; ++i)
						v.itempos[i] = o.via.array.ptr[3 + i].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_craft_item_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_craft_item_t const& v) const {
					// packing member variables as an array.
					o.pack_array(9);
					o.pack(v.skill_index);
					o.pack(v.subskill_index);
					o.pack(v.sub_type);
					for (int i = 0; i < 6; ++i)
						o.pack(v.itempos[i]);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_craft_item_t> {
				void operator()(msgpack::object::with_zone& o, cga_craft_item_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 9;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.skill_index, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.subskill_index, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.sub_type, o.zone);
					for (int i = 0; i < 6; ++i)
						o.via.array.ptr[3 + i] = msgpack::object(v.itempos[i], o.zone);
				}
			};

			//cga_craft_material_t

			template<>
			struct convert<cga_craft_material_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_craft_material_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 3) throw msgpack::type_error();
					v.itemid = o.via.array.ptr[0].as<int>();
					v.count = o.via.array.ptr[1].as<int>();
					v.name = o.via.array.ptr[2].as<std::string>();
					return o;
				}
			};

			template<>
			struct pack<cga_craft_material_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_craft_material_t const& v) const {
					// packing member variables as an array.
					o.pack_array(3);
					o.pack(v.itemid);
					o.pack(v.count);
					o.pack(v.name);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_craft_material_t> {
				void operator()(msgpack::object::with_zone& o, cga_craft_material_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 3;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.itemid, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.count, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.name, o.zone);
				}
			};

			//cga_craft_info_t

			template<>
			struct convert<cga_craft_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_craft_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 13) throw msgpack::type_error();
					v.id = o.via.array.ptr[0].as<int>();
					v.cost = o.via.array.ptr[1].as<int>();
					v.level = o.via.array.ptr[2].as<int>();
					v.itemid = o.via.array.ptr[3].as<int>();
					v.index = o.via.array.ptr[4].as<int>();
					v.name = o.via.array.ptr[5].as<std::string>();
					v.info = o.via.array.ptr[6].as<std::string>();
					v.available = o.via.array.ptr[7].as<bool>();
					for (int i = 0; i < 5; ++i)
						v.materials[i] = o.via.array.ptr[8 + i].as<cga_craft_material_t>();
					return o;
				}
			};

			template<>
			struct pack<cga_craft_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_craft_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(13);
					o.pack(v.id);
					o.pack(v.cost);
					o.pack(v.level);
					o.pack(v.itemid);
					o.pack(v.index);
					o.pack(v.name);
					o.pack(v.info);
					o.pack(v.available);
					for (int i = 0; i < 5; ++i)
						o.pack(v.materials[i]);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_craft_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_craft_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 13;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.id, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.cost, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.level, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.itemid, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.index, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.info, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.available, o.zone);
					for (int i = 0; i < 5; ++i)
						o.via.array.ptr[8 + i] = msgpack::object(v.materials[i], o.zone);
				}
			};

			//cga_working_result_t
			template<>
			struct convert<cga_working_result_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_working_result_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 10) throw msgpack::type_error();
					v.type = o.via.array.ptr[0].as<int>();
					v.success = o.via.array.ptr[1].as<bool>();
					v.levelup = o.via.array.ptr[2].as<bool>();
					v.xp = o.via.array.ptr[3].as<int>();
					v.endurance = o.via.array.ptr[4].as<int>();
					v.skillful = o.via.array.ptr[5].as<int>();
					v.intelligence = o.via.array.ptr[6].as<int>();
					v.value = o.via.array.ptr[7].as<int>();
					v.value2 = o.via.array.ptr[8].as<int>();
					v.name = o.via.array.ptr[9].as<std::string>();
					return o;
				}
			};

			template<>
			struct pack<cga_working_result_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_working_result_t const& v) const {
					// packing member variables as an array.
					o.pack_array(10);
					o.pack(v.type);
					o.pack(v.success);
					o.pack(v.levelup);
					o.pack(v.xp);				
					o.pack(v.endurance);
					o.pack(v.skillful);
					o.pack(v.intelligence);
					o.pack(v.value);
					o.pack(v.value2);
					o.pack(v.name);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_working_result_t> {
				void operator()(msgpack::object::with_zone& o, cga_working_result_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 10;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.type, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.success, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.levelup, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.xp, o.zone);				
					o.via.array.ptr[4] = msgpack::object(v.endurance, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.skillful, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.intelligence, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.value, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.value2, o.zone);
					o.via.array.ptr[9] = msgpack::object(v.name, o.zone);
				}
			};

			//cga_game_data_t
			template<>
			struct convert<cga_game_data_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_game_data_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 1) throw msgpack::type_error();
					v.reserved = o.via.array.ptr[0].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_game_data_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_game_data_t const& v) const {
					// packing member variables as an array.
					o.pack_array(1);
					o.pack(v.reserved);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_game_data_t> {
				void operator()(msgpack::object::with_zone& o, cga_game_data_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 110;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.reserved, o.zone);
				}
			};

			//cga_chat_msg_t
			template<>
			struct convert<cga_chat_msg_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_chat_msg_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 4) throw msgpack::type_error();
					v.unitid = o.via.array.ptr[0].as<int>();
					v.msg = o.via.array.ptr[1].as<std::string>();
					v.color = o.via.array.ptr[2].as<int>();
					v.size = o.via.array.ptr[3].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_chat_msg_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_chat_msg_t const& v) const {
					// packing member variables as an array.
					o.pack_array(4);
					o.pack(v.unitid);
					o.pack(v.msg);
					o.pack(v.color);
					o.pack(v.size);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_chat_msg_t> {
				void operator()(msgpack::object::with_zone& o, cga_chat_msg_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 4;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.unitid, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.msg, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.color, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.size, o.zone);
				}
			};

			//cga_trade_stuff_info_t
			template<>
			struct convert<cga_trade_stuff_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_trade_stuff_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 5) throw msgpack::type_error();
					v.items = o.via.array.ptr[0].as<cga_items_info_t>();
					v.pet = o.via.array.ptr[1].as<cga_trade_pet_info_t>();
					v.petskills = o.via.array.ptr[2].as<cga_trade_pet_skill_info_t>();
					v.gold = o.via.array.ptr[3].as<int>();
					v.type = o.via.array.ptr[4].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_trade_stuff_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_trade_stuff_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(5);
					o.pack(v.items);
					o.pack(v.pet);
					o.pack(v.petskills);
					o.pack(v.gold);
					o.pack(v.type);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_trade_stuff_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_trade_stuff_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 5;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.items, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.pet, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.petskills, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.gold, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.type, o.zone);
				}
			};

			//cga_team_player_t
			template<>
			struct convert<cga_team_player_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_team_player_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 8) throw msgpack::type_error();
					v.unit_id = o.via.array.ptr[0].as<int>();
					v.hp = o.via.array.ptr[1].as<int>();
					v.maxhp = o.via.array.ptr[2].as<int>();
					v.mp = o.via.array.ptr[3].as<int>();
					v.maxmp = o.via.array.ptr[4].as<int>();
					v.xpos = o.via.array.ptr[5].as<int>();
					v.ypos = o.via.array.ptr[6].as<int>();
					v.name = o.via.array.ptr[7].as<std::string>();
					return o;
				}
			};

			template<>
			struct pack<cga_team_player_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_team_player_t const& v) const {
					// packing member variables as an array.
					o.pack_array(8);
					o.pack(v.unit_id);
					o.pack(v.hp);
					o.pack(v.maxhp);
					o.pack(v.mp);
					o.pack(v.maxmp);
					o.pack(v.xpos);
					o.pack(v.ypos);
					o.pack(v.name);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_team_player_t> {
				void operator()(msgpack::object::with_zone& o, cga_team_player_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 8;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.unit_id, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.hp, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.maxhp, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.mp, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.maxmp, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.xpos, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.ypos, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.name, o.zone);
				}
			};

			//cga_trade_dialog_t
			template<>
			struct convert<cga_trade_dialog_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_trade_dialog_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 2) throw msgpack::type_error();
					v.name = o.via.array.ptr[0].as<std::string>();
					v.level = o.via.array.ptr[1].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_trade_dialog_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_trade_dialog_t const& v) const {
					// packing member variables as an array.
					o.pack_array(2);
					o.pack(v.name);
					o.pack(v.level);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_trade_dialog_t> {
				void operator()(msgpack::object::with_zone& o, cga_trade_dialog_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 2;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.name, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.level, o.zone);
				}
			};

			//cga_battle_context_t
			template<>
			struct convert<cga_battle_context_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_battle_context_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 9) throw msgpack::type_error();
					v.round_count = o.via.array.ptr[0].as<int>();
					v.player_pos = o.via.array.ptr[1].as<int>();
					v.player_status = o.via.array.ptr[2].as<int>();
					v.skill_performed = o.via.array.ptr[3].as<int>();
					v.skill_allowbit = o.via.array.ptr[4].as<int>();
					v.petskill_allowbit = o.via.array.ptr[5].as<int>();
					v.weapon_allowbit = o.via.array.ptr[6].as<int>();
					v.petid = o.via.array.ptr[7].as<int>();
					v.effect_flags = o.via.array.ptr[8].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_battle_context_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_battle_context_t const& v) const {
					// packing member variables as an array.
					o.pack_array(9);
					o.pack(v.round_count);
					o.pack(v.player_pos);
					o.pack(v.player_status);
					o.pack(v.skill_performed);
					o.pack(v.skill_allowbit);
					o.pack(v.petskill_allowbit);
					o.pack(v.weapon_allowbit);
					o.pack(v.petid);
					o.pack(v.effect_flags);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_battle_context_t> {
				void operator()(msgpack::object::with_zone& o, cga_battle_context_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 8;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.round_count, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.player_pos, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.player_status, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.skill_performed, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.skill_allowbit, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.petskill_allowbit, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.weapon_allowbit, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.petid, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.effect_flags, o.zone);
				}
			};

			//cga_playerpet_detail_info_t
			template<>
			struct convert<cga_playerpet_detail_info_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_playerpet_detail_info_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 25) throw msgpack::type_error();
					v.points_remain = o.via.array.ptr[0].as<int>();
					v.points_endurance = o.via.array.ptr[1].as<int>();
					v.points_strength = o.via.array.ptr[2].as<int>();
					v.points_defense = o.via.array.ptr[3].as<int>();
					v.points_agility = o.via.array.ptr[4].as<int>();
					v.points_magical = o.via.array.ptr[5].as<int>();
					v.value_attack = o.via.array.ptr[6].as<int>();
					v.value_defensive = o.via.array.ptr[7].as<int>();
					v.value_agility = o.via.array.ptr[8].as<int>();
					v.value_spirit = o.via.array.ptr[9].as<int>();
					v.value_recovery = o.via.array.ptr[10].as<int>();
					v.resist_poison = o.via.array.ptr[11].as<int>();
					v.resist_sleep = o.via.array.ptr[12].as<int>();
					v.resist_medusa = o.via.array.ptr[13].as<int>();
					v.resist_drunk = o.via.array.ptr[14].as<int>();
					v.resist_chaos = o.via.array.ptr[15].as<int>();
					v.resist_forget = o.via.array.ptr[16].as<int>();
					v.fix_critical = o.via.array.ptr[17].as<int>();
					v.fix_strikeback = o.via.array.ptr[18].as<int>();
					v.fix_accurancy = o.via.array.ptr[19].as<int>();
					v.fix_dodge = o.via.array.ptr[20].as<int>();
					v.element_earth = o.via.array.ptr[21].as<int>();
					v.element_water = o.via.array.ptr[22].as<int>();
					v.element_fire = o.via.array.ptr[23].as<int>();
					v.element_wind = o.via.array.ptr[24].as<int>();
					return o;
				}
			};

			template<>
			struct pack<cga_playerpet_detail_info_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_playerpet_detail_info_t const& v) const {
					// packing member variables as an array.
					o.pack_array(25);
					o.pack(v.points_remain);
					o.pack(v.points_endurance);
					o.pack(v.points_strength);
					o.pack(v.points_defense);
					o.pack(v.points_agility);
					o.pack(v.points_magical);
					o.pack(v.value_attack);
					o.pack(v.value_defensive);
					o.pack(v.value_agility);
					o.pack(v.value_spirit);
					o.pack(v.value_recovery);
					o.pack(v.resist_poison);
					o.pack(v.resist_sleep);
					o.pack(v.resist_medusa);
					o.pack(v.resist_drunk);
					o.pack(v.resist_chaos);
					o.pack(v.resist_forget);
					o.pack(v.fix_critical);
					o.pack(v.fix_strikeback);
					o.pack(v.fix_accurancy);
					o.pack(v.fix_dodge);
					o.pack(v.element_earth);
					o.pack(v.element_water);
					o.pack(v.element_fire);
					o.pack(v.element_wind);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_playerpet_detail_info_t> {
				void operator()(msgpack::object::with_zone& o, cga_playerpet_detail_info_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 25;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.points_remain, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.points_endurance, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.points_strength, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.points_defense, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.points_agility, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.points_magical, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.value_attack, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.value_defensive, o.zone);
					o.via.array.ptr[8] = msgpack::object(v.value_agility, o.zone);
					o.via.array.ptr[9] = msgpack::object(v.value_spirit, o.zone);
					o.via.array.ptr[10] = msgpack::object(v.value_recovery, o.zone);
					o.via.array.ptr[11] = msgpack::object(v.resist_poison, o.zone);
					o.via.array.ptr[12] = msgpack::object(v.resist_sleep, o.zone);
					o.via.array.ptr[13] = msgpack::object(v.resist_medusa, o.zone);
					o.via.array.ptr[14] = msgpack::object(v.resist_drunk, o.zone);
					o.via.array.ptr[15] = msgpack::object(v.resist_chaos, o.zone);
					o.via.array.ptr[16] = msgpack::object(v.resist_forget, o.zone);
					o.via.array.ptr[17] = msgpack::object(v.fix_critical, o.zone);
					o.via.array.ptr[18] = msgpack::object(v.fix_strikeback, o.zone);
					o.via.array.ptr[19] = msgpack::object(v.fix_accurancy, o.zone);
					o.via.array.ptr[20] = msgpack::object(v.fix_dodge, o.zone);
					o.via.array.ptr[21] = msgpack::object(v.element_earth, o.zone);
					o.via.array.ptr[22] = msgpack::object(v.element_water, o.zone);
					o.via.array.ptr[23] = msgpack::object(v.element_fire, o.zone);
					o.via.array.ptr[24] = msgpack::object(v.element_wind, o.zone);
				}
			};

			//cga_download_map_t

			template<>
			struct convert<cga_download_map_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_download_map_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 6) throw msgpack::type_error();

					v.index1 = o.via.array.ptr[0].as<int>();
					v.index3 = o.via.array.ptr[1].as<int>();
					v.xbase = o.via.array.ptr[2].as<int>();
					v.ybase = o.via.array.ptr[3].as<int>();
					v.xtop = o.via.array.ptr[4].as<int>();
					v.ytop = o.via.array.ptr[5].as<int>();

					return o;
				}
			};

			template<>
			struct pack<cga_download_map_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_download_map_t const& v) const {
					// packing member variables as an array.
					o.pack_array(6);
					o.pack(v.index1);
					o.pack(v.index3);
					o.pack(v.xbase);
					o.pack(v.ybase);
					o.pack(v.xtop);
					o.pack(v.ytop);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_download_map_t> {
				void operator()(msgpack::object::with_zone& o, cga_download_map_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 6;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.index1, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.index3, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.xbase, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.ybase, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.xtop, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.ytop, o.zone);				}
			};

			//cga_sys_time_t

			template<>
			struct convert<cga_sys_time_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_sys_time_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 8) throw msgpack::type_error();

					v.years = o.via.array.ptr[0].as<int>();
					v.month = o.via.array.ptr[1].as<int>();
					v.days = o.via.array.ptr[2].as<int>();
					v.hours = o.via.array.ptr[3].as<int>();
					v.mins = o.via.array.ptr[4].as<int>();
					v.secs = o.via.array.ptr[5].as<int>();
					v.local_time = o.via.array.ptr[6].as<int>();
					v.server_time = o.via.array.ptr[7].as<int>();

					return o;
				}
			};

			template<>
			struct pack<cga_sys_time_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_sys_time_t const& v) const {
					// packing member variables as an array.
					o.pack_array(8);
					o.pack(v.years);
					o.pack(v.month);
					o.pack(v.days);
					o.pack(v.hours);
					o.pack(v.mins);
					o.pack(v.secs);
					o.pack(v.local_time);
					o.pack(v.server_time);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_sys_time_t> {
				void operator()(msgpack::object::with_zone& o, cga_sys_time_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 8;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.years, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.month, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.days, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.hours, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.mins, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.secs, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.local_time, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.server_time, o.zone);
				}
			};

			//cga_conn_state_t

			template<>
			struct convert<cga_conn_state_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_conn_state_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 2) throw msgpack::type_error();

					v.state = o.via.array.ptr[0].as<int>();
					v.msg = o.via.array.ptr[1].as<std::string>();

					return o;
				}
			};

			template<>
			struct pack<cga_conn_state_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_conn_state_t const& v) const {
					// packing member variables as an array.
					o.pack_array(2);
					o.pack(v.state);
					o.pack(v.msg);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_conn_state_t> {
				void operator()(msgpack::object::with_zone& o, cga_conn_state_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 2;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.state, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.msg, o.zone);
				}
			};

			//cga_pers_desc_t

			template<>
			struct convert<cga_pers_desc_t> {
				msgpack::object const& operator()(msgpack::object const& o, cga_pers_desc_t& v) const {
					if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
					if (o.via.array.size != 8) throw msgpack::type_error();

					v.changeBits = o.via.array.ptr[0].as<int>();
					v.sellIcon = o.via.array.ptr[1].as<int>();
					v.sellString = o.via.array.ptr[2].as<std::string>();
					v.buyIcon = o.via.array.ptr[3].as<int>();
					v.buyString = o.via.array.ptr[4].as<std::string>();
					v.wantIcon = o.via.array.ptr[5].as<int>();
					v.wantString = o.via.array.ptr[6].as<std::string>();
					v.descString = o.via.array.ptr[7].as<std::string>();

					return o;
				}
			};

			template<>
			struct pack<cga_pers_desc_t> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, cga_pers_desc_t const& v) const {
					// packing member variables as an array.
					o.pack_array(8);
					o.pack(v.changeBits);
					o.pack(v.sellIcon);
					o.pack(v.sellString);
					o.pack(v.buyIcon);
					o.pack(v.buyString);
					o.pack(v.wantIcon);
					o.pack(v.wantString);
					o.pack(v.descString);
					return o;
				}
			};

			template <>
			struct object_with_zone<cga_pers_desc_t> {
				void operator()(msgpack::object::with_zone& o, cga_pers_desc_t const& v) const {
					o.type = type::ARRAY;
					o.via.array.size = 8;
					o.via.array.ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
					o.via.array.ptr[0] = msgpack::object(v.changeBits, o.zone);
					o.via.array.ptr[1] = msgpack::object(v.sellIcon, o.zone);
					o.via.array.ptr[2] = msgpack::object(v.sellString, o.zone);
					o.via.array.ptr[3] = msgpack::object(v.buyIcon, o.zone);
					o.via.array.ptr[4] = msgpack::object(v.buyString, o.zone);
					o.via.array.ptr[5] = msgpack::object(v.wantIcon, o.zone);
					o.via.array.ptr[6] = msgpack::object(v.wantString, o.zone);
					o.via.array.ptr[7] = msgpack::object(v.descString, o.zone);
				}
			};

		} // namespace adaptor
	} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack