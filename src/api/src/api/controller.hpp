#ifndef NEXUSPOOL_API_CONTROLLER_HPP
#define NEXUSPOOL_API_CONTROLLER_HPP

#include "api/dto.hpp"
#include "api/shared_data_reader.hpp"
#include "common/pool_api_data_exchange.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

namespace nexuspool
{
namespace api
{
#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Rest_controller : public oatpp::web::server::api::ApiController 
{
public:
    /**
     * Constructor with object mapper.
     * @param objectMapper - default object mapper used to serialize/deserialize DTOs.
     */
    Rest_controller(Shared_data_reader::Sptr data_reader, 
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : m_data_reader{std::move(data_reader)}
        , m_pool_api_data_exchange{std::move(pool_api_data_exchange)}
        , oatpp::web::server::api::ApiController(objectMapper)
    {}

    ENDPOINT("GET", "/metainfo", metainfo) 
    {
        auto dto = Meta_infos_dto::createShared();
        auto const config = m_data_reader->get_config();

        dto->pool_hashrate = m_data_reader->get_pool_hashrate();
        dto->round_duration = config.m_round_duration_hours;
        dto->fee = config.m_fee;
        dto->mining_mode = config.m_mining_mode.c_str();
        dto->active_miners = m_pool_api_data_exchange->get_active_miners();
        return createDtoResponse(Status::CODE_200, dto);
    }

private:

    Shared_data_reader::Sptr m_data_reader;
    common::Pool_api_data_exchange::Sptr m_pool_api_data_exchange;

};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif