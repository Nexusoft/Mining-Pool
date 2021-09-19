#ifndef NEXUSPOOL_REWARD_PAYOUT_MANAGER_HPP
#define NEXUSPOOL_REWARD_PAYOUT_MANAGER_HPP

#include "nexus_http_interface/component.hpp"
#include "persistance/data_writer.hpp"
#include "persistance/data_reader.hpp"
#include <spdlog/spdlog.h>
#include <vector>

namespace nexuspool {
namespace reward {

class Payout_manager
{
public:
    Payout_manager(
        std::shared_ptr<spdlog::logger> logger,
        nexus_http_interface::Component& http_interface,
        persistance::Shared_data_writer& shared_data_writer,
        persistance::Data_reader& data_reader);

    void calculate_reward_of_blocks(std::uint32_t round);
    void payout(std::uint32_t current_round);

private:

    std::shared_ptr<spdlog::logger> m_logger;
    nexus_http_interface::Component& m_http_interface;
    persistance::Shared_data_writer& m_shared_data_writer;
    persistance::Data_reader& m_data_reader;
};

}
}

#endif
