#ifndef NEXUSPOOL_API_CONTROLLER_HPP
#define NEXUSPOOL_API_CONTROLLER_HPP

#include "api/dto.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

namespace nexuspool
{
namespace api
{

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

/**
    *  EXAMPLE ApiController
    *  Basic examples of howto create ENDPOINTs
    *  More details on oatpp.io
    */
class Rest_controller : public oatpp::web::server::api::ApiController 
{
public:
    /**
     * Constructor with object mapper.
     * @param objectMapper - default object mapper used to serialize/deserialize DTOs.
     */
    Rest_controller(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper)
    {}
    /**
        *  Inject @objectMapper component here as default parameter
        *  Do not return bare Controllable* object! use shared_ptr!
        */
    static std::shared_ptr<Rest_controller> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    {
        return std::shared_ptr<Rest_controller>(new Rest_controller(objectMapper));
    }

    ENDPOINT("GET", "/metainfo", metainfo) {
        auto dto = Meta_infos_dto::createShared();
        dto->pool_hashrate = "Hello World!";
        dto->round_duration = 24;
        dto->fee = 1;
        dto->mining_mode = "Hello World!";
        dto->active_miners = 5;
        return createDtoResponse(Status::CODE_200, dto);
    }
};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif