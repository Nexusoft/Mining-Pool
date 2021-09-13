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
	double m_hashrate;
};

struct Account_data_for_payment
{
	std::string m_address;
	double m_shares;
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
	double m_shares;
	std::string m_payment_date_time;
	std::int64_t m_round;
};

struct Config_data
{
	std::string m_version;
	int m_fee;
	int m_difficulty_divider;
	std::string m_mining_mode;
	int m_round_duration_hours;
};

} // namespace database
} // namespace nexuspool

#endif
