#include "api/connection.hpp"
#include "api/parser_impl.hpp"
#include <json/jsonrpcpp.hpp>
#include <json/json.hpp>
#include <spdlog/spdlog.h>

namespace nexuspool
{
namespace api
{
Connection::Connection(std::shared_ptr<spdlog::logger> logger, network::Connection::Sptr&& connection, Parser::Sptr parser)
	: m_connection{ std::move(connection) }
	, m_logger{ std::move(logger)}
	, m_parser{std::move(parser)}
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
			self->m_logger->error("Connection to {} was not successful. Result: {}", self->m_connection->remote_endpoint().to_string(), result);
			self->m_connection.reset();
		}
		else if (result == network::Result::connection_closed)
		{
			self->m_connection.reset();
		}
		else if (result == network::Result::connection_ok)
		{
			self->m_logger->info("Connection to {} established", self->m_connection->remote_endpoint().to_string());
		}
		else
		{	// data received
			self->process_data(std::move(receive_buffer));
		}
	};
}

void Connection::process_data(network::Shared_payload&& receive_buffer)
{
	// check if banned ip

	try
	{
		jsonrpcpp::entity_ptr entity =
			jsonrpcpp::Parser::do_parse_json(nlohmann::json::from_bson(*receive_buffer));

		jsonrpcpp::Response response{};
		auto result = m_parser->parse(entity, response);
		if (!result)
		{
			m_logger->error("Error while parsing api method");
			return;
		}

		std::vector<std::uint8_t> response_buffer{ nlohmann::json::to_bson(response.to_json()) };
		network::Shared_payload payload{ std::make_shared<network::Payload>(response_buffer.begin(), response_buffer.end()) };
		m_connection->transmit(payload);
	}
	catch (jsonrpcpp::ParseErrorException& e)
	{
		m_logger->debug("{} Reason: {}", e.error().message(), e.error().data().dump() );
		// increase ddos
		m_logger->warn("Received invalid jsonrpc request from {}", m_connection->remote_endpoint().to_string());
	}
	catch (std::exception& e)
	{
		m_logger->debug("{}", e.what());
		m_logger->warn("Received invalid jsonrpc request from {}", m_connection->remote_endpoint().to_string());
	}
}

}
}