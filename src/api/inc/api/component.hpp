#ifndef NEXUSPOOL_API_COMPONENT_HPP
#define NEXUSPOOL_API_COMPONENT_HPP

#include <memory>

namespace nexuspool 
{
namespace api 
{

class Component 
{
public:

    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

};


}
}

#endif 
