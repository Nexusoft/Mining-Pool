#ifndef NEXUSPOOL_API_CONTROLLER_MINING_CALC_HPP
#define NEXUSPOOL_API_CONTROLLER_MINING_CALC_HPP

#include "common/types.hpp"
#include "config/config_api.hpp"
#include "chrono/timer_factory.hpp"
#include "chrono/timer.hpp"
#include "api/controller/dto.hpp"
#include "api/client.hpp"

#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include <json/json.hpp>
#include <string>
#include <mutex>

namespace nexuspool
{
namespace api
{
using  oatpp::web::server::handler::BasicAuthorizationHandler;
using  oatpp::web::server::handler::DefaultBasicAuthorizationObject;
using  oatpp::web::server::api::ApiController;

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Controller_mining_calc : public ApiController
{
public:

    Controller_mining_calc(std::shared_ptr<NXS_client> nxs_client,
        chrono::Timer_factory::Sptr timer_factory,
        Shared_data_reader::Sptr data_reader,
        config::Config_api::Sptr config_api,
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
        : ApiController(objectMapper)
        , m_config_api{std::move(config_api)}
        , m_auth_user{ m_config_api->get_auth_user() }
        , m_auth_pw{ m_config_api->get_auth_pw() }
        , m_auth_nxs_string{ m_config_api->get_nxs_api_user() + ":" + m_config_api->get_nxs_api_pw() }
        , m_timer_factory{ std::move(timer_factory)}
        , m_nxs_client{std::move(nxs_client)}
    {
        setDefaultAuthorizationHandler(std::make_shared<BasicAuthorizationHandler>("nexuspool"));
        m_mining_info_timer = m_timer_factory->create_timer();
        m_mining_info_timer->start(chrono::Seconds(m_config_api->get_reward_calc_update_interval()), mining_info_handler(m_config_api->get_reward_calc_update_interval()));
    }

    ~Controller_mining_calc()
    {
        m_mining_info_timer->stop();
    }

    ENDPOINT("GET", "/reward_data", rewarddata, AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");

        auto mining_info = m_cached_mining_info;
        if (!mining_info.is_valid())
        {
            return createResponse(Status::CODE_404, "get_mininginfo error");
        }

        auto dto = Reward_data_dto::createShared();
        if (m_config_api->get_mining_mode() == common::Mining_mode::HASH)
        {
            dto->block_reward = mining_info.m_hash_rewards;
            dto->network_diff = mining_info.m_hash_difficulty;
        }
        else
        {
            dto->block_reward = mining_info.m_prime_reward;
            dto->network_diff = mining_info.m_prime_difficulty;
        }

        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT("GET", "/hardware_data", hardwaredata, AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject))
    {
        OATPP_ASSERT_HTTP(authObject->userId == m_auth_user && authObject->password == m_auth_pw, Status::CODE_401, "Unauthorized");

        auto dto = Hardware_data_dto::createShared();
        auto const devices = m_config_api->get_devices();
        for (auto const& device : devices)
        {
            dto->devices->push_back(Hardware_dto::createShared(device.m_model.c_str(), device.m_hashrate, device.m_power_consumption));
        }

        return createDtoResponse(Status::CODE_200, dto);
    }

private:

    chrono::Timer::Handler mining_info_handler(std::uint16_t mining_info_interval)
    {
        return[this, mining_info_interval]()
        {
            auto response = m_nxs_client->get_mininginfo(m_auth_nxs_string);
            auto const status_code = response->getStatusCode();
            {
                std::scoped_lock lock(m_mining_info_mutex);

                if (status_code != 200)
                {
                    m_cached_mining_info = common::Mining_info{};
                }
                else
                {
                    auto data_json = nlohmann::json::parse(response->readBodyToString()->c_str());
                    m_cached_mining_info.m_height = data_json["result"]["blocks"];
                    m_cached_mining_info.m_hash_rewards = data_json["result"]["hashValue"];
                    m_cached_mining_info.m_hash_difficulty = data_json["result"]["hashDifficulty"];
                    m_cached_mining_info.m_prime_reward = data_json["result"]["primeValue"];
                    m_cached_mining_info.m_prime_difficulty = data_json["result"]["primeDifficulty"];
                }
            }

            // restart timer
            m_mining_info_timer->start(chrono::Seconds(mining_info_interval), mining_info_handler(mining_info_interval));
        };
    }

    config::Config_api::Sptr m_config_api;
    std::string m_auth_user;
    std::string m_auth_pw;
    std::string m_auth_nxs_string;
    chrono::Timer_factory::Sptr m_timer_factory;
    std::shared_ptr<NXS_client> m_nxs_client;
    common::Mining_info m_cached_mining_info;
    chrono::Timer::Uptr m_mining_info_timer;
    std::mutex m_mining_info_mutex;
};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif