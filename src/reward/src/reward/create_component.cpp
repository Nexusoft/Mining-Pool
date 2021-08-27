#include "reward/create_component.hpp"
#include "reward/component_impl.hpp"

namespace nexuspool {
namespace reward {

Component::Uptr create_component(std::shared_ptr<spdlog::logger> logger, persistance::Shared_data_writer::Sptr shared_data_writer, persistance::Data_reader::Uptr data_reader)
{
    return std::make_unique<Component_impl>(std::move(logger), std::move(shared_data_writer), std::move(data_reader));
}

}
}
