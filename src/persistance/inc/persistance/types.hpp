#ifndef NEXUSPOOL_PERSISTANCE_TYPES_HPP
#define NEXUSPOOL_PERSISTANCE_TYPES_HPP

#include <memory>
#include <string>
#include <map>

namespace nexuspool {
namespace persistance {

struct Account_data
{
	std::string m_address;
	std::uint16_t m_connections;
	std::string m_created_at;
	std::string m_last_active;
	double m_shares;
	double m_balance;
	double m_hashrate;
};

struct Block_data
{
	std::string m_hash;
	std::uint32_t m_height;
	std::string m_type;
	double m_shares;
	double m_difficulty;
	bool m_orphan;
	std::string m_block_finder;
	std::uint32_t m_round;
	std::string m_block_found_time;
	double m_mainnet_reward;
};

struct Round_data
{
	std::int64_t m_round;
	double m_total_shares;
	double m_total_rewards;
	std::uint32_t m_blocks;
	std::uint32_t m_connection_count;
	std::string m_start_date_time;
	std::string m_end_date_time;
	bool m_is_active;
	bool m_is_paid;
};

struct Payment_data
{
	std::string m_account;
	double m_amount;
	std::string m_payment_date_time;
};

struct connection_miner_data
{
	connection_miner_data() : m_pps{ 0 }, m_wps{ 0 }, m_guid{ "" }
	{}
	explicit connection_miner_data(std::string guid)
		: m_pps{ 0 }, m_wps{ 0 }, m_guid{ std::move(guid) }
	{}

	double m_pps,
		m_wps;
	std::string m_guid;
};

struct connection_data
{
public:
	connection_data() : m_address{ "" } {}
	connection_data(std::string address) : m_address{ std::move(address) }
	{
		//	LASTSAVETIMER.Start();
	}


	double GetTotalPPS() const
	{
		double PPS = 0;
		for (auto iter = MINER_DATA.begin(); iter != MINER_DATA.end(); ++iter)
		{
			PPS += iter->second.m_pps;
		}

		return PPS;
	}
	double GetTotalWPS() const
	{
		double WPS = 0;
		for (auto iter = MINER_DATA.begin(); iter != MINER_DATA.end(); ++iter)
		{
			WPS += iter->second.m_wps;
		}

		return WPS;
	}

	std::string m_address;
	std::map<std::string, connection_miner_data> MINER_DATA;

	/** Used to track when this connection data was last persisted**/
	//LLP::Timer LASTSAVETIMER;
};

struct account_earning_transaction
{
	std::string m_account_address;
	uint32_t m_round;
	uint32_t m_block_number;
	uint64_t m_shares;
	uint64_t m_amount_earned;
	time_t m_time;

};

struct account_payment_transaction
{
	std::string m_account_address;
	uint32_t m_round;
	uint32_t m_block_number;
	uint32_t m_amount_paid;
	time_t m_time;

};

} // namespace database
} // namespace nexuspool

#endif
