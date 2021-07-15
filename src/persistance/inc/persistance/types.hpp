#ifndef NEXUSPOOL_PERSISTANCE_TYPES_HPP
#define NEXUSPOOL_PERSISTANCE_TYPES_HPP

#include <memory>
#include <string>
#include <map>

namespace nexuspool {
namespace persistance {

struct pool_data
{
public:
	pool_data()
	{
		//nRound = Core::nCurrentRound;
		//nBlockNumber = Core::nBestHeight;
		//nRoundReward = Core::nRoundReward;
		//nTotalShares = Core::TotalWeight();
		//nConnectionCount = Core::nConnections;

	}

	uint32_t m_round;
	uint32_t m_block_number;
	uint64_t m_round_reward;
	uint64_t m_total_shares;
	uint32_t m_connection_count;
};

struct account_data
{
	std::string m_account_address;
	int m_connections;
	uint64_t m_shares;
	uint64_t m_balance;
	uint64_t m_pending_payout;
};

struct round_data
{
	explicit round_data(uint32_t round_to_set) : m_round{ round_to_set } {}
	round_data()
	{
		//round = Core::nCurrentRound;
		//nBlockNumber = Core::nBestHeight;
		//hashBlock = Core::hashBlockSubmission.ToString();
		//nRoundReward = Core::nRoundReward;
		//nTotalShares = Core::TotalWeight();
		//strBlockFinder = Core::LAST_ROUND_BLOCKFINDER;
		//bOrphan = false;
		//tBlockFoundTime = time(0);
	}


	uint32_t m_round;
	uint32_t m_block_number;
	std::string m_hash_block;
	uint64_t m_round_reward;
	uint64_t m_total_shares;
	std::string m_block_finder;
	bool m_orphan;
	time_t m_block_found_time;
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
