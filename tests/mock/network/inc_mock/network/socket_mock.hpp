#ifndef NEXUSPOOL_NETWORK_SOCKET_MOCK_HPP
#define NEXUSPOOL_NETWORK_SOCKET_MOCK_HPP

#include "gmock/gmock.h"
#include "network/socket.hpp"


namespace nexuspool {
namespace network {

class Socket_mock : public Socket 
{
public:

    MOCK_METHOD(Connection::Uptr, connect, (Endpoint, Connection::Handler), (override));
    MOCK_METHOD(Result::Code, listen, (Connect_handler), (override));
    MOCK_METHOD(void, stop_listen, (), (override));
    MOCK_METHOD(Endpoint const&, local_endpoint, (), (const, override));
};

}
}

#endif

