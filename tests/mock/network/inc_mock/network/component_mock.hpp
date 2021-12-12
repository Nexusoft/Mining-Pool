#ifndef NEXUSPOOL_NETWORK_COMPONENT_MOCK_HPP
#define NEXUSPOOL_NETWORK_COMPONENT_MOCK_HPP

#include "gmock/gmock.h"
#include "network/component.hpp"


namespace nexuspool {
namespace network {

class Component_mock : public Component {
public:
    MOCK_METHOD(Socket_factory::Sptr, get_socket_factory, (), (override));
};

}
} 

#endif

