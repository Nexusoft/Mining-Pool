#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_IMPL_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_IMPL_HPP

#include "nexus_http_interface/component.hpp"
#include "nexus_http_interface/api_client.hpp"
#include <spdlog/spdlog.h>
#include <string>

namespace nexuspool {
namespace nexus_http_interface {

class Component_impl : public Component
{
public:
    Component_impl(std::shared_ptr<spdlog::logger> logger, std::string wallet_ip);

    bool get_block(std::string hash) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    std::string m_wallet_ip;
    std::shared_ptr<Api_client> m_client;
    

};

}
}

#endif
