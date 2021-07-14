#ifndef NEXUSPOOL_PERSISTANCE_DATA_STORAGE_FACTORY_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_STORAGE_FACTORY_IMPL_HPP

#include "persistance/data_storage_factory.hpp"
#include "persistance/data_storage_impl.hpp"
#include "config/config.hpp"

namespace nexuspool {
namespace persistance {

class Data_storage_factory_impl : public Data_storage_factory
{
public:

    Data_storage_factory_impl(config::Config& config)
        : m_config{ config }
    {
    }


private:
    config::Config& m_config;

    Data_storage::Sptr create_data_storage_impl() override
    {
        Data_storage::Sptr result{};

        result = std::make_shared<Data_storage_impl>();

        return result;
    }
};
}
}

#endif
