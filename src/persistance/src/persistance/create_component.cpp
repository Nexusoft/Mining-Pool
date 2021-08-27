#include "persistance/create_component.hpp"
#include "persistance/component_impl.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool {
namespace persistance {

Component::Uptr create_component(std::shared_ptr<spdlog::logger> logger, config::Persistance_config config)
{
    return std::make_unique<Component_impl>(std::move(logger), std::move(config));
}

}
}
