#include "persistance/component_impl.hpp"
#include "persistance/data_reader_factory_impl.hpp"
#include "persistance/data_writer_impl.hpp"
#include "persistance/sqlite/storage_manager_impl.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool {
namespace persistance {

Component_impl::Component_impl(std::shared_ptr<spdlog::logger> logger, config::Persistance_config config)
    : m_logger{std::move(logger)}
    , m_config{std::move(config)}
    , m_data_storage_factory{ std::make_shared<Data_storage_factory_impl>(m_logger) }
    , m_data_reader_factory{std::make_shared<Data_reader_factory_impl>(m_logger, m_config, m_data_storage_factory)}
{
}

Data_reader_factory::Sptr Component_impl::get_data_reader_factory()
{
    return m_data_reader_factory;
}

Data_writer::Uptr Component_impl::create_data_writer()
{
    std::unique_ptr<Storage_manager> storage_manager;
    switch (m_config.m_type)
    {
    case config::Persistance_type::database:
    case config::Persistance_type::sqlite:
    {
        storage_manager = std::make_unique<Storage_manager_sqlite>(m_logger, m_config.m_file, false);
        break;
    }
    default:
    {
        m_logger->critical("Unsupported persistance type!");
        std::exit(1);       // cant do anything without persistance module
    }
    }
    storage_manager->start();

    return std::make_unique<Data_writer_impl>(m_logger, m_data_storage_factory->create_data_storage(),
        std::make_shared<command::Command_factory_impl>(m_logger, std::move(storage_manager)));
}

}
}
