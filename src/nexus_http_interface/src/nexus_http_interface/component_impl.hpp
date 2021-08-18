#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_IMPL_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_IMPL_HPP

#include "nexus_http_interface/component.hpp"
#include "nexus_http_interface/manager_impl.hpp"

namespace nexuspool {
namespace nexus_http_interface {

class Component_impl : public Component
{
public:
    Component_impl()
    {}

    Manager::Uptr create_nexus_http_interface_manager() override
    {
        return std::make_unique<Manager_impl>();
    }

private:

    

};

}
}

#endif
