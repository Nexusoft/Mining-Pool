#ifndef NEXUSPOOL_NETWORK_SOCKET_FACTORY_MOCK_HPP
#define NEXUSPOOL_NETWORK_SOCKET_FACTORY_MOCK_HPP

#include "gmock/gmock.h"
#include "network/socket_factory.hpp"

namespace nexuspool {
namespace network {

class Socket_factory_mock : public Socket_factory
{
public:
    MOCK_METHOD(Socket::Uptr, create_socket_impl, (Endpoint), (override));
};

}
}

#endif

