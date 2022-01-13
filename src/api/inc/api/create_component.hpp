#ifndef NEXUSPOOL_API_CREATE_COMPONENT_HPP
#define NEXUSPOOL_API_CREATE_COMPONENT_HPP

#include "api/component.hpp"
#include "persistance/data_reader.hpp"
#include "common/pool_api_data_exchange.hpp"
#include "config/config_api.hpp"
#include "chrono/timer_factory.hpp"

#include <memory>

namespace spdlog { class logger; }
namespace nexuspool 
{
namespace api 
{

Component::Uptr create_component(std::shared_ptr<spdlog::logger> logger,
    persistance::Data_reader::Uptr data_reader,
    config::Config_api::Sptr config_api,
    common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
    chrono::Timer_factory::Sptr timer_factory);

}
}

#endif 
