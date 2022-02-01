#ifndef NEXUSPOOL_CONFIG_TYPES_HPP
#define NEXUSPOOL_CONFIG_TYPES_HPP

#include <string>
#include <variant>

namespace nexuspool
{
namespace config
{

enum class Persistance_type : std::uint8_t 
{
	database = 0,
	sqlite
};

struct Persistance_config
{
	Persistance_type m_type{ Persistance_type::database};
	std::string m_file{};
};

struct Pool_config
{
	std::string m_account{};
	std::string m_pin{};
	std::uint16_t m_fee{};
	std::string m_fee_address{};
	std::uint16_t m_difficulty_divider{};
	std::uint16_t m_round_duration_hours{};
	std::string m_nxs_api_user{};
	std::string m_nxs_api_pw{};
};

struct Hardware_config
{
	std::string m_model{};
	double m_hashrate{ 0.0 };
	double m_power_consumption{ 0.0 };
};

}
}
#endif
