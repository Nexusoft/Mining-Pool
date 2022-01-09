#ifndef NEXUSPOOL_POOL_MINER_CONNECTION_MOCK_HPP
#define NEXUSPOOL_POOL_MINER_CONNECTION_MOCK_HPP

#include <gmock/gmock.h>
#include "pool/miner_connection.hpp"
#include <memory>

namespace nexuspool
{
class Miner_connection_mock : public Miner_connection
{
public:

    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(network::Connection::Handler, connection_handler, (), (override));
    MOCK_METHOD(void, get_hashrate, (), (override));
};

}

#endif
