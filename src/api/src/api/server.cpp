#include "api/server.hpp"
#include "api/shared_data_reader.hpp"
#include "api/app_component.hpp"
#include "api/controller.hpp"
#include "oatpp/network/Server.hpp"
#include <spdlog/spdlog.h>


namespace nexuspool
{
namespace api
{

Server::Server(std::shared_ptr<spdlog::logger> logger, 
	persistance::Data_reader::Uptr data_reader,
	std::string public_ip,
	std::uint16_t api_listen_port, 
	common::Pool_api_data_exchange::Sptr pool_api_data_exchange)
	: m_logger{std::move(logger)}
	, m_shared_data_reader{std::make_shared<Shared_data_reader>(std::move(data_reader))}
	, m_public_ip{ std::move(public_ip) }
	, m_api_listen_port{ api_listen_port }
	, m_local_endpoint{ network::Transport_protocol::tcp, m_public_ip, m_api_listen_port }
	, m_pool_api_data_exchange{std::move(pool_api_data_exchange)}
{
}

void Server::start()
{
	oatpp::base::Environment::init();
	m_server_thread = std::thread([this]() {
		App_component components{ m_public_ip, m_api_listen_port }; // Create scope Environment components

		OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

		/* create ApiControllers and add endpoints to router */
		auto rest_controller = std::make_shared<Rest_controller>(m_shared_data_reader, m_pool_api_data_exchange);
		router->addController(rest_controller);

		/* Get connection handler component */
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

		/* Get connection provider component */
		auto connectionProvider = components.get_serverConnectionProvider();

		oatpp::network::Server server(connectionProvider, connectionHandler);
		m_logger->info("API server started");
		server.run();
	});
}

void Server::stop()
{
	m_server_thread.join();
	oatpp::base::Environment::destroy();
	m_logger->info("API server stopped");
}



}
}