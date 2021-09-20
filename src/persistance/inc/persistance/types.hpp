#ifndef NEXUSPOOL_PERSISTANCE_TYPES_HPP
#define NEXUSPOOL_PERSISTANCE_TYPES_HPP

#include <memory>
#include <string>
#include <map>

namespace nexuspool {
namespace persistance {

struct Account_data
{
	std::string m_address{};
	std::uint16_t m_connections{};
	std::string m_created_at{};
	std::string m_last_active{};
	double m_shares{ 0 };
	double m_hashrate{0};

	bool is_empty() const { return (m_address.empty()); }
};

struct Account_data_for_payment
{
	std::string m_address{};
	double m_shares{ 0 };

	bool is_empty() const { return (m_address.empty()); }
};

struct Block_data
{
	std::string m_hash{};
	std::uint32_t m_height{ 0 };
	std::string m_type{};
	double m_difficulty{ 0 };
	bool m_orphan{ false };
	std::string m_block_finder{};
	std::uint32_t m_round{ 0 };
	std::string m_block_found_time{};
	double m_mainnet_reward{ 0 };

	bool is_empty() const { return (m_hash.empty()); }
};

struct Round_data
{
	std::int64_t m_round{ 0 };
	double m_total_shares{ 0 };
	double m_total_rewards{ 0 };
	std::uint32_t m_blocks{ 0 };
	std::uint32_t m_connection_count{ 0 };
	std::string m_start_date_time{};
	std::string m_end_date_time{};
	bool m_is_active{ false };
	bool m_is_paid{ false };

	bool is_empty() const { return (m_round == 0); }
};

struct Payment_data
{
	std::string m_account{};
	double m_amount{ 0 };
	double m_shares{ 0 };
	std::string m_payment_date_time{};
	std::int64_t m_round{ 0 };

	bool is_empty() const { return (m_account.empty()); }
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
