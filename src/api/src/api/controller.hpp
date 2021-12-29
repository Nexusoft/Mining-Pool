#ifndef NEXUSPOOL_API_CONTROLLER_HPP
#define NEXUSPOOL_API_CONTROLLER_HPP

#include "api/controller_base.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace nexuspool
{
namespace api
{
using  oatpp::web::server::api::ApiController;

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Rest_controller : public Base_controller
{
public:

    Rest_controller(Shared_data_reader::Sptr data_reader,
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
    : Base_controller(std::move(data_reader), std::move(pool_api_data_exchange), std::move(objectMapper))
    {}

    ENDPOINT("GET", "/metainfo", metainfo)
    {
        return get_meta_infos();
    }

    ENDPOINT("GET", "/latestblocks", latestblocks)
    {
        return get_latest_blocks();
    }

    ENDPOINT("GET", "/account/header", accountheader, QUERY(String, account))
    {
        return get_account_header(account);
    }

    ENDPOINT("GET", "/account/detail", accountdetail, QUERY(String, account))
    {
        return get_account_detail(account);
    }
  
    ENDPOINT("GET", "/account/payout", accountpayout, QUERY(String, account))
    {
        return get_account_payout(account);
    }
};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif