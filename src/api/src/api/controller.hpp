#ifndef NEXUSPOOL_API_CONTROLLER_HPP
#define NEXUSPOOL_API_CONTROLLER_HPP

#include "api/dto.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

/**
 *  EXAMPLE ApiController
 *  Basic examples of howto create ENDPOINTs
 *  More details on oatpp.io
 */
class MyController : public oatpp::web::server::api::ApiController {
protected:
    MyController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {}
public:

    /**
     *  Inject @objectMapper component here as default parameter
     *  Do not return bare Controllable* object! use shared_ptr!
     */
    static std::shared_ptr<MyController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>,
        objectMapper)) {
        return std::shared_ptr<MyController>(new MyController(objectMapper));
    }

    ENDPOINT("POST", "demo/api/json", postJson,
        BODY_DTO(Object<MyDto>, dto)) 
    {
        auto dtoMessage = dto->message;
        return createResponse(Status::CODE_200, "dtoMessage: " + dtoMessage);
    }
    /**
     *  Hello World endpoint Coroutine mapped to the "/" (root)
     */
    ENDPOINT_ASYNC("GET", "/", Root) {

        ENDPOINT_ASYNC_INIT(Root)

            /**
             *  Coroutine entrypoint act()
             *  returns Action (what to do next)
             */
            Action act() override {
            auto dto = HelloDto::createShared();
            dto->message = "Hello Async!";
            dto->server = Header::Value::SERVER;
            dto->userAgent = request->getHeader(Header::USER_AGENT);
            return _return(controller->createDtoResponse(Status::CODE_200, dto));
        }

    };

    /**
     *  Echo body endpoint Coroutine. Mapped to "/body/string".
     *  Returns body received in the request
     */
    ENDPOINT_ASYNC("GET", "/body/string", EchoStringBody) {

        ENDPOINT_ASYNC_INIT(EchoStringBody)

            Action act() override {
            /* return Action to start child coroutine to read body */
            return request->readBodyToStringAsync().callbackTo(&EchoStringBody::returnResponse);
        }

        Action returnResponse(const oatpp::String & body) {
            /* return Action to return created OutgoingResponse */
            return _return(controller->createResponse(Status::CODE_200, body));
        }

    };

    /**
     *  Echo body endpoint Coroutine. Mapped to "/body/dto".
     *  Deserialize DTO reveived, and return same DTO
     *  Returns body as MessageDto received in the request
     */
    ENDPOINT_ASYNC("GET", "/body/dto", EchoDtoBody) {

        ENDPOINT_ASYNC_INIT(EchoDtoBody)

            Action act() override {
            return request->readBodyToDtoAsync<oatpp::Object<MessageDto>>(controller->getDefaultObjectMapper()).callbackTo(&EchoDtoBody::returnResponse);
        }

        Action returnResponse(const oatpp::Object<MessageDto>&body) {
            return _return(controller->createDtoResponse(Status::CODE_200, body));
        }

    };

};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

#endif