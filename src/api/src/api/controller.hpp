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
class MyController : public oatpp::web::server::api::ApiController 
{
public:
    /**
     * Constructor with object mapper.
     * @param objectMapper - default object mapper used to serialize/deserialize DTOs.
     */
    MyController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper)
    {}
    /**
        *  Inject @objectMapper component here as default parameter
        *  Do not return bare Controllable* object! use shared_ptr!
        */
    static std::shared_ptr<MyController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>,
        objectMapper)) {
        return std::shared_ptr<MyController>(new MyController(objectMapper));
    }

    ENDPOINT("POST", "demo/api/json", postJson, BODY_DTO(Object<Meta_infos_dto>, dto))
    {
        auto dtoMessage = dto->pool_hashrate;
        return createResponse(Status::CODE_200, "dtoMessage: " + dtoMessage);
    }

};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif