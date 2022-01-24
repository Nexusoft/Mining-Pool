
#include "common/pool_api_data_exchange_impl.hpp"

namespace nexuspool 
{
namespace common 
{

std::uint32_t Pool_api_data_exchange_impl::get_active_miners() const
{
    return m_active_miners;
}

void Pool_api_data_exchange_impl::set_active_miners(std::uint32_t active_miners)
{
    m_active_miners = active_miners;
}

bool Pool_api_data_exchange_impl::is_config_updated() const
{
    return m_config_updated;
}

void Pool_api_data_exchange_impl::set_config_updated(bool update)
{
    m_config_updated = update;
}

std::string Pool_api_data_exchange_impl::get_payout_time() const
{
    std::scoped_lock lock(m_payout_time_mutex);

    return m_payout_time;
}

void Pool_api_data_exchange_impl::set_payout_time(std::string payout_time)
{
    std::scoped_lock lock(m_payout_time_mutex);

    m_payout_time = std::move(payout_time);
}

Mining_info Pool_api_data_exchange_impl::get_mining_info() const
{
    std::scoped_lock lock(m_mining_info_mutex);

    return m_mining_info;
}

void Pool_api_data_exchange_impl::set_mining_info(Mining_info const& mining_info)
{
    std::scoped_lock lock(m_mining_info_mutex);

    m_mining_info = mining_info;
}


}
}