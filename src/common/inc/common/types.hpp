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

}
}
#endif
