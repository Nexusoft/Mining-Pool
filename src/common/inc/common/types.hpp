#ifndef NEXUSPOOL_COMMON_TYPES_HPP
#define NEXUSPOOL_COMMON_TYPES_HPP

#include <string>
#include <variant>

namespace nexuspool
{
namespace common
{
enum class Mining_mode : std::uint8_t
{
	PRIME = 0,
	HASH = 1
};

}
}
#endif
