#ifndef NEXUSPOOL_API_APP_COMPONENT_HPP
#define NEXUSPOOL_API_APP_COMPONENT_HPP

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"
#include <string>

namespace nexuspool
{
namespace api
{

class App_component
{
public:

    App_component(std::string public_ip, std::uint16_t api_listen_port)
        : m_public_ip{ std::move(m_public_ip) }
        , m_api_listen_port{ api_listen_port }
    {}

    /**
        *  Create ConnectionProvider component which listens on the port
        */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {
        return oatpp::network::tcp::server::ConnectionProvider::createShared({ m_public_ip.c_str(), m_api_listen_port, oatpp::network::Address::IP_4 });
        }());

    /**
        *  Create Router component
        */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
        return oatpp::web::server::HttpRouter::createShared();
        }());

    /**
        *  Create ConnectionHandler component which uses Router component to route requests
        */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
        return oatpp::web::server::HttpConnectionHandler::createShared(router);
        }());

private:

    std::string m_public_ip;
    std::uint16_t m_api_listen_port;

};

}
}

#endif