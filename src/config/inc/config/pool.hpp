#ifndef NEXUSPOOL_CONFIG_POOL_HPP
#define NEXUSPOOL_CONFIG_POOL_HPP

#include <string>

namespace nexuspool
{
namespace config
{

struct Pool
{
    std::string m_address{};
    std::uint16_t m_fee{};
    std::uint32_t m_min_share{};
};

}
}
#endif