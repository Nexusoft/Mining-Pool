#ifndef NEXUSPOOL_CHRONO_TIMER_FACTORY_IMPL_HPP
#define NEXUSPOOL_CHRONO_TIMER_FACTORY_IMPL_HPP

#include "chrono/timer_factory.hpp"
#include "chrono/timer_impl.hpp"
#include "asio/io_service.hpp"
#include <memory>

namespace nexuspool {
namespace chrono {

class Timer_factory_impl : public Timer_factory
{
public:

    explicit Timer_factory_impl(std::shared_ptr<asio::io_context> io_context)
        : m_io_context{std::move(io_context)}
    {
    }

    Timer::Uptr create_timer() override { return std::make_unique<Timer_impl>(m_io_context); }

private:
    std::shared_ptr<asio::io_context> m_io_context;
};


}
}

#endif 
