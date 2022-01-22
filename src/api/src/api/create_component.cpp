#include "api/create_component.hpp"
#include "api/component_impl.hpp"

namespace nexuspool 
{
namespace api 
{

Component::Uptr create_component(std::shared_ptr<spdlog::logger> logger,
    persistance::Data_reader::Uptr data_reader,
    config::Config_api::Sptr config_api,
    common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
    chrono::Timer_factory::Sptr timer_factory)
{
    return std::make_unique<Component_impl>(std::move(logger), 
        std::move(data_reader), 
        std::move(config_api),
        std::move(pool_api_data_exchange), 
        std::move(timer_factory));
}

}
}
