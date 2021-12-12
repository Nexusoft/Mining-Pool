
#include "common/pool_api_data_exchange_impl.hpp"

namespace nexuspool {
namespace common {

std::uint32_t Pool_api_data_exchange_impl::get_active_miners() const
{
    std::scoped_lock lock(m_miners_mutex);

    return m_active_miners;
}

void Pool_api_data_exchange_impl::set_active_miners(std::uint32_t active_miners)
{
    std::scoped_lock lock(m_miners_mutex);

    m_active_miners = active_miners;
}


}
}