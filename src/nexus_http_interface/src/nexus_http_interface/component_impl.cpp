#include "nexus_http_interface/component_impl.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

namespace nexuspool {
namespace nexus_http_interface {

Component_impl::Component_impl(std::shared_ptr<spdlog::logger> logger, std::string wallet_ip)
	: m_logger{std::move(logger)}
	, m_wallet_ip{std::move(wallet_ip)}
{
	oatpp::base::Environment::init();
	/* Create ObjectMapper for serialization of DTOs  */
	auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

	/* Create RequestExecutor which will execute ApiClient's requests */
	auto connectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared({ m_wallet_ip.c_str(), 8080 });
	auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider);

	/* ObjectMapper passed here is used for serialization of outgoing DTOs */
	m_client = Api_client::createShared(requestExecutor, objectMapper);
}

bool Component_impl::get_block(std::string hash)
{
	std::string parameter{ "?hash=" };
	parameter += hash;
	auto response = m_client->get_block(parameter.c_str());
	auto const status_code = response->getStatusCode();
	if (status_code != 200)
	{
		return false;
	}

	auto data = response->readBodyToString();
	m_logger->info("[get_block] data={}", data->c_str());
}

}
}