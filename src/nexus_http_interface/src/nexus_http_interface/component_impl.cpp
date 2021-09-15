#include "nexus_http_interface/component_impl.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

namespace {
std::shared_ptr<oatpp::web::client::RequestExecutor> createOatppExecutor() 
{
	OATPP_LOGD("App", "Using Oat++ native HttpRequestExecutor.");
	auto connectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared({ "httpbin.org", 80 });
	return oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider);
}


}

namespace nexuspool {
namespace nexus_http_interface {

Component_impl::Component_impl()
{
	/* Create ObjectMapper for serialization of DTOs  */
	auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

	/* Create RequestExecutor which will execute ApiClient's requests */
	auto requestExecutor = createOatppExecutor(); 

	/* ObjectMapper passed here is used for serialization of outgoing DTOs */
	m_client = Api_client::createShared(requestExecutor, objectMapper);
}

}
}
