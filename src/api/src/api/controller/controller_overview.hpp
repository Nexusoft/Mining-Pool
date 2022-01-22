#ifndef NEXUSPOOL_API_CONTROLLER_OVERVIEW_HPP
#define NEXUSPOOL_API_CONTROLLER_OVERVIEW_HPP

#include "config/config_api.hpp"
#include "api/shared_data_reader.hpp"
#include "api/controller/dto.hpp"
#include "api/client.hpp"
#include "common/pool_api_data_exchange.hpp"
#include "TAO/Register/types/address.h"

#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include <json/json.hpp>
#include <string>

namespace nexuspool
{
namespace api
{
using  oatpp::web::server::handler::BasicAuthorizationHandler;
using  oatpp::web::server::handler::DefaultBasicAuthorizationObject;
using  oatpp::web::server::api::ApiController;

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Controller_overview : public ApiController
{
public:

    Controller_overview(std::shared_ptr<NXS_client> nxs_client, 
        Shared_data_reader::Sptr data_reader,
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
        config::Config_api::Sptr config_api,
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
    : ApiController(objectMapper)
    , m_auth_user{ config_api->get_auth_user() }
    , m_auth_pw{ config_api->get_auth_pw() }
    , m_data_reader{ std::move(data_reader) }
    , m_pool_api_data_exchange{ std::move(pool_api_data_exchange) }
    , m_auth_nxs_string{ config_api->get_nxs_api_user() + ":" + config_api->get_nxs_api_pw() }
    , m_nxs_client{std::move(nxs_client)}
    {
        setDefaultAuthorizationHandler(std::make_shared<BasicAuthorizationHandler>("nexuspool"));
    }

    ENDPOINT("GET", "/metainfo", metainfo, AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");

        auto dto = Meta_infos_dto::createShared();
        auto const config = get_config_data();

        dto->pool_hashrate = m_data_reader->get_pool_hashrate();
        dto->round_duration = config.m_round_duration_hours;
        dto->fee = config.m_fee;
        dto->mining_mode = config.m_mining_mode.c_str();
        dto->active_miners = m_pool_api_data_exchange->get_active_miners();
        dto->payout_time = m_pool_api_data_exchange->get_payout_time();
        auto const system_info = get_system_info();
        dto->wallet_version = system_info.m_wallet_version;
        dto->pool_version = system_info.m_pool_version;

        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT("GET", "/latestblocks", latestblocks, AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");

        auto dto = Latest_blocks_dto::createShared();
        auto blocks = m_data_reader->get_latest_blocks();
        for (auto& block : blocks)
        {
            dto->blocks->push_back(Block_dto::createShared(block.m_height, block.m_hash.c_str(), block.m_mainnet_reward, block.m_block_found_time.c_str(), block.m_difficulty));
        }

        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT("GET", "/account/header", accountheader, QUERY(String, account), AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");

        // is account a valid nxs address  
        TAO::Register::Address address_check{ account };
        if (!address_check.IsValid())
        {
            return createResponse(Status::CODE_400, "invalid account");
        }

        if (m_data_reader->does_account_exists(account))
        {
            return createResponse(Status::CODE_200, "");
        }
        else
        {
            return createResponse(Status::CODE_404, "account doesn't exist");
        }
    }

private:

    persistance::Config_data get_config_data()
    {
        if (m_pool_api_data_exchange->is_config_updated())
        {
            m_cached_config = m_data_reader->get_config();
            m_pool_api_data_exchange->set_config_updated(false);
        }
        return m_cached_config;
    }

    common::System_info get_system_info() const
    {
        auto response = m_nxs_client->get_systeminfo(m_auth_nxs_string);
        auto const status_code = response->getStatusCode();
        if (status_code != 200)
        {
            return common::System_info{};
        }

        auto data_json = nlohmann::json::parse(response->readBodyToString()->c_str());
        common::System_info system_info;
        system_info.m_wallet_version = data_json["result"]["version"];
        system_info.m_pool_version = "0.9";

        return system_info;
    }


    std::string m_auth_user;
    std::string m_auth_pw;
    Shared_data_reader::Sptr m_data_reader;
    common::Pool_api_data_exchange::Sptr m_pool_api_data_exchange;
    persistance::Config_data m_cached_config;
    std::string m_auth_nxs_string;
    std::shared_ptr<NXS_client> m_nxs_client;
};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif