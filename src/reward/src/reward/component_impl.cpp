#include "component_impl.hpp"



namespace nexuspool {
namespace reward {

Component_impl::Component_impl(std::shared_ptr<spdlog::logger> logger, persistance::Shared_data_writer::Sptr shared_data_writer, persistance::Data_reader::Uptr data_reader)
    : m_logger{std::move(logger)}
    , m_shared_data_writer{ std::move(shared_data_writer) }
    , m_data_reader{ std::move(data_reader) }
{}

void Component_impl::start_round()
{
    m_shared_data_writer->create_round();
}

bool Component_impl::end_round(std::uint32_t round_number)
{
    return true;
}

}
}
