#include "api/parser_impl.hpp"
#include "api/method_impl.hpp"
#include <json/json.hpp>
#include <spdlog/spdlog.h>

namespace nexuspool
{
namespace api
{

Parser_impl::Parser_impl(std::shared_ptr<spdlog::logger> logger, Methods_factory::Sptr methods_factory)
: m_logger{ std::move(logger) }
, m_methods{ methods_factory->create_api_methods()}
{

}

bool Parser_impl::parse(jsonrpcpp::entity_ptr request, jsonrpcpp::Response& response)
{
	if (request && request->is_request())
	{
		jsonrpcpp::request_ptr request_ptr = std::dynamic_pointer_cast<jsonrpcpp::Request>(request);
		m_logger->debug("Request: {}, id: {}, has params: {} ", request_ptr->method(), request_ptr->id().int_id(), !request_ptr->params().is_null());

		if (!m_methods.count(request_ptr->method()))
		{
			return false;
		}

		Method_params params{};
		if (request_ptr->params().is_array())
		{
			params = request_ptr->params().to_json();
		}
		auto result = m_methods[request_ptr->method()]->execute(params);

		response.setId(request_ptr->id());
		if (result.m_is_error)
		{
			response.setError(result.m_error_message, result.m_error_code, result.m_result);
		}
		else
		{
			response.setResult(result.m_result);
		}

		return true;
	}
	else
	{
		return false;
	}

}


}
}