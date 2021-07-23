#include "persistance/component_impl.hpp"
#include "persistance/storage_manager_impl.hpp"
#include "persistance/data_storage_factory_impl.hpp"
#include "persistance/command/command_factory_impl.hpp"
#include "persistance/data_access_factory_impl.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool {
namespace persistance {

Component_impl::Component_impl(std::shared_ptr<spdlog::logger> logger, config::Persistance_config config)
    : m_logger{std::move(logger)}
    , m_config{std::move(config)}
{
}

Data_storage_factory::Sptr Component_impl::get_data_storage_factory() 
{ 
    return m_data_storage_factory; 
}

command::Command_factory::Sptr Component_impl::get_command_factory()  
{
    return m_command_factory; 
}

Data_access_factory::Sptr Component_impl::get_data_access_factory()
{
    return m_data_access_factory;
}

void Component_impl::start()
{
    m_data_storage_factory = std::make_shared<Data_storage_factory_impl>(m_logger);

    switch (m_config.m_type)
    {
    case config::Persistance_type::database:
    case config::Persistance_type::sqlite:
    {
        m_storage_manager = std::make_shared<Storage_manager_sqlite>(m_logger, m_config.m_file);
        break;
    }
    default:
    {
        m_logger->critical("Unsupported persistance type!");
        std::exit(1);       // cant do anything without persistance module
    }
    }
    m_storage_manager->start();
    m_command_factory = std::make_shared<command::Command_factory_impl>(m_logger, m_storage_manager);
    m_data_access_factory = std::make_shared<Data_access_factory_impl>(m_logger, m_data_storage_factory->create_data_storage(), m_command_factory);
}

void Component_impl::stop()
{
    m_command_factory.reset();
    // shutdown db etc
    m_storage_manager->stop();
}

}
}
