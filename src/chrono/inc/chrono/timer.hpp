#ifndef NEXUSPOOL_CHRONO_TIMER_HPP
#define NEXUSPOOL_CHRONO_TIMER_HPP

#include <functional>
#include <memory>
#include <chrono>

namespace nexuspool {
namespace chrono {

using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;

class Timer
{
public:

    using Uptr = std::unique_ptr <Timer>;

    // Called when the asynchronous timer expires
    using Handler = std::function<void()>;

    virtual void start(Milliseconds expires_in, Handler handler) = 0;

    virtual void start(Seconds expires_in, Handler handler) = 0;

    virtual void stop() = 0;
};

}
}

#endif