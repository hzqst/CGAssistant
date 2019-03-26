#define _WIN32_WINNT 0x0501
#include <rest_rpc/client.hpp>
#include "gameinterface.h"
#include "packdata.h"

namespace CGAServiceProtocol
{
	using namespace CGA;
	TIMAX_DEFINE_PROTOCOL(Initialize, void(cga_game_data_t));
	TIMAX_DEFINE_PROTOCOL(Connect, bool());
	TIMAX_DEFINE_PROTOCOL(IsInGame, int());
	TIMAX_DEFINE_PROTOCOL(GetWorldStatus, int());
	TIMAX_DEFINE_PROTOCOL(GetGameStatus, int());
	TIMAX_DEFINE_PROTOCOL(GetPlayerInfo, cga_player_info_t());
	TIMAX_DEFINE_PROTOCOL(SetPlayerFlagEnabled, void(int, bool));
	TIMAX_DEFINE_PROTOCOL(IsPlayerFlagEnabled, bool(int));
	TIMAX_DEFINE_PROTOCOL(IsPetValid, bool(int));
	TIMAX_DEFINE_PROTOCOL(GetPetInfo, cga_pet_info_t(int));
	TIMAX_DEFINE_PROTOCOL(GetPetsInfo, cga_pets_info_t());
	TIMAX_DEFINE_PROTOCOL(IsPetSkillValid, bool(int,int));
	TIMAX_DEFINE_PROTOCOL(GetPetSkillInfo, cga_pet_skill_info_t(int, int));
	TIMAX_DEFINE_PROTOCOL(GetPetSkillsInfo, cga_pet_skills_info_t(int));
	TIMAX_DEFINE_PROTOCOL(IsSkillValid, bool(int));
	TIMAX_DEFINE_PROTOCOL(GetSkillInfo, cga_skill_info_t(int));
	TIMAX_DEFINE_PROTOCOL(GetSkillsInfo, cga_skills_info_t());
	TIMAX_DEFINE_PROTOCOL(GetSubSkillInfo, cga_subskill_info_t(int, int));
	TIMAX_DEFINE_PROTOCOL(GetSubSkillsInfo, cga_subskills_info_t(int));
	TIMAX_DEFINE_PROTOCOL(IsItemValid, bool(int));
	TIMAX_DEFINE_PROTOCOL(GetItemInfo, cga_item_info_t(int));
	TIMAX_DEFINE_PROTOCOL(GetItemsInfo, cga_items_info_t());
	TIMAX_DEFINE_PROTOCOL(GetBankItemsInfo, cga_items_info_t());
	TIMAX_DEFINE_PROTOCOL(DropItem, bool(int));
	TIMAX_DEFINE_PROTOCOL(UseItem, bool(int));
	TIMAX_DEFINE_PROTOCOL(MoveItem, bool(int, int, int));
	TIMAX_DEFINE_PROTOCOL(GetMapXY, std::tuple<int, int>());
	TIMAX_DEFINE_PROTOCOL(GetMapXYFloat, std::tuple<float, float>());
	TIMAX_DEFINE_PROTOCOL(GetMoveSpeed, std::tuple<float, float>());
	TIMAX_DEFINE_PROTOCOL(GetMapName, std::string());
	TIMAX_DEFINE_PROTOCOL(GetMapUnits, cga_map_units_t());
	TIMAX_DEFINE_PROTOCOL(WalkTo, void(int,int));
	TIMAX_DEFINE_PROTOCOL(TurnTo, void(int, int));
	TIMAX_DEFINE_PROTOCOL(ForceMove, bool(int, bool));
	TIMAX_DEFINE_PROTOCOL(ForceMoveTo, bool(int, int, bool));
	TIMAX_DEFINE_PROTOCOL(IsMapCellPassable, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(SetMoveSpeed, void(int));
	TIMAX_DEFINE_PROTOCOL(LogBack, void());
	TIMAX_DEFINE_PROTOCOL(LogOut, void());
	TIMAX_DEFINE_PROTOCOL(SayWords, void(std::string, int, int, int));
	TIMAX_DEFINE_PROTOCOL(ClickNPCDialog, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(SellNPCStore, bool(cga_sell_items_t));
	TIMAX_DEFINE_PROTOCOL(BuyNPCStore, bool(cga_buy_items_t));
	TIMAX_DEFINE_PROTOCOL(PlayerMenuSelect, bool(int));
	TIMAX_DEFINE_PROTOCOL(UnitMenuSelect, bool(int));
	TIMAX_DEFINE_PROTOCOL(IsBattleUnitValid, bool(int));
	TIMAX_DEFINE_PROTOCOL(GetBattleUnit, cga_battle_unit_t(int));
	TIMAX_DEFINE_PROTOCOL(GetBattleUnits, cga_battle_units_t());
	TIMAX_DEFINE_PROTOCOL(GetBattleRoundCount, int());
	TIMAX_DEFINE_PROTOCOL(GetBattlePlayerPosition, int());
	TIMAX_DEFINE_PROTOCOL(GetBattlePlayerStatus, int());
	TIMAX_DEFINE_PROTOCOL(GetBattlePetId, int());
	TIMAX_DEFINE_PROTOCOL(BattleNormalAttack, bool(int));
	TIMAX_DEFINE_PROTOCOL(BattleSkillAttack, bool(int, int, int));
	TIMAX_DEFINE_PROTOCOL(BattleDefense, bool());
	TIMAX_DEFINE_PROTOCOL(BattleEscape, bool());
	TIMAX_DEFINE_PROTOCOL(BattleExchangePosition, bool());
	TIMAX_DEFINE_PROTOCOL(BattleChangePet, bool(int));
	TIMAX_DEFINE_PROTOCOL(BattleUseItem, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(BattlePetSkillAttack, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(BattleSetHighSpeedEnabled, void(bool));
	TIMAX_DEFINE_PROTOCOL(BattleSetShowHPMPEnabled, void(bool));
	TIMAX_DEFINE_PROTOCOL(GetBattleEndTick, int());
	TIMAX_DEFINE_PROTOCOL(SetBattleEndTick, void(int));
	TIMAX_DEFINE_PROTOCOL(SetWorkDelay, void(int));
	TIMAX_DEFINE_PROTOCOL(SetWorkAcceleration, void(int));
	TIMAX_DEFINE_PROTOCOL(StartWork, bool(int,int));
	TIMAX_DEFINE_PROTOCOL(CraftItem, bool(cga_craft_item_t));
	TIMAX_DEFINE_PROTOCOL(AssessItem, bool(int,int));
	TIMAX_DEFINE_PROTOCOL(GetCraftInfo, cga_craft_info_t(int,int));
	TIMAX_DEFINE_PROTOCOL(GetCraftsInfo, cga_crafts_info_t(int));
	TIMAX_DEFINE_FORWARD(NotifyServerShutdown, int);
	TIMAX_DEFINE_FORWARD(NotifyBattleAction, int);
	TIMAX_DEFINE_FORWARD(NotifyPlayerMenu, cga_player_menu_items_t);
	TIMAX_DEFINE_FORWARD(NotifyUnitMenu, cga_unit_menu_items_t);
	TIMAX_DEFINE_FORWARD(NotifyNPCDialog, cga_npc_dialog_t);
	TIMAX_DEFINE_FORWARD(NotifyWorkingResult, cga_working_result_t);
	TIMAX_DEFINE_FORWARD(NotifyChatMsg, cga_chat_msg_t);
}

namespace CGA
{
	class CGAService : public CGAInterface
	{
	private:
		using end_point = boost::asio::ip::basic_endpoint<boost::asio::ip::tcp>;
		using sync_client_t = timax::rpc::sync_client<timax::rpc::msgpack_codec>;
		using async_client_t = timax::rpc::async_client<timax::rpc::msgpack_codec>;
		end_point m_endpoint;
		sync_client_t m_client;
		async_client_t m_async_client;
		bool m_connected;
	public:
		CGAService()
		{
			m_connected = false;
		}
		virtual bool IsConnected() {
			return m_connected;
		}
		virtual void Disconnect() {
			m_connected = false;;
		}
		virtual bool Connect(int port) {
			m_endpoint = timax::rpc::get_tcp_endpoint("127.0.0.1", port);

			if (!m_connected)
			{
				try
				{
					m_connected = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::Connect);
				}
				catch (timax::rpc::exception const &e) { m_connected = false; }
				catch (msgpack::parse_error &e) {}
			}
			return m_connected;
		}
		virtual bool Initialize(cga_game_data_t &data) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::Initialize, data);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool IsInGame(int &ingame) {
			if (m_connected) {
				try {
					ingame = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::IsInGame);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetWorldStatus(int &status) {
			if (m_connected) {
				try {
					status = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetWorldStatus);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetGameStatus(int &status) {
			if (m_connected) {
				try {
					status = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetGameStatus);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetPlayerInfo(cga_player_info_t &info){
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetPlayerInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool SetPlayerFlagEnabled(int index, bool enable) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::SetPlayerFlagEnabled, index, enable);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool IsPlayerFlagEnabled(int index, bool &enable) {
			if (m_connected) {
				try {
					enable = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::IsPlayerFlagEnabled, index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool IsPetValid(int petid, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::IsPetValid, petid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetPetInfo(int petid, cga_pet_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetPetInfo, petid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetPetsInfo(cga_pets_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetPetsInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool IsPetSkillValid(int petid, int skillid, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::IsPetSkillValid, petid, skillid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetPetSkillInfo(int petid, int skillpos, cga_pet_skill_info_t &skill) {
			if (m_connected) {
				try {
					skill = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetPetSkillInfo, petid, skillpos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetPetSkillsInfo(int petid, cga_pet_skills_info_t &skills) {
			if (m_connected) {
				try {
					skills = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetPetSkillsInfo, petid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool IsSkillValid(int skillid, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::IsSkillValid, skillid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetSkillInfo(int skillid, cga_skill_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetSkillInfo, skillid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetSkillsInfo(cga_skills_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetSkillsInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetSubSkillInfo(int skillid, int stage, cga_subskill_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetSubSkillInfo, skillid, stage);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return 0;
		}
		virtual bool GetSubSkillsInfo(int skillid, cga_subskills_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetSubSkillsInfo, skillid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return 0;
		}
		virtual bool IsItemValid(int itempos, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::IsItemValid, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetItemInfo(int itempos, cga_item_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetItemInfo, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetItemsInfo(cga_items_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetItemsInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetBankItemsInfo(cga_items_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetBankItemsInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool DropItem(int itempos, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::DropItem, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool UseItem(int itempos, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::UseItem, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool MoveItem(int itempos, int dstpos, int count, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::MoveItem, itempos, dstpos, count);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetMapXY(int &x, int &y) {
			if (m_connected) {
				try {
					std::tuple<int, int> tup = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetMapXY);
					x = std::get<0>(tup);
					y = std::get<1>(tup);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetMapXYFloat(float &x, float &y) {
			if (m_connected) {
				try {
					std::tuple<float, float> tup = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetMapXYFloat);
					x = std::get<0>(tup);
					y = std::get<1>(tup);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetMoveSpeed(float &x, float &y) {
			if (m_connected) {
				try {
					std::tuple<float, float> tup = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetMoveSpeed);
					x = std::get<0>(tup);
					y = std::get<1>(tup);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetMapName(std::string &name) {
			if (m_connected) {
				try {
					name = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetMapName);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetMapUnits(cga_map_units_t &units) {
			if (m_connected) {
				try {
					units = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetMapUnits);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool WalkTo(int x, int y) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::WalkTo, x, y);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool TurnTo(int x, int y) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::TurnTo, x, y);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool ForceMove(int dir, bool show, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::ForceMove, dir, show);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool ForceMoveTo(int x, int y, bool show, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::ForceMoveTo, x, y, show);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool IsMapCellPassable(int x, int y, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::IsMapCellPassable, x, y);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool SetMoveSpeed(int speed) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::SetMoveSpeed, speed);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool LogBack() {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::LogBack);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool LogOut() {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::LogOut);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool SayWords(std::string &str, int color , int range, int size) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::SayWords, str, color, range, size);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool ClickNPCDialog(int option, int index, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::ClickNPCDialog, option, index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool SellNPCStore(cga_sell_items_t &items, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::SellNPCStore, items);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BuyNPCStore(cga_buy_items_t &items, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BuyNPCStore, items);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool PlayerMenuSelect(int menuindex, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::PlayerMenuSelect, menuindex);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool UnitMenuSelect(int menuindex, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::UnitMenuSelect, menuindex);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool IsBattleUnitValid(int pos, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::IsBattleUnitValid, pos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetBattleUnit(int pos, cga_battle_unit_t &unit) {
			if (m_connected) {
				try {
					unit = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetBattleUnit, pos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetBattleUnits(cga_battle_units_t &units) {
			if (m_connected) {
				try {
					units = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetBattleUnits);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetBattleRoundCount(int &count) {
			if (m_connected) {
				try {
					count = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetBattleRoundCount);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetBattlePlayerPosition(int &pos) {
			if (m_connected) {
				try {
					pos = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetBattlePlayerPosition);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetBattlePlayerStatus(int &status) {
			if (m_connected) {
				try {
					status = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetBattlePlayerStatus);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetBattlePetId(int &petid) {
			if (m_connected) {
				try {
					petid = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetBattlePetId);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattleNormalAttack(int target, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattleNormalAttack, target);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattleSkillAttack(int skillpos, int skilllv, int target, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattleSkillAttack, skillpos, skilllv, target);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattleDefense(bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattleDefense);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattleEscape(bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattleEscape);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattleExchangePosition(bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattleExchangePosition);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattleChangePet(int petid, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattleChangePet, petid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattleUseItem(int itempos, int target, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattleUseItem, itempos, target);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattlePetSkillAttack(int skillpos, int target, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattlePetSkillAttack, skillpos, target);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattleSetHighSpeedEnabled(bool enable) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattleSetHighSpeedEnabled, enable);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool BattleSetShowHPMPEnabled(bool enable) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::BattleSetShowHPMPEnabled, enable);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetBattleEndTick(int &msec) {
			if (m_connected) {
				try {
					msec = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetBattleEndTick);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool SetBattleEndTick(int msec) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::SetBattleEndTick, msec);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool SetWorkDelay(int delay)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::SetWorkDelay, delay);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool SetWorkAcceleration(int percent)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::SetWorkAcceleration, percent);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool StartWork(int skill_index, int sub_index, bool &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::StartWork, skill_index, sub_index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool CraftItem(cga_craft_item_t &craft, bool &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::CraftItem, craft);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool AssessItem(int skill_index, int itempos, bool &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::AssessItem, skill_index, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetCraftInfo(int skill_index, int sub_index, cga_craft_info_t &info)
		{
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetCraftInfo, skill_index, sub_index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool GetCraftsInfo(int skill_index, cga_crafts_info_t &info)
		{
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(1000), m_endpoint, CGAServiceProtocol::GetCraftsInfo, skill_index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA(__FUNCTION__); }
				catch (msgpack::parse_error &e) {}
			}
			return false;
		}
		virtual bool RegisterServerShutdownNotify(const std::function<void(int)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyServerShutdown,
						[this, callback](int port) {
						m_connected = false;
						if (callback) 
							callback(port);
					},
						[](auto const& e) {
					}
					);
				}
				catch (timax::rpc::exception const& e) {}
			}
			return false;
		}
		virtual bool RegisterBattleActionNotify(const std::function<void(int)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyBattleAction,
						[callback](int flags) { 
						if (callback) 
							callback(flags);
					},
						[](auto const& e) {}
					);
					return true;
				}
				catch (timax::rpc::exception const& e) {}
			}
			return false;
		}
		virtual bool RegisterPlayerMenuNotify(const std::function<void(cga_player_menu_items_t)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyPlayerMenu,
						[callback](cga_player_menu_items_t p) {
						if (callback)
							callback(p);
					},
						[](auto const& e) {}
					);
					return true;
				}
				catch (timax::rpc::exception const& e) {}
			}
			return false;
		}
		virtual bool RegisterUnitMenuNotify(const std::function<void(cga_unit_menu_items_t)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyUnitMenu,
						[callback](cga_unit_menu_items_t u) {
						if (callback)
							callback(u);
					},
						[](auto const& e) {}
					);
					return true;
				}
				catch (timax::rpc::exception const& e) {}
			}
			return false;
		}
		virtual bool RegisterNPCDialogNotify(const std::function<void(cga_npc_dialog_t)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyNPCDialog,
						[callback](cga_npc_dialog_t dlg) {
						if (callback)
							callback(dlg);
					},
						[](auto const& e) {}
					);
					return true;
				}
				catch (timax::rpc::exception const& e) {}
			}
			return false;
		}
		virtual bool RegisterWorkingResultNotify(const std::function<void(cga_working_result_t)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyWorkingResult,
						[callback](cga_working_result_t rs) {
						if (callback)
							callback(rs);
					},
						[](auto const& e) {
						OutputDebugStringA(e.get_error_message().c_str());
					}
					);
					return true;
				}
				catch (timax::rpc::exception const& e) {
					OutputDebugStringA(e.get_error_message().c_str());
				}
			}
			return false;
		}
		virtual bool RegisterChatMsgNotify(const std::function<void(cga_chat_msg_t)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyChatMsg,
						[callback](cga_chat_msg_t rs) {
						if (callback)
							callback(rs);
					},
						[](auto const& e) {
						OutputDebugStringA(e.get_error_message().c_str());
					}
					);
					return true;
				}
				catch (timax::rpc::exception const& e) {
					OutputDebugStringA(e.get_error_message().c_str());
				}
			}
			return false;
		}
	};

	CGAInterface *CreateInterface()
	{
		return new CGAService();
	}
}