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
	// check if banned ip

	try
	{
		jsonrpcpp::entity_ptr entity =
			jsonrpcpp::Parser::do_parse(nlohmann::json::parse(std::string{ receive_buffer->begin(), receive_buffer->end() }));

		jsonrpcpp::Response response{};
		auto result = m_parser->parse(entity, response);
		if (!result)
		{
			m_logger->error("Error while parsing api method");
			return;
		}

		std::string response_string{ response.to_json().dump() };
		m_logger->debug("Response: {}", response_string);

		network::Shared_payload payload{ std::make_shared<network::Payload>(response_string.begin(), response_string.end()) };
		m_connection->transmit(payload);
	}
	catch (jsonrpcpp::ParseErrorException& e)
	{
		m_logger->debug("{} Reason: {}", e.error().message(), e.error().data().dump() );
		// increase ddos
		m_logger->warn("Received invalid jsonrpc request from {}", m_remote_address);
	}
}

}
}