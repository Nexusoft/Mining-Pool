#ifndef NEXUSPOOL_CHRONO_COMPONENT_IMPL_HPP
#define NEXUSPOOL_CHRONO_COMPONENT_IMPL_HPP

#include "chrono/component.hpp"
#include "chrono/timer_factory_impl.hpp"

namespace nexuspool {
namespace chrono {

class Component_impl : public Component
{
public:

    explicit Component_impl(std::shared_ptr<::asio::io_context> io_context)
        : m_timer_factory{ std::make_shared<Timer_factory_impl>(std::move(io_context)) }
    {
    }

    Timer_factory::Sptr get_timer_factory() override { return m_timer_factory; }

private:

    Timer_factory::Sptr m_timer_factory;
};

}
}

#endif 
