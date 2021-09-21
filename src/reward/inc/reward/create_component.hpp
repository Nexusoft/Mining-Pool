#ifndef NEXUSPOOL_REWARD_CREATE_COMPONENT_HPP
#define NEXUSPOOL_REWARD_CREATE_COMPONENT_HPP

#include "reward/component.hpp"
#include "nexus_http_interface/component.hpp"
#include "persistance/data_writer.hpp"
#include "persistance/data_reader.hpp"
#include <spdlog/spdlog.h>
#include <memory>
#include <string>

namespace nexuspool {
namespace reward {

// Component factory
Component::Uptr create_component(
	std::shared_ptr<spdlog::logger> logger, 
	nexus_http_interface::Component::Uptr http_interface,
	persistance::Shared_data_writer::Sptr shared_data_writer, 
	persistance::Data_reader::Uptr data_reader,
	std::string account_from,
	std::string pin);

}

}

#endif 
