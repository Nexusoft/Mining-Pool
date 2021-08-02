#ifndef NEXUSPOOL_PERSISTANCE_DATA_READER_FACTORY_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_READER_FACTORY_IMPL_HPP

#include "persistance/data_reader_factory.hpp"
#include "persistance/data_reader_impl.hpp"
#include "persistance/storage_manager_impl.hpp"

#include "persistance/command/command_factory_impl.hpp"
#include "config/types.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool {
namespace persistance {

class Data_reader_factory_impl : public Data_reader_factory
{
public:

    Data_reader_factory_impl(std::shared_ptr<spdlog::logger> logger, 
        config::Persistance_config config, 
        persistance::Data_storage_factory::Sptr data_storage_factory)
        : m_logger{ std::move(logger) }
        , m_config{std::move(config)}
        , m_data_storage_factory{ std::move(data_storage_factory)}
    {
    }


private:
    std::shared_ptr<spdlog::logger> m_logger;
    config::Persistance_config m_config;
    persistance::Data_storage_factory::Sptr m_data_storage_factory;

    Data_reader::Uptr create_data_reader_impl() override
    {
        std::unique_ptr<Storage_manager> storage_manager;
        switch (m_config.m_type)
        {
        case config::Persistance_type::database:
        case config::Persistance_type::sqlite:
        {
            storage_manager = std::make_unique<Storage_manager_sqlite>(m_logger, m_config.m_file);
            break;
        }
        default:
        {
            m_logger->critical("Unsupported persistance type!");
            std::exit(1);       // cant do anything without persistance module
        }
        }
        storage_manager->start();

        return std::make_unique<Data_reader_impl>(m_logger, m_data_storage_factory->create_data_storage(), 
            std::make_shared<command::Command_factory_impl>(m_logger, std::move(storage_manager)));
    }
};

}
}

#endif
