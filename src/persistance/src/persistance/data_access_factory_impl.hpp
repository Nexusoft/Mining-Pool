#ifndef NEXUSPOOL_PERSISTANCE_DATA_ACCESS_FACTORY_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_ACCESS_FACTORY_IMPL_HPP

#include "persistance/data_access_factory.hpp"
#include "persistance/data_access_impl.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool {
namespace persistance {

class Data_access_factory_impl : public Data_access_factory
{
public:

    Data_access_factory_impl(std::shared_ptr<spdlog::logger> logger, persistance::Data_storage::Sptr data_storage,
        std::shared_ptr<persistance::command::Command_factory> command_factory)
        : m_logger{ std::move(logger) }
        , m_data_storage{std::move(data_storage)}
        , m_command_factory{std::move(command_factory)}
    {
    }


private:
    std::shared_ptr<spdlog::logger> m_logger;
    persistance::Data_storage::Sptr m_data_storage;
    std::shared_ptr<persistance::command::Command_factory> m_command_factory;

    Data_access::Sptr create_data_access_impl() override
    {
        Data_access::Sptr result{};

        result = std::make_shared<Data_access_impl>(m_logger, m_data_storage, m_command_factory);

        return result;
    }
};

}
}

#endif
