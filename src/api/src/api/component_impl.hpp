#ifndef NEXUSPOOL_API_COMPONENT_IMPL_HPP
#define NEXUSPOOL_API_COMPONENT_IMPL_HPP

#include "api/component.hpp"
#include "persistance/data_reader.hpp"
#include "common/pool_api_data_exchange.hpp"
#include "config/config_api.hpp"
#include "chrono/timer_factory.hpp"

#include <thread>
#include <memory>
#include <vector>
#include <string>
#include <atomic>

namespace spdlog { class logger; }
namespace nexuspool
{
namespace api
{
class Shared_data_reader;
class NXS_client;

class Component_impl : public Component
{
public:

    Component_impl(std::shared_ptr<spdlog::logger> logger,
        persistance::Data_reader::Uptr data_reader,
        config::Config_api::Sptr config_api,
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
        chrono::Timer_factory::Sptr timer_factory);

    void start() override;
    void stop() override;

private:

    std::thread m_server_thread;
    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<Shared_data_reader> m_shared_data_reader;
    config::Config_api::Sptr m_config_api;
    common::Pool_api_data_exchange::Sptr m_pool_api_data_exchange;
    chrono::Timer_factory::Sptr m_timer_factory;
    std::shared_ptr<NXS_client> m_nxs_client;
    std::atomic_bool m_server_stopped;

};

}
}

#endif
