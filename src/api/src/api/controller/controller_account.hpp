#ifndef NEXUSPOOL_API_CONTROLLER_ACCOUNT_HPP
#define NEXUSPOOL_API_CONTROLLER_ACCOUNT_HPP

#include "api/controller/dto.hpp"
#include "config/config_api.hpp"
#include "api/shared_data_reader.hpp"
#include "TAO/Register/types/address.h"

#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"


namespace nexuspool
{
namespace api
{
using  oatpp::web::server::handler::BasicAuthorizationHandler;
using  oatpp::web::server::handler::DefaultBasicAuthorizationObject;
using  oatpp::web::server::api::ApiController;

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Controller_account : public ApiController
{
public:

    Controller_account(Shared_data_reader::Sptr data_reader,
        config::Config_api::Sptr config_api,
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
        : ApiController(objectMapper)
        , m_auth_user{ config_api->get_auth_user() }
        , m_auth_pw{ config_api->get_auth_pw() }
        , m_data_reader{ std::move(data_reader) }
    {
        setDefaultAuthorizationHandler(std::make_shared<BasicAuthorizationHandler>("nexuspool"));
    }

    ENDPOINT("GET", "/account/detail", accountdetail, QUERY(String, account), AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
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
            auto dto = Account_dto::createShared();
            auto const account_data = m_data_reader->get_account(account);

            dto->account = account_data.m_address;
            dto->created_at = account_data.m_created_at;
            dto->last_active = account_data.m_last_active;
            dto->shares = account_data.m_shares;
            dto->hashrate = account_data.m_hashrate;
            dto->display_name = account_data.m_display_name;

            return createDtoResponse(Status::CODE_200, dto);
        }
        else
        {
            return createResponse(Status::CODE_404, "account doesn't exist");
        }
    }

    ENDPOINT("GET", "/account/payout", accountpayout, QUERY(String, account), AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
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
            auto dto = Account_payouts_dto::createShared();
            auto const account_data = m_data_reader->get_account(account);

            auto const payments = m_data_reader->get_payments(account);
            for (auto const& payment : payments)
            {
                dto->payouts->push_back(Payout_dto::createShared(payment.m_payment_date_time.c_str(), payment.m_amount, payment.m_shares, payment.m_tx_id.c_str()));
            }

            return createDtoResponse(Status::CODE_200, dto);
        }
        else
        {
            return createResponse(Status::CODE_404, "account doesn't exist");
        }
    }

private:

    std::string m_auth_user;
    std::string m_auth_pw;
    Shared_data_reader::Sptr m_data_reader;
};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif