#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP

#include "persistance/component.hpp"
#include "persistance/data_storage_factory_impl.hpp"
#include "config/config.hpp"
#include <memory>

namespace nexuspool {
namespace persistance {

class Component_impl : public Component
{
public:
    Component_impl(config::Config& config)
        : m_data_storage_factory{ std::make_shared<Data_storage_factory_impl>(config) }
    {
    }

    Data_storage_factory::Sptr get_data_storage_factory() override { return m_data_storage_factory; }

private:
    Data_storage_factory::Sptr m_data_storage_factory;
};

}
}

#endif
