#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_API_CLIENT_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_API_CLIENT_HPP

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"


namespace nexuspool {
namespace nexus_http_interface {

#include OATPP_CODEGEN_BEGIN(DTO)
class Payout_recipient_dto : public oatpp::DTO 
{
	DTO_INIT(Payout_recipient_dto, DTO)

	DTO_FIELD(String, address_to);
	DTO_FIELD(Float64, amount);

public:

	Payout_recipient_dto() = default;
	Payout_recipient_dto(const char* paddress_to, double pamount)
		: address_to(paddress_to)
		, amount(pamount)
	{}

};

class Payout_dto : public oatpp::DTO 
{
	DTO_INIT(Payout_dto, DTO)

	DTO_FIELD(String, pin);
	DTO_FIELD(String, name);
	DTO_FIELD(Vector<Object<Payout_recipient_dto>>, recipients) = {};

};
#include OATPP_CODEGEN_END(DTO)

#include OATPP_CODEGEN_BEGIN(ApiClient)
class Api_client : public oatpp::web::client::ApiClient
{
	API_CLIENT_INIT(Api_client)
	API_CALL("GET", "ledger/get/block{parameter}", get_block, PATH(String, parameter), AUTHORIZATION_BASIC(String, authString))
	API_CALL("GET", "ledger/get/blockhash", get_blockhash, QUERY(UInt32, height), AUTHORIZATION_BASIC(String, authString))
	API_CALL("GET", "ledger/get/mininginfo", get_mininginfo, AUTHORIZATION_BASIC(String, authString))
	API_CALL("GET", "finance/get/account", get_account, QUERY(String, address), AUTHORIZATION_BASIC(String, authString))
	API_CALL("POST", "finance/debit/account", payout, BODY_DTO(Object<Payout_dto>, body), AUTHORIZATION_BASIC(String, authString))
	API_CALL("GET", "system/get/info", get_systeminfo, AUTHORIZATION_BASIC(String, authString))
};
#include OATPP_CODEGEN_END(ApiClient)

}
}

#endif
