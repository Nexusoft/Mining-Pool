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
}
}
#endif
