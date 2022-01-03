#ifndef NEXUSPOOL_COMMON_TYPES_HPP
#define NEXUSPOOL_COMMON_TYPES_HPP

#include <string>

namespace nexuspool
{
namespace common
{
enum class Mining_mode : std::uint8_t
{
	PRIME = 0,
	HASH = 1
};

struct Block_reward_data
{
	std::string m_tx_type;
	std::string m_timestamp;
	double m_reward;
	std::uint32_t m_tx_confirmations;
};

struct Mining_info
{
	std::uint32_t m_height{ 0 };
	double m_prime_difficulty{ 0.0 };
	double m_hash_difficulty{ 0.0 };
	double m_prime_reward{ 0.0 };
	double m_hash_rewards{ 0.0 };

	bool is_valid() const { return m_height != 0; }
};

struct System_info
{
	std::string m_wallet_version{};
	std::string m_pool_version{};

	bool is_valid() const { return !m_wallet_version.empty(); }
};

}
}
#endif
