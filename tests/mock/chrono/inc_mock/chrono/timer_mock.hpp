#ifndef NEXUSPOOL_CHRONO_TIMER_MOCK_HPP
#define NEXUSPOOL_CHRONO_TIMER_MOCK_HPP

#include "gmock/gmock.h"
#include "chrono/timer.hpp"

namespace nexuspool {
namespace chrono {

class Timer_mock : public Timer 
{
public:

    MOCK_METHOD(void, start, (Milliseconds expires_in, Handler handler), (override));
    MOCK_METHOD(void, start, (Seconds expires_in, Handler handler), (override));
    MOCK_METHOD(void, stop, (), (override));
};

}
}

