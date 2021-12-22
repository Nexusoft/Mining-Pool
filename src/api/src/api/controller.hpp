#ifndef NEXUSPOOL_API_CONTROLLER_HPP
#define NEXUSPOOL_API_CONTROLLER_HPP

#include "api/dto.hpp"
#include "api/shared_data_reader.hpp"
#include "common/pool_api_data_exchange.hpp"
#include "TAO/Register/types/address.h"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

namespace nexuspool
{
namespace api
{
using  oatpp::web::server::handler::BasicAuthorizationHandler;
using  oatpp::web::server::handler::DefaultBasicAuthorizationObject;
using  oatpp::web::server::api::ApiController;

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Rest_controller : public oatpp::web::server::api::ApiController 
{
public:

    Rest_controller(Shared_data_reader::Sptr data_reader,
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
        std::string auth_user,
        std::string auth_pw,
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper);

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

private:

    persistance::Config_data get_config_data();
    std::shared_ptr<ApiController::OutgoingResponse> get_meta_infos();
    std::shared_ptr<ApiController::OutgoingResponse> get_latest_blocks();
    std::shared_ptr<ApiController::OutgoingResponse> get_account_header(std::string account);
    std::shared_ptr<ApiController::OutgoingResponse> get_account_detail(std::string account);
    std::shared_ptr<ApiController::OutgoingResponse> get_account_payout(std::string account);

    Shared_data_reader::Sptr m_data_reader;
    common::Pool_api_data_exchange::Sptr m_pool_api_data_exchange;
    std::string m_auth_user;
    std::string m_auth_pw;
    persistance::Config_data m_cached_config;

};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif