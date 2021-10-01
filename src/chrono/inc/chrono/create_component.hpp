#ifndef NEXUSPOOL_CHRONO_CREATE_COMPONENT_HPP
#define NEXUSPOOL_CHRONO_CREATE_COMPONENT_HPP

#include <memory>
#include "chrono/component.hpp"

namespace asio { class io_context; }

namespace nexuspool {
namespace chrono {

Component::Uptr create_component(std::shared_ptr<asio::io_context> io_context);

}
}

#endif 
