#include "persistance/create_component.hpp"
#include "persistance/component_impl.hpp"

namespace nexuspool {
namespace persistance {

Component::Uptr create_component(config::Config& config)
{
    return std::make_unique<Component_impl>(config);
}

}
}
