#ifndef NEXUSPOOL_PERSISTANCE_DATA_WRITER_FACTORY_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_WRITER_FACTORY_IMPL_HPP

#include "persistance/data_writer_factory.hpp"
#include "persistance/data_writer_impl.hpp"
#include "persistance/sqlite/storage_manager_impl.hpp"
#include "persistance/sqlite/command/command_factory_impl.hpp"
#include "config/types.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool {
namespace persistance {

class Data_writer_factory_impl : public Data_writer_factory
{
public:

    Data_writer_factory_impl(std::shared_ptr<spdlog::logger> logger,
        config::Persistance_config config,
        persistance::Data_storage_factory::Sptr data_storage_factory)
        : m_logger{ std::move(logger) }
        , m_config{ std::move(config) }
        , m_data_storage_factory{ std::move(data_storage_factory) }
        , m_shared_data_writer{}
    {
    }

private:
    std::shared_ptr<spdlog::logger> m_logger;
    config::Persistance_config m_config;
    persistance::Data_storage_factory::Sptr m_data_storage_factory;
    Shared_data_writer::Sptr m_shared_data_writer;

    Shared_data_writer::Sptr create_shared_data_writer_impl() override
    {
        if (m_shared_data_writer == nullptr)
        {
            // first time initialisation
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

            auto data_writer = std::make_unique<Data_writer_impl>(m_logger, m_data_storage_factory->create_data_storage(),
                std::make_shared<command::Command_factory_impl>(m_logger, std::move(storage_manager)));

            m_shared_data_writer = std::make_shared<Shared_data_writer_impl>(std::move(data_writer));
        }

        return m_shared_data_writer;
    }
};

}
}

#endif
