#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP

#include "persistance/component.hpp"
#include "persistance/data_storage_factory_impl.hpp"
#include "persistance/command/command_factory_impl.hpp"
#include "persistance/storage_manager.hpp"
#include "config/config.hpp"
#include <spdlog/spdlog.h>
#include <memory>

namespace nexuspool {
namespace persistance {

class Component_impl : public Component
{
public:
    Component_impl(std::shared_ptr<spdlog::logger> logger, config::Config& config)
        : m_data_storage_factory{ std::make_shared<Data_storage_factory_impl>(logger, config) }
    {
        auto persistance_config = config.get_persistance_config();
        switch (persistance_config.m_type)
        {
            case config::Persistance_type::database:
            case config::Persistance_type::sqlite:
            {
                m_storage_manager = std::make_shared<Storage_manager_sqlite>(logger, persistance_config.m_file);
                break;
            }
        }

        m_command_factory = std::make_shared<command::Command_factory_impl>(std::move(logger), config, m_storage_manager);
    }

    Data_storage_factory::Sptr get_data_storage_factory() override { return m_data_storage_factory; }
    command::Command_factory::Sptr get_command_factory() override { return m_command_factory; }

private:

    Data_storage_factory::Sptr m_data_storage_factory;
    Storage_manager::Sptr m_storage_manager;
    command::Command_factory::Sptr m_command_factory;
};

}
}

#endif
