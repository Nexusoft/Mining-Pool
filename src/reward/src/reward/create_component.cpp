#include "reward/create_component.hpp"
#include "reward/component_impl.hpp"

namespace nexuspool {
namespace reward {

Component::Uptr create_component(
    std::shared_ptr<spdlog::logger> logger, 
    chrono::Timer_factory::Sptr timer_factory,
    nexus_http_interface::Component::Sptr http_interface,
    persistance::Shared_data_writer::Sptr shared_data_writer, 
    persistance::Data_reader::Uptr data_reader,
    std::string account_from,
    std::string pin,
    std::uint16_t pool_fee,
    std::uint16_t update_block_hashes_interval)
{
    return std::make_unique<Component_impl>(
        std::move(logger), 
        std::move(timer_factory),
        std::move(http_interface), 
        std::move(shared_data_writer), 
        std::move(data_reader),
        std::move(account_from),
        std::move(pin),
        pool_fee,
        update_block_hashes_interval);
}

}
}
