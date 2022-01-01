#include "api/server.hpp"
#include "api/shared_data_reader.hpp"
#include "api/app_component.hpp"
#include "api/controller.hpp"
#include "api/controller_auth.hpp"
#include "oatpp/network/Server.hpp"
#include <spdlog/spdlog.h>


namespace nexuspool
{
namespace api
{

Server::Server(std::shared_ptr<spdlog::logger> logger, 
	persistance::Data_reader::Uptr data_reader,
	config::Config_api::Sptr config_api,
	common::Pool_api_data_exchange::Sptr pool_api_data_exchange)
	: m_logger{std::move(logger)}
	, m_shared_data_reader{std::make_shared<Shared_data_reader>(std::move(data_reader))}
	, m_config_api{std::move(config_api)}
	, m_pool_api_data_exchange{std::move(pool_api_data_exchange)}
	, m_server_stopped{false}
{
}

void Server::start()
{
	oatpp::base::Environment::init();

	m_server_thread = std::thread([this]()
	{
		App_component components{ m_config_api->get_public_ip(), m_config_api->get_listen_port() }; // Create scope Environment components

		OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
		OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

		/* create ApiControllers and add endpoints to router */
		if (m_config_api->get_auth_user().empty())
		{
			auto rest_controller = std::make_shared<Rest_controller>(m_shared_data_reader, m_pool_api_data_exchange, m_config_api, objectMapper);
			router->addController(rest_controller);
		}
		else
		{
			auto rest_auth_controller = std::make_shared<Rest_auth_controller>(m_shared_data_reader, m_pool_api_data_exchange, m_config_api, objectMapper);
			router->addController(rest_auth_controller);
		}

		/* Get connection handler component */
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

		/* Get connection provider component */
		auto connectionProvider = components.get_serverConnectionProvider();

		oatpp::network::Server server(connectionProvider, connectionHandler);
		m_logger->info("API server started");



		std::function<bool()> condition = [this]() {
			return !m_server_stopped.load();
		};

		server.run(condition);
		/* First, stop the ServerConnectionProvider so we don't accept any new connections */
		connectionProvider->stop();

		/* Finally, stop the ConnectionHandler and wait until all running connections are closed */
		connectionHandler->stop();

	});
}

void Server::stop()
{
	/* Signal the stop condition */
	m_server_stopped.store(true);

	/* Check if the thread has already stopped or if we need to wait for the server to stop */
	if (m_server_thread.joinable()) 
	{
		m_server_thread.join();
	}

	oatpp::base::Environment::destroy();
	m_logger->info("API server stopped");
}

}
}