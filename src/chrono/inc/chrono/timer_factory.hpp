#ifndef NEXUSPOOL_CHRONO_TIMER_FACTORY_HPP
#define NEXUSPOOL_CHRONO_TIMER_FACTORY_HPP

#include "timer.hpp"
#include <memory>

namespace nexuspool {
namespace chrono {

class Timer_factory
{
public:
    using Sptr = std::shared_ptr<Timer_factory>;
    using Wptr = std::weak_ptr<Timer_factory>;

    virtual ~Timer_factory() = default;

    virtual Timer::Uptr create_timer() = 0;
};

}
}

#endif 
