#include "nexus_http_interface/component_impl.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include <json/json.hpp>

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

bool Component_impl::get_block_reward_data(std::string hash, common::Block_reward_data& reward_data)
{
	std::string parameter{ "?verbose=summary&hash=" };
	parameter += hash;
	auto response = m_client->get_block(parameter.c_str());
	auto const status_code = response->getStatusCode();
	if (status_code != 200)
	{
		return false;
	}

	auto data_json = nlohmann::json::parse(response->readBodyToString()->c_str() );

	// TODO: error handling

	reward_data.m_reward = data_json["result"]["mint"];
	reward_data.m_timestamp = data_json["result"]["time"];
	reward_data.m_tx_confirmations = data_json["result"]["tx"][0]["confirmations"];
	reward_data.m_tx_type = data_json["result"]["tx"][0]["contracts"][0]["OP"];
}

bool Component_impl::payout(Payout_recipients const& recipients)
{
	/*
	"pin": "1234",
		"name" : "default"
		"recipients" :
		[
	{
		"name_to": "bob:savings",
			"amount" : 5,
			"reference" : 1234
	},
		{
			"address_to": "8CHjS5Qe7Mghgrqb6NeEaVxjexbdp9p2QVdRTt8W4rzbWhu3fL8",
			"amount" : 5,
			"reference" : 5678
		},
		{
			"address_to": "4kYv4Xft6wufMVi191nQDNprgkK6kzy6gqWRpmfMxyx9KMfEV1u",
			"amount" : 5
		}
		]
		*/

	std::string parameter{ "?verbose=summary&hash=" };
	auto response = m_client->payout(parameter.c_str());
	auto const status_code = response->getStatusCode();
	if (status_code != 200)
	{
		return false;
	}

	auto data_json = nlohmann::json::parse(response->readBodyToString()->c_str());
	std::string const tx_id = data_json["result"]["txid"];

	m_logger->info("Successfully paid all miners. Tx_id: {}", tx_id);

	return true;
}

}
}