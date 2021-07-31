#include "persistance/component_impl.hpp"
#include "persistance/data_access_factory_impl.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool {
namespace persistance {

Component_impl::Component_impl(std::shared_ptr<spdlog::logger> logger, config::Persistance_config config)
    : m_logger{std::move(logger)}
    , m_config{std::move(config)}
    , m_data_access_factory{std::make_shared<Data_access_factory_impl>(m_logger, m_config)}
{
}

Data_access_factory::Sptr Component_impl::get_data_access_factory()
{
    return m_data_access_factory;
}

}
}
