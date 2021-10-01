#ifndef NEXUSPOOL_CHRONO_COMPONENT_HPP
#define NEXUSPOOL_CHRONO_COMPONENT_HPP

#include <memory>
#include "chrono/timer_factory.hpp"

namespace nexuspool {
namespace chrono {

class Component 
{
public:

    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual Timer_factory::Sptr get_timer_factory() = 0;
};

}
}

#endif 
