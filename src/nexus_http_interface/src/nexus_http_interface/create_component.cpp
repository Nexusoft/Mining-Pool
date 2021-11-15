#include "nexus_http_interface/create_component.hpp"
#include "nexus_http_interface/component_impl.hpp"

namespace nexuspool {
namespace nexus_http_interface {

Component::Sptr create_component(std::shared_ptr<spdlog::logger> logger, std::string wallet_ip)
{
    return std::make_shared<Component_impl>(std::move(logger), std::move(wallet_ip));
}

}
}
