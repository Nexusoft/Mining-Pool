#include "reward/create_component.hpp"
#include "reward/component_impl.hpp"

namespace nexuspool {
namespace reward {

Component::Uptr create_component()
{
    return std::make_unique<Component_impl>();
}

}
}
