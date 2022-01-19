#ifndef NEXUSPOOL_API_APP_COMPONENT_HPP
#define NEXUSPOOL_API_APP_COMPONENT_HPP

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp-openssl/server/ConnectionProvider.hpp"
#include "oatpp-openssl/Config.hpp"
#include <string>

namespace nexuspool
{
namespace api
{

class App_component
{
public:

    App_component(std::string public_ip, 
        std::uint16_t api_listen_port,
        std::string ssl_crt_file,
        std::string ssl_pem_file)
        : m_public_ip{ std::move(public_ip) }
        , m_api_listen_port{ api_listen_port }
    {
        auto config = oatpp::openssl::Config::createDefaultServerConfigShared(ssl_pem_file, ssl_crt_file);
        m_serverConnectionProvider = oatpp::openssl::server::ConnectionProvider::createShared(config, { m_public_ip, m_api_listen_port });
    }

    std::shared_ptr<oatpp::openssl::server::ConnectionProvider> get_serverConnectionProvider()
    {
        return m_serverConnectionProvider;
    }

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        objectMapper->getDeserializer()->getConfig()->allowUnknownFields = false;
        return objectMapper;
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
    std::shared_ptr<oatpp::openssl::server::ConnectionProvider> m_serverConnectionProvider;

};

}
}

#endif