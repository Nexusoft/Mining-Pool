#ifndef NEXUSPOOL_CONFIG_TYPES_HPP
#define NEXUSPOOL_CONFIG_TYPES_HPP

#include <string>
#include <variant>

namespace nexuspool
{
namespace config
{
enum class Mining_mode : std::uint8_t
{
	PRIME = 0,
	HASH = 1
};

enum class Stats_printer_mode : std::uint8_t
{
	CONSOLE = 0,
	FILE
};

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
	std::string m_address{};
	std::uint16_t m_fee{};
	std::uint32_t m_min_share{};
};
}
}
#endif
