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
    MOCK_METHOD(void, set_active_miners, (), (override));
};

}
}

#endif

