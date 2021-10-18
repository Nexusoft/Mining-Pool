#ifndef NEXUSPOOL_REWARD_COMPONENT_IMPL_HPP
#define NEXUSPOOL_REWARD_COMPONENT_IMPL_HPP

#include "reward/component.hpp"
#include "nexus_http_interface/component.hpp"
#include "persistance/data_writer.hpp"
#include "persistance/data_reader.hpp"
#include "reward/payout_manager.hpp"
#include <spdlog/spdlog.h>
#include <vector>
#include <string>

namespace nexuspool {
namespace reward {

class Component_impl : public Component
{
public:
    Component_impl(
        std::shared_ptr<spdlog::logger> logger, 
        nexus_http_interface::Component::Uptr http_interface,
        persistance::Shared_data_writer::Sptr shared_data_writer, 
        persistance::Data_reader::Uptr data_reader,
        std::string account_from,
        std::string pin,
        std::uint16_t pool_fee);

    bool start_round(std::uint16_t round_duration_hours) override;
    bool is_round_active() override;
    std::uint32_t get_current_round() const;
    void get_start_end_round_times(std::chrono::system_clock::time_point& start_time, std::chrono::system_clock::time_point& end_time) override;
    bool end_round(std::uint32_t round_number) override;
    Difficulty_result check_difficulty(const LLP::CBlock& block, std::uint32_t pool_nbits) const override;
    bool pay_round(std::uint32_t round) override;

private:

    void update_block_hashes(std::uint32_t round);

    std::shared_ptr<spdlog::logger> m_logger;
    nexus_http_interface::Component::Uptr m_http_interface;
    persistance::Shared_data_writer::Sptr m_shared_data_writer;
    persistance::Data_reader::Uptr m_data_reader;
    std::uint32_t m_current_round;
    Payout_manager m_payout_manager;
    std::string m_account_from;
    std::string m_pin;
    std::uint16_t m_pool_fee;

};

}
}

#endif
