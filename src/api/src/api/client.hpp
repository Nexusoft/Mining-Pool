#ifndef NEXUSPOOL_API_CLIENT_HPP
#define NEXUSPOOL_API_CLIENT_HPP

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"


namespace nexuspool 
{
namespace api 
{

#include OATPP_CODEGEN_BEGIN(ApiClient)
class NXS_client : public oatpp::web::client::ApiClient
{
	API_CLIENT_INIT(NXS_client)
	API_CALL("GET", "ledger/get/mininginfo", get_mininginfo, AUTHORIZATION_BASIC(String, authString))
	API_CALL("GET", "system/get/info", get_systeminfo, AUTHORIZATION_BASIC(String, authString))
};
#include OATPP_CODEGEN_END(ApiClient)

}
}

#endif
