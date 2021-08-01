#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP

#include "persistance/component.hpp"
#include "config/types.hpp"
#include "persistance/data_access_factory_impl.hpp"
#include <memory>

namespace spdlog { class logger; }
namespace nexuspool {
namespace persistance {

class Component_impl : public Component
{
public:
    Component_impl(std::shared_ptr<spdlog::logger> logger, config::Persistance_config config);

    Data_access_factory::Sptr get_data_access_factory() override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    config::Persistance_config m_config;
    Data_access_factory::Sptr m_data_access_factory;
};

}
}

#endif
