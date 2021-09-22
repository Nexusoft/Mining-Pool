#include "reward/create_component.hpp"
#include "reward/component_impl.hpp"

namespace nexuspool {
namespace reward {

Component::Uptr create_component(
    std::shared_ptr<spdlog::logger> logger, 
    nexus_http_interface::Component::Uptr http_interface,
    persistance::Shared_data_writer::Sptr shared_data_writer, 
    persistance::Data_reader::Uptr data_reader,
    std::string account_from,
    std::string pin,
    std::uint16_t pool_fee)
{
    return std::make_unique<Component_impl>(
        std::move(logger), 
        std::move(http_interface), 
        std::move(shared_data_writer), 
        std::move(data_reader),
        std::move(account_from),
        std::move(pin),
        pool_fee);
}

}
}
