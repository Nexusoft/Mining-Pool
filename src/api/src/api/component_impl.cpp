#include "api/component_impl.hpp"
#include "api/shared_data_reader.hpp"
#include "api/app_component.hpp"
#include "api/client.hpp"
#include "api/controller/controller_overview.hpp"
#include "api/controller/controller_mining_calc.hpp"
#include "api/controller/controller_account.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include <spdlog/spdlog.h>
#include <asio/io_context.hpp>

namespace nexuspool
{
namespace api
{

Component_impl::Component_impl(std::shared_ptr<spdlog::logger> logger,
	persistance::Data_reader::Uptr data_reader,
	config::Config_api::Sptr config_api,
	common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
	chrono::Timer_factory::Sptr timer_factory)
	: m_logger{ std::move(logger) }
	, m_shared_data_reader{ std::make_shared<Shared_data_reader>(std::move(data_reader)) }
	, m_config_api{ std::move(config_api) }
	, m_pool_api_data_exchange{ std::move(pool_api_data_exchange) }
	, m_timer_factory{ std::move(timer_factory) }
	, m_server_stopped{ false }
{
}

void Component_impl::start()
{
	oatpp::base::Environment::init();

	m_server_thread = std::thread([this]()
		{
			App_component components{ m_config_api->get_public_ip(), m_config_api->get_listen_port() }; // Create scope Environment components

			OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
			OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

			/* Create RequestExecutor which will execute ApiClient's requests */
			auto connectionProvider_client = oatpp::network::tcp::client::ConnectionProvider::createShared({ m_config_api->get_wallet_ip(), 8080 });
			auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider_client);

			/* ObjectMapper passed here is used for serialization of outgoing DTOs */
			m_nxs_client = NXS_client::createShared(requestExecutor, objectMapper);

			/* create ApiControllers and add endpoints to router */
			router->addController(std::make_shared<Controller_overview>(m_nxs_client, m_shared_data_reader, m_pool_api_data_exchange, m_config_api, objectMapper));
			router->addController(std::make_shared<Controller_mining_calc>(m_nxs_client, m_timer_factory, m_shared_data_reader, m_pool_api_data_exchange, m_config_api, objectMapper));
			router->addController(std::make_shared<Controller_account>(m_shared_data_reader, m_config_api, objectMapper));

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

void Component_impl::stop()
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