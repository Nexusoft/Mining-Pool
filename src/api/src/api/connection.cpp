#include "api/connection.hpp"
#include <json/jsonrpcpp.hpp>
#include <json/json.hpp>

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
	// check if banned ip

	jsonrpcpp::entity_ptr entity =
		jsonrpcpp::Parser::do_parse(nlohmann::json::parse(std::string{ receive_buffer->begin(), receive_buffer->end() }));

	if (entity && entity->is_request())
	{
		jsonrpcpp::request_ptr request = std::dynamic_pointer_cast<jsonrpcpp::Request>(entity);
		m_logger->debug("Request: {}, id: {}, has params: {} ", request->method(), request->id().int_id(), !request->params().is_null() );
		// lookup request
		if (request->method() == "test")
		{
			nlohmann::json result;
			std::vector<std::string> blocks;
			if (request->params().is_array())
			{
				result = request->params().get<int>(0) - request->params().get<int>(1);
				for (auto i = 0U; i < request->params().get<int>(0); i++)
				{
					blocks.push_back("Big block data");
				}
			}
			else
			{

			}

			nlohmann::json block_array(blocks);
			nlohmann::json test;
			test["blocks"] = block_array;
			test["number"] = result;

			jsonrpcpp::Response response(*request, test);
			std::string response_string{ response.to_json().dump() };
			m_logger->debug("Response: {}", response_string);
			network::Shared_payload payload{ std::make_shared<network::Payload>(response_string.begin(), response_string.end()) };
			m_connection->transmit(payload);
		}

	}
	else
	{
		// increase ddos
		m_logger->warn("Received invalid jsonrpc request from {}", m_remote_address);
	}
}

}
}