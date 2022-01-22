#ifndef NEXUSPOOL_API_CONTROLLER_STATISTICS_HPP
#define NEXUSPOOL_API_CONTROLLER_STATISTICS_HPP

#include "api/controller/dto.hpp"
#include "config/config_api.hpp"
#include "api/shared_data_reader.hpp"

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

class Controller_statistics : public ApiController
{
public:

    Controller_statistics(Shared_data_reader::Sptr data_reader,
        config::Config_api::Sptr config_api,
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
        : ApiController(objectMapper)
        , m_auth_user{ config_api->get_auth_user() }
        , m_auth_pw{ config_api->get_auth_pw() }
        , m_data_reader{ std::move(data_reader) }
    {
        setDefaultAuthorizationHandler(std::make_shared<BasicAuthorizationHandler>("nexuspool"));
    }

    ENDPOINT("GET", "/statistics/longestchain", longestchain, AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");

        auto dto = Statistics_longest_chain_dto::createShared();
        auto const longest_chain_finder = m_data_reader->get_longest_chain_finder();

        dto->display_name = longest_chain_finder.m_display_name;
        dto->chain = longest_chain_finder.m_difficulty;

        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT("GET", "/statistics/blockfinders", blockfinders, QUERY(UInt32, num), AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");

        auto dto = Statistics_top_block_finders_dto::createShared();
        auto const block_finders = m_data_reader->get_top_block_finders(num);

        for (auto const& block_finder : block_finders)
        {
            dto->block_finders->push_back(Statistics_block_finder_dto::createShared(block_finder.m_display_name.c_str(), block_finder.m_num_blocks));
        }

        return createDtoResponse(Status::CODE_200, dto);
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