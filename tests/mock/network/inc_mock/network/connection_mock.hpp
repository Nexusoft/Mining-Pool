#ifndef NEXUSPOOL_NETWORK_CONNECTION_MOCK_HPP
#define NEXUSPOOL_NETWORK_CONNECTION_MOCK_HPP

#include "gmock/gmock.h"
#include "network/connection.hpp"

namespace nexuspool {
namespace network {

class Connection_mock : public Connection 
{
public:

    MOCK_METHOD(Endpoint const&, remote_endpoint, (), (const, override));
    MOCK_METHOD(Endpoint const&, local_endpoint, (), (const, override));
    MOCK_METHOD(void, transmit, (Shared_payload tx_buffer), (override));
    MOCK_METHOD(void, close, (), (override));
};

}
}

#endif

