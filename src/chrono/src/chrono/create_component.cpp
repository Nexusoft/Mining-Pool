#include "chrono/create_component.hpp"
#include "chrono/component_impl.hpp"
#include "asio/io_context.hpp"

namespace nexuspool {
namespace chrono {

Component::Uptr create_component(std::shared_ptr<asio::io_context> io_context)
{
    return std::make_unique<Component_impl>(std::move(io_context));
}

}
}
