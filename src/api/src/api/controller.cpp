#include "api/controller.hpp"


namespace nexuspool
{
namespace api
{
persistance::Config_data Rest_controller::get_config_data()
{
    auto config_updated = m_pool_api_data_exchange->is_config_updated();
    if (config_updated)
    {
        m_cached_config = m_data_reader->get_config();
        config_updated = false;
        m_pool_api_data_exchange->set_config_updated(config_updated);
    }
    return m_cached_config;
}

}
}