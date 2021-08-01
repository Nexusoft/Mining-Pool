#ifndef NEXUSPOOL_PERSISTANCE_CREATE_COMPONENT_HPP
#define NEXUSPOOL_PERSISTANCE_CREATE_COMPONENT_HPP

#include "persistance/component.hpp"
#include "config/types.hpp"
#include <memory>

namespace spdlog { class logger; }
namespace nexuspool {
namespace persistance {

// Component factory

Component::Uptr create_component(std::shared_ptr<spdlog::logger> logger, config::Persistance_config config);

}
}

#endif 
