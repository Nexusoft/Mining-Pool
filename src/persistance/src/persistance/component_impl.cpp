#include "persistance/component_impl.hpp"
#include "persistance/data_reader_factory_impl.hpp"
#include "persistance/data_writer_factory_impl.hpp"
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
    , m_data_writer_factory{ std::make_shared<Data_writer_factory_impl>(m_logger, m_config, m_data_storage_factory) }
{
    // create tables here so that they are available before and data_reader/writer setup their command_factory
    // create a tmp data_writer -> this setup the storage the first time before any user can create a reader or writer
    m_data_writer_factory->create_shared_data_writer();
}

Data_reader_factory::Sptr Component_impl::get_data_reader_factory()
{
    return m_data_reader_factory;
}

Data_writer_factory::Sptr Component_impl::get_data_writer_factory()
{
    return m_data_writer_factory;
}

}
}
