#include "api/server.hpp"
#include "api/connection.hpp"
#include "api/parser_impl.hpp"
#include "api/method_impl.hpp"
#include "api/shared_data_access.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool
{
namespace api
{

Server::Server(std::shared_ptr<spdlog::logger> logger, 
	persistance::Data_access::Uptr data_access,
	std::string local_ip,
	std::uint16_t api_listen_port, 
	network::Socket_factory::Sptr socket_factory)
	: m_logger{std::move(logger)}
	, m_shared_data_access{std::make_shared<Shared_data_access>(std::move(data_access))}
	, m_local_ip{ std::move(local_ip) }
	, m_api_listen_port{ api_listen_port }
	, m_local_endpoint{ network::Transport_protocol::tcp, m_local_ip, m_api_listen_port }
	, m_socket_factory{ std::move(socket_factory) }
	, m_listen_socket{}
	, m_parser{std::make_shared<Parser_impl>(m_logger, std::make_shared<Methods_factory_impl>(m_logger, m_shared_data_access))}
{
}

void Server::start()
{
	// listen
	m_listen_socket = m_socket_factory->create_socket(m_local_endpoint);


	// on listen/accept, save created connection to pool_conenctions and call the connection_handler of created pool connection object
	auto socket_handler = [this](network::Connection::Sptr&& connection)
	{
		m_api_connections.emplace_back(std::make_shared<api::Connection>(m_logger, std::move(connection), m_parser));

		return m_api_connections.back()->connection_handler();
	};

	m_listen_socket->listen(socket_handler);
	m_logger->info("API server started");

}

void Server::stop()
{
	m_listen_socket->stop_listen();
	m_api_connections.clear();
	m_logger->info("API server stopped");
}



}
}