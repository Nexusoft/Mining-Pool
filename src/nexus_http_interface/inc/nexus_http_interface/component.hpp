#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_HPP

#include <memory>
#include <string>

namespace nexuspool {
namespace nexus_http_interface {

class Component 
{
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual bool get_block(std::string hash) = 0;

};


}
}

#endif 
