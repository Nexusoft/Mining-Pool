#include "nexus_http_interface/create_component.hpp"
#include "nexus_http_interface/component_impl.hpp"

namespace nexuspool {
namespace nexus_http_interface {

Component::Uptr create_component()
{
    return std::make_unique<Component_impl>();
}

}
}
