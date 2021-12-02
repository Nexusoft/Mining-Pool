#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_CREATE_COMPONENT_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_CREATE_COMPONENT_HPP

#include "nexus_http_interface/component.hpp"
#include <memory>
#include <string>

namespace spdlog { class logger; }
namespace nexuspool {
namespace nexus_http_interface {

// Component factory

Component::Sptr create_component(std::shared_ptr<spdlog::logger> logger, std::string wallet_ip);

}
}

#endif 
