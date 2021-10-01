#ifndef NEXUSPOOL_CHRONO_TIMER_FACTORY_MOCK_HPP
#define NEXUSPOOL_CHRONO_TIMER_FACTORY_MOCK_HPP

#include "gmock/gmock.h"
#include "chrono/timer_factory.hpp"

namespace nexuspool {
namespace chrono {

class Timer_factory_mock : public Timer_factory 
{
public:

    MOCK_METHOD(Timer::Uptr, create_timer, (), (override));
};

}
}

