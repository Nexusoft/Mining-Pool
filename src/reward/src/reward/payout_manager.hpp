#ifndef NEXUSPOOL_REWARD_PAYOUT_MANAGER_HPP
#define NEXUSPOOL_REWARD_PAYOUT_MANAGER_HPP

#include "nexus_http_interface/component.hpp"
#include "persistance/data_writer.hpp"
#include "persistance/data_reader.hpp"
#include <spdlog/spdlog.h>
#include <vector>
#include <string>

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

    double calculate_reward_of_blocks(std::uint32_t round, bool& calculation_finished);
    bool payout(std::string const& account_from, std::string const& pin, std::uint32_t current_round);
    bool is_all_paid(std::uint32_t current_round) const;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    nexus_http_interface::Component& m_http_interface;
    persistance::Shared_data_writer& m_shared_data_writer;
    persistance::Data_reader& m_data_reader;
    std::uint32_t m_not_fully_paid_round;
    double m_current_avg_block_reward;
};

}
}

#endif
