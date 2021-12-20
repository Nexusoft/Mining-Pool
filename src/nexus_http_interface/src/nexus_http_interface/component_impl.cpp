#include "nexus_http_interface/component_impl.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"
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
		m_logger->error("API error. Code: {} Message: {}", status_code, response->readBodyToString()->c_str());
		return false;
	}

	auto data_json = nlohmann::json::parse(response->readBodyToString()->c_str() );

	// TODO: error handling

	reward_data.m_reward = data_json["result"]["mint"];
	reward_data.m_timestamp = data_json["result"]["time"];

	auto txs = nlohmann::json::array();
	txs = data_json["result"]["tx"];
	for (auto& tx : txs)
	{
		if (tx["type"] == "legacy user")
		{
			continue;
		}

		if (tx["type"] == "tritium base")
		{
			reward_data.m_tx_confirmations = tx["confirmations"];
			auto contracts = nlohmann::json::array();
			contracts = tx["contracts"];
			reward_data.m_tx_type = contracts.front()["OP"];
			break;
		}
	}

	return true;
}

bool Component_impl::get_block_hash(std::uint32_t height, std::string& hash)
{
	std::string parameter{ "?height=" };
	parameter += std::to_string(height);
	auto response = m_client->get_blockhash(parameter.c_str());
	auto const status_code = response->getStatusCode();
	if (status_code != 200)
	{
		m_logger->error("API error. Code: {} Message: {}", status_code, response->readBodyToString()->c_str());
		return false;
	}

	auto data_json = nlohmann::json::parse(response->readBodyToString()->c_str());
	hash = data_json["result"]["hash"];

	return true;
}

bool Component_impl::does_account_exists(std::string const& account)
{
	std::string parameter{ "?address=" };
	parameter += account;
	auto response = m_client->get_account(parameter.c_str());
	auto const status_code = response->getStatusCode();
	if (status_code != 200)
	{
		m_logger->error("API error. Code: {} Message: {}", status_code, response->readBodyToString()->c_str());
		return false;
	}

	return true;
}

bool Component_impl::payout(std::string account_from, std::string pin, Payout_recipients const& recipients, std::string& tx_id)
{
	auto dto = Payout_dto::createShared();
	dto->pin = oatpp::String(pin.c_str());
	dto->name = oatpp::String(account_from.c_str());
	for (auto& recipient : recipients)
	{
		dto->recipients->push_back(Payout_recipient_dto::createShared(recipient.m_address.c_str(), recipient.m_reward));
	}

	auto response = m_client->payout(dto);
	auto const status_code = response->getStatusCode();
	if (status_code != 200)
	{
		m_logger->error("API error. Code: {} Message: {}", status_code, response->readBodyToString()->c_str());
		return false;
	}

	auto data_json = nlohmann::json::parse(response->readBodyToString()->c_str());
	tx_id = data_json["result"]["txid"];

	m_logger->info("Successfully paid all miners. Tx_id: {}", tx_id);

	return true;
}

}
}