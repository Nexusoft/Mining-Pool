#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP

#include "persistance/component.hpp"
#include "persistance/data_storage_factory_impl.hpp"
#include "persistance/command/command_factory_impl.hpp"
#include "persistance/storage_manager.hpp"
#include <memory>

namespace spdlog { class logger; }
namespace nexuspool {
namespace config { class Config; }
namespace persistance {

class Component_impl : public Component
{
public:
    Component_impl(std::shared_ptr<spdlog::logger> logger, config::Config& config);

    void start() override;
    void stop() override;

    Data_storage_factory::Sptr get_data_storage_factory() override;
    command::Command_factory::Sptr get_command_factory() override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    config::Config& m_config;
    Data_storage_factory::Sptr m_data_storage_factory;
    Storage_manager::Sptr m_storage_manager;
    command::Command_factory::Sptr m_command_factory;
};

}
}

#endif
