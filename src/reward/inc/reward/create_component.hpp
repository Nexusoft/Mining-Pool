#ifndef NEXUSPOOL_REWARD_CREATE_COMPONENT_HPP
#define NEXUSPOOL_REWARD_CREATE_COMPONENT_HPP

#include "reward/component.hpp"
#include "nexus_http_interface/component.hpp"
#include "persistance/data_writer.hpp"
#include "persistance/data_reader.hpp"
#include "chrono/timer_factory.hpp"
#include <spdlog/spdlog.h>
#include <memory>
#include <string>

namespace nexuspool {
namespace reward {

// Component factory
Component::Uptr create_component(
	std::shared_ptr<spdlog::logger> logger, 
	chrono::Timer_factory::Sptr timer_factory,
	nexus_http_interface::Component::Sptr http_interface,
	persistance::Shared_data_writer::Sptr shared_data_writer, 
	persistance::Data_reader::Uptr data_reader,
	std::string account_from,
	std::string pin,
	std::uint16_t pool_fee,
	std::uint16_t update_block_hashes_interval,
	std::string fee_address);

}

}

#endif 
