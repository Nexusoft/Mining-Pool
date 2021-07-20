#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP

#include "persistance/component.hpp"
#include "persistance/data_storage_factory_impl.hpp"
#include "persistance/command/command_factory_impl.hpp"
#include "config/config.hpp"
#include <spdlog/spdlog.h>
#include <memory>

namespace nexuspool {
namespace persistance {

class Component_impl : public Component
{
public:
    Component_impl(std::shared_ptr<spdlog::logger> logger, config::Config& config)
        : m_data_storage_factory{ std::make_shared<Data_storage_factory_impl>(std::move(logger), config) }
        , m_command_factory{ std::make_shared<command::Command_factory_impl>(std::move(logger), config, nullptr) }  // TODO sqlite db handle
    {
    }

    Data_storage_factory::Sptr get_data_storage_factory() override { return m_data_storage_factory; }
    command::Command_factory::Sptr get_command_factory() override { return m_command_factory; }

private:
    Data_storage_factory::Sptr m_data_storage_factory;
    command::Command_factory::Sptr m_command_factory;
};

}
}

#endif
