#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_IMPL_HPP

#include "persistance/component.hpp"
#include "persistance/data_storage_factory_impl.hpp"
#include "config/types.hpp"
#include <memory>

namespace spdlog { class logger; }
namespace nexuspool {
namespace persistance {

class Component_impl : public Component
{
public:
    Component_impl(std::shared_ptr<spdlog::logger> logger, config::Persistance_config config);

    Data_reader_factory::Sptr get_data_reader_factory() override;
    Data_writer_factory::Sptr get_data_writer_factory() override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    config::Persistance_config m_config;
    persistance::Data_storage_factory::Sptr m_data_storage_factory;
    Data_reader_factory::Sptr m_data_reader_factory;
    Data_writer_factory::Sptr m_data_writer_factory;
};

}
}

#endif
