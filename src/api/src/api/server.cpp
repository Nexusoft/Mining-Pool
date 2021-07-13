#include "api/server.hpp"
#include "api/connection.hpp"
#include "config/config.hpp"

namespace nexuspool
{
namespace api
{

Server::Server(config::Config& config, network::Socket_factory::Sptr socket_factory)
	: m_config{ config }
	, m_socket_factory{ std::move(socket_factory) }
	, m_logger{ spdlog::get("logger") }
	, m_listen_socket{}
{
}

void Server::start()
{
	// listen
	network::Endpoint api_listen_endpoint{ network::Transport_protocol::tcp, m_config.get_local_ip(), m_config.get_api_listen_port() };
	m_listen_socket = m_socket_factory->create_socket(api_listen_endpoint);


	// on listen/accept, save created connection to pool_conenctions and call the connection_handler of created pool connection object
	auto socket_handler = [this](network::Connection::Sptr&& connection)
	{
		m_api_connections.emplace_back(std::make_shared<api::Connection>(std::move(connection)));

		return m_api_connections.back()->connection_handler();
	};

	m_listen_socket->listen(socket_handler);

}

void Server::stop()
{
	m_listen_socket->stop_listen();
	m_api_connections.clear();
}



}
}