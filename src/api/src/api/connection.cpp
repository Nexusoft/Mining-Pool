#include "api/connection.hpp"

namespace nexuspool
{
namespace api
{
Connection::Connection(network::Connection::Sptr&& connection)
	: m_connection{ std::move(connection) }
	, m_logger{ spdlog::get("logger") }
	, m_remote_address{ "" }
{
}

void Connection::stop()
{
	m_connection->close();
}

network::Connection::Handler Connection::connection_handler()
{
	return[self = shared_from_this()](network::Result::Code result, network::Shared_payload&& receive_buffer)
	{
		if (result == network::Result::connection_declined ||
			result == network::Result::connection_aborted ||
			result == network::Result::connection_error)
		{
			self->m_logger->error("Connection to {0} was not successful. Result: {1}", self->m_remote_address, result);
			self->m_connection.reset();
		}
		else if (result == network::Result::connection_closed)
		{
			self->m_connection.reset();
		}
		else if (result == network::Result::connection_ok)
		{
			self->m_connection->remote_endpoint().address(self->m_remote_address);
			self->m_logger->info("Connection to {} established", self->m_remote_address);
		}
		else
		{	// data received
			self->process_data(std::move(receive_buffer));
		}
	};
}

void Connection::process_data(network::Shared_payload&& receive_buffer)
{
}

}
}