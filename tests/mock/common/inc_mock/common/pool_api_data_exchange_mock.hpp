#ifndef NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_MOCK_HPP
#define NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_MOCK_HPP

#include "gmock/gmock.h"
#include "common/pool_api_data_exchange.hpp"

namespace nexuspool {
namespace chrono {

class Pool_api_data_exchange_mock : public Pool_api_data_exchange
{
public:
    MOCK_METHOD(std::uint32_t, get_active_miners, (), (const override));
    MOCK_METHOD(void, set_active_miners, (std::uint32_t active_miners), (override));
    MOCK_METHOD(bool, is_config_updated, (), (const override));
    MOCK_METHOD(void, set_config_updated, (bool update), (override));
    MOCK_METHOD(Mining_info, get_mining_info, (), (const override));
    MOCK_METHOD(void, set_mining_info, (Mining_info const& mining_info), (override));
};

}
}

#endif

