#ifndef NEXUSPOOL_API_CONTROLLER_AUTH_HPP
#define NEXUSPOOL_API_CONTROLLER_AUTH_HPP

#include "api/controller/controller_base.hpp"

#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace nexuspool
{
namespace api
{
using  oatpp::web::server::handler::BasicAuthorizationHandler;
using  oatpp::web::server::handler::DefaultBasicAuthorizationObject;
using  oatpp::web::server::api::ApiController;

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Rest_auth_controller : public Base_controller
{
public:

    Rest_auth_controller(chrono::Timer_factory::Sptr timer_factory,
        Shared_data_reader::Sptr data_reader,
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
        config::Config_api::Sptr config_api,
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
    : Base_controller(std::move(timer_factory), std::move(data_reader), std::move(pool_api_data_exchange), config_api, std::move(objectMapper))
    , m_auth_user{ config_api->get_auth_user() }
    , m_auth_pw{ config_api->get_auth_pw() }
    {
        setDefaultAuthorizationHandler(std::make_shared<BasicAuthorizationHandler>("nexuspool"));
    }

    ENDPOINT("GET", "/metainfo", metainfo, AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");
        return get_meta_infos();
    }

    ENDPOINT("GET", "/latestblocks", latestblocks, AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");
        return get_latest_blocks();
    }

    ENDPOINT("GET", "/account/header", accountheader, QUERY(String, account), AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");
        return get_account_header(account);
    }

    ENDPOINT("GET", "/account/detail", accountdetail, QUERY(String, account), AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");
        return get_account_detail(account);
    }

    ENDPOINT("GET", "/account/payout", accountpayout, QUERY(String, account), AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");
        return get_account_payout(account);
    }

    ENDPOINT("GET", "/reward_data", rewarddata, AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");
        return get_reward_data();
    }

    ENDPOINT("GET", "/hardware_data", hardwaredata, AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");
        return get_hardware_data();
    }

private:

    std::string m_auth_user;
    std::string m_auth_pw;
};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif