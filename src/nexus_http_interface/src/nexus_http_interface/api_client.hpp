#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_API_CLIENT_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_API_CLIENT_HPP

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"


namespace nexuspool {
namespace nexus_http_interface {

#include OATPP_CODEGEN_BEGIN(ApiClient)
class Api_client : public oatpp::web::client::ApiClient
{
API_CLIENT_INIT(Api_client)
API_CALL("GET", "ledger/get/block{parameter}", get_block, PATH(String, parameter))
API_CALL("GET", "finance/debit/account{parameter}", payout, PATH(String, parameter))



};
#include OATPP_CODEGEN_END(ApiClient)

}
}

#endif
