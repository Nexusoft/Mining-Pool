#ifndef NEXUSPOOL_CHRONO_TIMER_IMPL_HPP
#define NEXUSPOOL_CHRONO_TIMER_IMPL_HPP

#include "chrono/timer.hpp"
#include "asio/basic_waitable_timer.hpp"
#include "asio/io_service.hpp"
#include <functional>
#include <memory>

namespace nexuspool {
namespace chrono {

class Timer_impl : public Timer
{
public:

    // According to asio documentation -> if a running timer gets deleted, asio implicitly calls cancel() on that timer
    explicit Timer_impl(std::shared_ptr<asio::io_context> io_context)
        : m_io_context{std::move(io_context)}, m_timer{*m_io_context }
    {
    }

    void start(Milliseconds expires_in, Handler handler)
    {
        start_int(expires_in, std::move(handler));
    }

    void start(Seconds expires_in, Handler handler)
    {
        start_int(expires_in, std::move(handler));
    }

    void cancel() { m_timer.cancel(); }


private:
    std::shared_ptr<asio::io_context> m_io_context;
    asio::basic_waitable_timer<std::chrono::steady_clock> m_timer;

    template<typename T>
    void start_int(T expires_in, Handler&& handler)
    {
        cancel();

        m_timer.expires_after(expires_in);
        m_timer.async_wait([lambda_handler = std::move(handler)](const asio::error_code& error) {
            if (error) {
                // timer was canceled or restartet
                lambda_handler(true);
            }
            else {
                lambda_handler(false);
            }
        });
    }
};


}
}

#endif