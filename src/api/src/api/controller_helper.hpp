#ifndef NEXUSPOOL_API_CONTROLLER_HELPER_HPP
#define NEXUSPOOL_API_CONTROLLER_HELPER_HPP

#include "api/shared_data_reader.hpp"
#include "chrono/create_component.hpp"
#include "common/pool_api_data_exchange.hpp"
#include "common/types.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include <string>
#include <json/json.hpp>

namespace nexuspool
{
namespace api
{


class Controller_helper
{
public:

    Controller_helper(chrono::Timer_factory::Sptr timer_factory,
        Shared_data_reader::Sptr data_reader,
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
        std::string const& wallet_ip,
        std::uint16_t reward_calc_update_interval,
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
        : m_timer_factory{std::move(timer_factory)}
        , m_data_reader{std::move(data_reader)}
        , m_pool_api_data_exchange{ std::move(pool_api_data_exchange) }
    {
        /* Create RequestExecutor which will execute ApiClient's requests */
        auto connectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared({ wallet_ip.c_str(), 8080 });
        auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider);

        /* ObjectMapper passed here is used for serialization of outgoing DTOs */
        m_client = Client::createShared(requestExecutor, objectMapper);
        m_mining_info_timer = m_timer_factory->create_timer();
        m_mining_info_timer->start(chrono::Seconds(reward_calc_update_interval), mining_info_handler(reward_calc_update_interval));
    }

    ~Controller_helper()
    {
        m_mining_info_timer->stop();
    }

    persistance::Config_data get_config_data()
    {
        auto config_updated = m_pool_api_data_exchange->is_config_updated();
        if (config_updated)
        {
            m_cached_config = m_data_reader->get_config();
            config_updated = false;
            m_pool_api_data_exchange->set_config_updated(config_updated);
        }
        return m_cached_config;
    }

    std::uint32_t get_active_miners() const
    {
        return m_pool_api_data_exchange->get_active_miners();
    }

    common::Mining_info get_mining_info() const
    {
        return m_cached_mining_info;
    }

    common::System_info get_system_info() const
    {
        auto response = m_client->get_systeminfo();
        auto const status_code = response->getStatusCode();
        if (status_code != 200)
        {
            return common::System_info{};
        }

        auto data_json = nlohmann::json::parse(response->readBodyToString()->c_str());
        common::System_info system_info;
        system_info.m_wallet_version = data_json["result"]["version"];
        system_info.m_pool_version = "0.9";

        return system_info;
    }


private:

    chrono::Timer::Handler mining_info_handler(std::uint16_t mining_info_interval)
    {
        return[this, mining_info_interval]()
        {
            auto response = m_client->get_mininginfo();
            auto const status_code = response->getStatusCode();
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

            // restart timer
            m_mining_info_timer->start(chrono::Seconds(mining_info_interval), mining_info_handler(mining_info_interval));
        };
    }

    chrono::Timer_factory::Sptr m_timer_factory;
    Shared_data_reader::Sptr m_data_reader;
    common::Pool_api_data_exchange::Sptr m_pool_api_data_exchange;
    persistance::Config_data m_cached_config;
    common::Mining_info m_cached_mining_info;
    std::shared_ptr<Client> m_client;
    chrono::Component::Uptr m_chrono_component;
    chrono::Timer::Uptr m_mining_info_timer;
};


}
}

#endif