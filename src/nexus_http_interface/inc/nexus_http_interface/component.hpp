#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_HPP

#include "manager.hpp"
#include <memory>

namespace nexuspool {
namespace nexus_http_interface {

class Component 
{
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual Manager::Uptr create_nexus_http_interface_manager() = 0;

};


}
}

#endif 
