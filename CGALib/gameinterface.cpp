#define _WIN32_WINNT 0x0501
#include <rest_rpc/client.hpp>
#include <rest_rpc/server.hpp>
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
	TIMAX_DEFINE_PROTOCOL(GetBGMIndex, int());
	TIMAX_DEFINE_PROTOCOL(GetSysTime, cga_sys_time_t());
	TIMAX_DEFINE_PROTOCOL(GetPlayerInfo, cga_player_info_t());
	TIMAX_DEFINE_PROTOCOL(SetPlayerFlagEnabled, void(int, bool));
	TIMAX_DEFINE_PROTOCOL(IsPlayerFlagEnabled, bool(int));
	TIMAX_DEFINE_PROTOCOL(IsPetValid, bool(int));
	TIMAX_DEFINE_PROTOCOL(GetPetInfo, cga_pet_info_t(int));
	TIMAX_DEFINE_PROTOCOL(GetPetsInfo, cga_pets_info_t());
	TIMAX_DEFINE_PROTOCOL(GetBankPetsInfo, cga_pets_info_t());
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
	TIMAX_DEFINE_PROTOCOL(GetBankGold, int());
	TIMAX_DEFINE_PROTOCOL(UseItem, bool(int));
	TIMAX_DEFINE_PROTOCOL(MoveItem, bool(int, int, int));
	TIMAX_DEFINE_PROTOCOL(MovePet, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(MoveGold, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(DropItem, bool(int));
	TIMAX_DEFINE_PROTOCOL(DropPet, bool(int));
	TIMAX_DEFINE_PROTOCOL(ChangePetState, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(GetMapIndex, std::tuple<int, int, int>());
	TIMAX_DEFINE_PROTOCOL(GetMapXY, std::tuple<int, int>());
	TIMAX_DEFINE_PROTOCOL(GetMapXYFloat, std::tuple<float, float>());
	TIMAX_DEFINE_PROTOCOL(GetMoveSpeed, std::tuple<float, float>());
	TIMAX_DEFINE_PROTOCOL(GetMapName, std::string());
	TIMAX_DEFINE_PROTOCOL(GetMapUnits, cga_map_units_t());
	TIMAX_DEFINE_PROTOCOL(GetMapTileTable, cga_map_cells_t(bool));
	TIMAX_DEFINE_PROTOCOL(GetMapCollisionTable, cga_map_cells_t(bool));
	TIMAX_DEFINE_PROTOCOL(GetMapCollisionTableRaw, cga_map_cells_t(bool));
	TIMAX_DEFINE_PROTOCOL(GetMapObjectTable, cga_map_cells_t(bool));
	TIMAX_DEFINE_PROTOCOL(WalkTo, void(int,int));
	TIMAX_DEFINE_PROTOCOL(TurnTo, void(int, int));
	TIMAX_DEFINE_PROTOCOL(ForceMove, bool(int, bool));
	TIMAX_DEFINE_PROTOCOL(ForceMoveTo, bool(int, int, bool));
	TIMAX_DEFINE_PROTOCOL(IsMapCellPassable, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(SetMoveSpeed, void(int));
	TIMAX_DEFINE_PROTOCOL(LogBack, void());
	TIMAX_DEFINE_PROTOCOL(LogOut, void());
	TIMAX_DEFINE_PROTOCOL(SayWords, void(std::string, int, int, int));
	TIMAX_DEFINE_PROTOCOL(ChangeNickName, bool(std::string));
	TIMAX_DEFINE_PROTOCOL(ChangeTitleName, bool(int));
	TIMAX_DEFINE_PROTOCOL(ChangePersDesc, void(cga_pers_desc_t));
	TIMAX_DEFINE_PROTOCOL(ClickNPCDialog, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(SellNPCStore, bool(cga_sell_items_t));
	TIMAX_DEFINE_PROTOCOL(BuyNPCStore, bool(cga_buy_items_t));
	TIMAX_DEFINE_PROTOCOL(PlayerMenuSelect, bool(int, std::string));
	TIMAX_DEFINE_PROTOCOL(UnitMenuSelect, bool(int));
	TIMAX_DEFINE_PROTOCOL(UpgradePlayer, void(int));
	TIMAX_DEFINE_PROTOCOL(UpgradePet, void(int,int));
	TIMAX_DEFINE_PROTOCOL(IsBattleUnitValid, bool(int));
	TIMAX_DEFINE_PROTOCOL(GetBattleUnit, cga_battle_unit_t(int));
	TIMAX_DEFINE_PROTOCOL(GetBattleUnits, cga_battle_units_t());
	TIMAX_DEFINE_PROTOCOL(GetBattleContext, cga_battle_context_t());
	TIMAX_DEFINE_PROTOCOL(BattleNormalAttack, bool(int));
	TIMAX_DEFINE_PROTOCOL(BattleSkillAttack, bool(int, int, int, bool));
	TIMAX_DEFINE_PROTOCOL(BattleRebirth, bool());
	TIMAX_DEFINE_PROTOCOL(BattleGuard, bool());
	TIMAX_DEFINE_PROTOCOL(BattleEscape, bool());
	TIMAX_DEFINE_PROTOCOL(BattleExchangePosition, bool());
	TIMAX_DEFINE_PROTOCOL(BattleChangePet, bool(int));
	TIMAX_DEFINE_PROTOCOL(BattleUseItem, bool(int, int));
	TIMAX_DEFINE_PROTOCOL(BattlePetSkillAttack, bool(int, int, bool));
	TIMAX_DEFINE_PROTOCOL(BattleDoNothing, bool());
	TIMAX_DEFINE_PROTOCOL(BattleSetHighSpeedEnabled, void(bool));
	TIMAX_DEFINE_PROTOCOL(BattleSetShowHPMPEnabled, void(bool));
	TIMAX_DEFINE_PROTOCOL(GetBattleEndTick, int());
	TIMAX_DEFINE_PROTOCOL(SetBattleEndTick, void(int));
	TIMAX_DEFINE_PROTOCOL(SetWorkDelay, void(int));
	TIMAX_DEFINE_PROTOCOL(SetWorkAcceleration, void(int));
	TIMAX_DEFINE_PROTOCOL(SetImmediateDoneWork, void(bool));
	TIMAX_DEFINE_PROTOCOL(GetImmediateDoneWorkState, int(void));
	TIMAX_DEFINE_PROTOCOL(StartWork, bool(int,int));
	TIMAX_DEFINE_PROTOCOL(CraftItem, bool(cga_craft_item_t));
	TIMAX_DEFINE_PROTOCOL(AssessItem, bool(int,int));
	TIMAX_DEFINE_PROTOCOL(GetCraftInfo, cga_craft_info_t(int,int));
	TIMAX_DEFINE_PROTOCOL(GetCraftsInfo, cga_crafts_info_t(int));
	TIMAX_DEFINE_PROTOCOL(GetCraftStatus, int());
	TIMAX_DEFINE_PROTOCOL(DoRequest, bool(int));
	TIMAX_DEFINE_PROTOCOL(EnableFlags, bool(int, bool));
	TIMAX_DEFINE_PROTOCOL(TradeAddStuffs, void(cga_sell_items_t, cga_sell_pets_t, int));
	TIMAX_DEFINE_PROTOCOL(GetTeamPlayerInfo, cga_team_players_t());
	TIMAX_DEFINE_PROTOCOL(FixMapWarpStuck, void(int));
	TIMAX_DEFINE_PROTOCOL(SetNoSwitchAnim, void(bool));
	TIMAX_DEFINE_PROTOCOL(GetMoveHistory, std::vector<DWORD>());
	TIMAX_DEFINE_PROTOCOL(SetWindowResolution, void(int, int));
	TIMAX_DEFINE_PROTOCOL(RequestDownloadMap, void(int, int, int, int));
	TIMAX_DEFINE_PROTOCOL(GetNextAnimTickCount, double());
	TIMAX_DEFINE_PROTOCOL(IsUIDialogPresent, bool(int));
	TIMAX_DEFINE_PROTOCOL(LoginGameServer, void(std::string, std::string, int, int, int, int));
	TIMAX_DEFINE_FORWARD(NotifyServerShutdown, int);
	TIMAX_DEFINE_FORWARD(NotifyBattleAction, int);
	TIMAX_DEFINE_FORWARD(NotifyPlayerMenu, cga_player_menu_items_t);
	TIMAX_DEFINE_FORWARD(NotifyUnitMenu, cga_unit_menu_items_t);
	TIMAX_DEFINE_FORWARD(NotifyNPCDialog, cga_npc_dialog_t);
	TIMAX_DEFINE_FORWARD(NotifyWorkingResult, cga_working_result_t);
	TIMAX_DEFINE_FORWARD(NotifyChatMsg, cga_chat_msg_t);
	TIMAX_DEFINE_FORWARD(NotifyTradeStuffs, cga_trade_stuff_info_t);
	TIMAX_DEFINE_FORWARD(NotifyTradeDialog, cga_trade_dialog_t);
	TIMAX_DEFINE_FORWARD(NotifyTradeState, int);
	TIMAX_DEFINE_FORWARD(NotifyDownloadMap, cga_download_map_t);
	TIMAX_DEFINE_FORWARD(NotifyConnectionState, cga_conn_state_t);
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
					m_connected = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::Connect);
				}
				catch (timax::rpc::exception const &e) { m_connected = false; }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return m_connected;
		}
		virtual bool Initialize(cga_game_data_t &data) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::Initialize, data);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool IsInGame(int &ingame) {
			if (m_connected) {
				try {
					ingame = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::IsInGame);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetWorldStatus(int &status) {
			if (m_connected) {
				try {
					status = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetWorldStatus);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetGameStatus(int &status) {
			if (m_connected) {
				try {
					status = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetGameStatus);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetBGMIndex(int &status) {
			if (m_connected) {
				try {
					status = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetBGMIndex);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetSysTime(cga_sys_time_t &t) {
			if (m_connected) {
				try {
					t = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetSysTime);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetPlayerInfo(cga_player_info_t &info){
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetPlayerInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SetPlayerFlagEnabled(int index, bool enable) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SetPlayerFlagEnabled, index, enable);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool IsPlayerFlagEnabled(int index, bool &enable) {
			if (m_connected) {
				try {
					enable = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::IsPlayerFlagEnabled, index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool IsPetValid(int petid, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::IsPetValid, petid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetPetInfo(int petid, cga_pet_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetPetInfo, petid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetPetsInfo(cga_pets_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetPetsInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetBankPetsInfo(cga_pets_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetBankPetsInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool IsPetSkillValid(int petid, int skillid, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::IsPetSkillValid, petid, skillid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetPetSkillInfo(int petid, int skillpos, cga_pet_skill_info_t &skill) {
			if (m_connected) {
				try {
					skill = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetPetSkillInfo, petid, skillpos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetPetSkillsInfo(int petid, cga_pet_skills_info_t &skills) {
			if (m_connected) {
				try {
					skills = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetPetSkillsInfo, petid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool IsSkillValid(int skillid, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::IsSkillValid, skillid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetSkillInfo(int skillid, cga_skill_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetSkillInfo, skillid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetSkillsInfo(cga_skills_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetSkillsInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetSubSkillInfo(int skillid, int stage, cga_subskill_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetSubSkillInfo, skillid, stage);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return 0;
		}
		virtual bool GetSubSkillsInfo(int skillid, cga_subskills_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetSubSkillsInfo, skillid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return 0;
		}
		virtual bool IsItemValid(int itempos, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::IsItemValid, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetItemInfo(int itempos, cga_item_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetItemInfo, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetItemsInfo(cga_items_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetItemsInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetBankItemsInfo(cga_items_info_t &info) {
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetBankItemsInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetBankGold(int &gold) {
			if (m_connected) {
				try {
					gold = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetBankGold);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool UseItem(int itempos, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::UseItem, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool MoveItem(int itempos, int dstpos, int count, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::MoveItem, itempos, dstpos, count);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool MovePet(int itempos, int dstpos, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::MovePet, itempos, dstpos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool MoveGold(int gold, int opt, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::MoveGold, gold, opt);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool DropItem(int itempos, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::DropItem, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool DropPet(int petpos, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::DropPet, petpos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool ChangePetState(int petpos, int state, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::ChangePetState, petpos, state);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMapIndex(int &index1, int &index2, int &index3) {
			if (m_connected) {
				try {
					std::tuple<int, int, int> tup = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMapIndex);
					index1 = std::get<0>(tup);
					index2 = std::get<1>(tup);
					index3 = std::get<2>(tup);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMapXY(int &x, int &y) {
			if (m_connected) {
				try {
					std::tuple<int, int> tup = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMapXY);
					x = std::get<0>(tup);
					y = std::get<1>(tup);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMapXYFloat(float &x, float &y) {
			if (m_connected) {
				try {
					std::tuple<float, float> tup = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMapXYFloat);
					x = std::get<0>(tup);
					y = std::get<1>(tup);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMoveSpeed(float &x, float &y) {
			if (m_connected) {
				try {
					std::tuple<float, float> tup = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMoveSpeed);
					x = std::get<0>(tup);
					y = std::get<1>(tup);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMapName(std::string &name) {
			if (m_connected) {
				try {
					name = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMapName);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMapUnits(cga_map_units_t &units) {
			if (m_connected) {
				try {
					units = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMapUnits);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMapCollisionTable(bool loadall, cga_map_cells_t &cells) {
			if (m_connected) {
				try {
					cells = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMapCollisionTable, loadall);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMapCollisionTableRaw(bool loadall, cga_map_cells_t &cells) {
			if (m_connected) {
				try {
					cells = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMapCollisionTableRaw, loadall);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMapObjectTable(bool loadall, cga_map_cells_t &cells) {
			if (m_connected) {
				try {
					cells = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMapObjectTable, loadall);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMapTileTable(bool loadall, cga_map_cells_t &cells) {
			if (m_connected) {
				try {
					cells = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMapTileTable, loadall);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool WalkTo(int x, int y) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::WalkTo, x, y);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool TurnTo(int x, int y) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::TurnTo, x, y);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool ForceMove(int dir, bool show, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::ForceMove, dir, show);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool ForceMoveTo(int x, int y, bool show, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::ForceMoveTo, x, y, show);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool IsMapCellPassable(int x, int y, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::IsMapCellPassable, x, y);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SetMoveSpeed(int speed) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SetMoveSpeed, speed);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool LogBack() {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::LogBack);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool LogOut() {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::LogOut);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SayWords(std::string &str, int color , int range, int size) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SayWords, str, color, range, size);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool ChangeNickName(std::string &str, bool &bresult) {
			if (m_connected) {
				try {
					bresult = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::ChangeNickName, str);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool ChangeTitleName(int titleId, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::ChangeTitleName, titleId);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool ChangePersDesc(CGA::cga_pers_desc_t &desc) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::ChangePersDesc, desc);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool ClickNPCDialog(int option, int index, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::ClickNPCDialog, option, index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SellNPCStore(cga_sell_items_t &items, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SellNPCStore, items);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BuyNPCStore(cga_buy_items_t &items, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BuyNPCStore, items);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool PlayerMenuSelect(int menuindex, std::string &menustring, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::PlayerMenuSelect, menuindex, menustring);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool UnitMenuSelect(int menuindex, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::UnitMenuSelect, menuindex);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool UpgradePlayer(int attr) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::UpgradePlayer, attr);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool UpgradePet(int petid, int attr) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::UpgradePet, petid, attr);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool IsBattleUnitValid(int pos, bool &valid) {
			if (m_connected) {
				try {
					valid = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::IsBattleUnitValid, pos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetBattleUnit(int pos, cga_battle_unit_t &unit) {
			if (m_connected) {
				try {
					unit = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetBattleUnit, pos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetBattleUnits(cga_battle_units_t &units) {
			if (m_connected) {
				try {
					units = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetBattleUnits);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetBattleContext(cga_battle_context_t &ctx) {
			if (m_connected) {
				try {
					ctx = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetBattleContext);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleNormalAttack(int target, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleNormalAttack, target);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleSkillAttack(int skillpos, int skilllv, int target, bool packetOnly, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleSkillAttack, skillpos, skilllv, target, packetOnly);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleRebirth(bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleRebirth);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleGuard(bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleGuard);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleEscape(bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleEscape);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleExchangePosition(bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleExchangePosition);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleChangePet(int petid, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleChangePet, petid);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleUseItem(int itempos, int target, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleUseItem, itempos, target);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattlePetSkillAttack(int skillpos, int target, bool packetOnly, bool &result) {
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattlePetSkillAttack, skillpos, target, packetOnly);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleDoNothing(bool &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleDoNothing);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleSetHighSpeedEnabled(bool enable) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleSetHighSpeedEnabled, enable);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool BattleSetShowHPMPEnabled(bool enable) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::BattleSetShowHPMPEnabled, enable);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetBattleEndTick(int &msec) {
			if (m_connected) {
				try {
					msec = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetBattleEndTick);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SetBattleEndTick(int msec) {
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SetBattleEndTick, msec);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SetWorkDelay(int delay)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SetWorkDelay, delay);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SetWorkAcceleration(int percent)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SetWorkAcceleration, percent);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SetImmediateDoneWork(bool enable)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SetImmediateDoneWork, enable);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetImmediateDoneWorkState(int &state)
		{
			if (m_connected) {
				try {
					state = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetImmediateDoneWorkState);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool StartWork(int skill_index, int sub_index, bool &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::StartWork, skill_index, sub_index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool CraftItem(cga_craft_item_t &craft, bool &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::CraftItem, craft);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) {
					OutputDebugStringA(e.what());
				}
			}
			return false;
		}
		virtual bool AssessItem(int skill_index, int itempos, bool &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::AssessItem, skill_index, itempos);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetCraftInfo(int skill_index, int sub_index, cga_craft_info_t &info)
		{
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetCraftInfo, skill_index, sub_index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetCraftsInfo(int skill_index, cga_crafts_info_t &info)
		{
			if (m_connected) {
				try {
					info = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetCraftsInfo, skill_index);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}

		virtual bool GetCraftStatus(int &st)
		{
			if (m_connected) {
				try {
					st = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetCraftStatus);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool DoRequest(int request_type, bool &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::DoRequest, request_type);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool EnableFlags(int type, bool enable, bool &result) 
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::EnableFlags, type, enable);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool TradeAddStuffs(cga_sell_items_t items, cga_sell_pets_t pets, int gold)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::TradeAddStuffs, items, pets, gold);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetTeamPlayerInfo(cga_team_players_t &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetTeamPlayerInfo);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool FixMapWarpStuck(int type)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::FixMapWarpStuck, type);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SetNoSwitchAnim(bool enable)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SetNoSwitchAnim, enable);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetMoveHistory(std::vector<DWORD> &v)
		{
			if (m_connected) {
				try {
					v = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetMoveHistory);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool SetWindowResolution(int w, int h)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::SetWindowResolution, w, h);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool RequestDownloadMap(int xbottom, int ybottom, int xsize, int ysize)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::RequestDownloadMap, xbottom, ybottom, xsize, ysize);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool GetNextAnimTickCount(double &next_anim_tick)
		{
			if (m_connected) {
				try {
					next_anim_tick = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::GetNextAnimTickCount);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool LoginGameServer(const std::string &gid, const std::string &glt, int serverid, int bigServerIndex, int serverIndex, int character)
		{
			if (m_connected) {
				try {
					m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::LoginGameServer, gid, glt, serverid, bigServerIndex, serverIndex, character);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
			}
			return false;
		}
		virtual bool IsUIDialogPresent(int dialog, bool &result)
		{
			if (m_connected) {
				try {
					result = m_client.call(std::chrono::milliseconds(10000), m_endpoint, CGAServiceProtocol::IsUIDialogPresent, dialog);
					return true;
				}
				catch (timax::rpc::exception const &e) { if (e.get_error_code() != timax::rpc::error_code::TIMEOUT) m_connected = false; OutputDebugStringA("rpc exception from " __FUNCTION__); OutputDebugStringA(e.get_error_message().c_str()); }
				catch (msgpack::parse_error &e) { OutputDebugStringA("parse exception from " __FUNCTION__); OutputDebugStringA(e.what()); }
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
		virtual bool RegisterTradeStuffsNotify(const std::function<void(cga_trade_stuff_info_t)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyTradeStuffs,
						[callback](cga_trade_stuff_info_t rs) {
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
		virtual bool RegisterTradeDialogNotify(const std::function<void(cga_trade_dialog_t)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyTradeDialog,
						[callback](cga_trade_dialog_t rs) {
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
		virtual bool RegisterTradeStateNotify(const std::function<void(int)> &callback) 
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyTradeState,
						[callback](int rs) {
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
		virtual bool RegisterDownloadMapNotify(const std::function<void(cga_download_map_t)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyDownloadMap,
						[callback](cga_download_map_t rs) {
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
		virtual bool RegisterConnectionStateNotify(const std::function<void(cga_conn_state_t)> &callback)
		{
			if (m_connected)
			{
				try
				{
					m_async_client.sub(m_endpoint, CGAServiceProtocol::NotifyConnectionState,
						[callback](cga_conn_state_t msg) {
						if (callback)
							callback(msg);
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