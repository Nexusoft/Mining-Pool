#ifndef NEXUSPOOL_PERSISTANCE_CREATE_COMPONENT_HPP
#define NEXUSPOOL_PERSISTANCE_CREATE_COMPONENT_HPP

#include "persistance/component.hpp"
#include "config/config.hpp"

#include <memory>

namespace nexuspool {
namespace persistance {

	// Component factory

	Component::Uptr create_component(config::Config& config);

}
}

#endif 
