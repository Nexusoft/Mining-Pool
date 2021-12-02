#ifndef NEXUSPOOL_NETWORK_CREATE_COMPONENT_HPP
#define NEXUSPOOL_NETWORK_CREATE_COMPONENT_HPP

#include "network/component.hpp"
#include "asio/io_service.hpp"

#include <memory>

namespace nexuspool {
namespace network {

// Component factory

Component::Uptr create_component(std::shared_ptr<::asio::io_context> io_context);

}
} 

#endif 
