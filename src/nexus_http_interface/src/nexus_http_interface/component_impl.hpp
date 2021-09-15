#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_IMPL_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_IMPL_HPP

#include "nexus_http_interface/component.hpp"
#include "api_client.hpp"

namespace nexuspool {
namespace nexus_http_interface {

class Component_impl : public Component
{
public:
    Component_impl();

private:

    std::shared_ptr<oatpp::web::client::ApiClient> m_client;

    

};

}
}

#endif
