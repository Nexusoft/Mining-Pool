#ifndef NEXUSPOOL_PERSISTANCE_DATA_STORAGE_FACTORY_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_STORAGE_FACTORY_IMPL_HPP

#include "persistance/data_storage_factory.hpp"
#include "persistance/sqlite/data_storage_impl.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool {
namespace persistance {

class Data_storage_factory_impl : public Data_storage_factory
{
public:

    Data_storage_factory_impl(std::shared_ptr<spdlog::logger> logger)
        : m_logger{ std::move(logger) }
    {
    }


private:
    std::shared_ptr<spdlog::logger> m_logger;

    Data_storage::Sptr create_data_storage_impl() override
    {
        Data_storage::Sptr result{};

        result = std::make_shared<sqlite::Data_storage_impl>(m_logger, "");

        return result;
    }
};
}
}

#endif
