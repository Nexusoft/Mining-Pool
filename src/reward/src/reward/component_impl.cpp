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
    if (!m_shared_data_writer->create_round())
    {
        m_logger->error("Failed to create a new round!");
    }
}

bool Component_impl::end_round(std::uint32_t round_number)
{
    auto const round_data = m_data_reader->get_latest_round();
    if (round_data.m_round != round_number)
    {
        m_logger->error("Failed to end round {}. Latest round is {}", round_number, round_data.m_round);
        return false;
    }

    if (!round_data.m_is_active)
    {
        m_logger->error("Failed to end round {}. Round is already ended.", round_number);
        return false;
    }

    if (round_data.m_total_rewards > 0) // did the pool actually earned something this round?
    {
        // get all accounts which contribute to the current round
        auto const active_accounts = m_data_reader->get_active_accounts_from_round();
        for (auto& active_account : active_accounts)
        {
            // calculate reward for account
            auto account_reward = round_data.m_total_rewards * (active_account.m_shares / round_data.m_total_shares);
            // add account to payment table (without datetime -> not paid yet)
            m_shared_data_writer->add_payment(persistance::Payment_data{ active_account.m_address, account_reward, active_account.m_shares, "", round_data.m_round });

            // update account -> reset shares
        }
    }



    // end round now
   // round_data.m


    return true;
}

}
}
