#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_CREATE_COMPONENT_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_CREATE_COMPONENT_HPP

#include "nexus_http_interface/component.hpp"
#include <memory>

namespace nexuspool {
namespace nexus_http_interface {

// Component factory

Component::Uptr create_component();

}
}

#endif 
