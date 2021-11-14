#ifndef NEXUSPOOL_POOL_MANAGER_IMPL_HPP
#define NEXUSPOOL_POOL_MANAGER_IMPL_HPP

#include "pool/pool_manager.hpp"
#include "network/types.hpp"

#include <mutex>
#include <atomic>

namespace nexuspool
{
class Wallet_connection;
class Session_registry;

class Pool_manager_impl : public Pool_manager, public std::enable_shared_from_this<Pool_manager_impl>
{
public:

    Pool_manager_impl(std::shared_ptr<asio::io_context> io_context,
        std::shared_ptr<spdlog::logger> logger,
        config::Config::Sptr config,
        chrono::Timer_factory::Sptr timer_factory,
        network::Socket_factory::Sptr socket_factory,
        persistance::Data_writer_factory::Sptr data_writer_factory,
        persistance::Data_reader_factory::Sptr data_reader_factory);

    void start() override;
    void stop() override;

    // Methods towards wallet_connection
    void set_current_height(std::uint32_t height) override;
    void set_block(LLP::CBlock const& block) override;
    void add_block_to_storage(std::uint32_t block_map_id) override;

    // Methods towards miner_connection
    void get_block(Get_block_handler&& handler) override;
    void submit_block(std::unique_ptr<LLP::CBlock> block, std::string const& blockfinder, Submit_block_handler handler) override;
    std::uint32_t get_pool_nbits() const override;

private:

    chrono::Timer::Handler session_registry_maintenance_handler(std::uint16_t session_registry_maintenance_interval);
    chrono::Timer::Handler end_round_handler();
    void end_round();
    persistance::Config_data storage_config_check();

    std::shared_ptr<::asio::io_context> m_io_context;
    std::shared_ptr<spdlog::logger> m_logger;
    config::Config::Sptr m_config;
    persistance::Config_data m_storage_config_data;
    chrono::Timer_factory::Sptr m_timer_factory;
    network::Socket_factory::Sptr m_socket_factory;
    persistance::Data_writer_factory::Sptr m_data_writer_factory;
    persistance::Data_reader_factory::Sptr m_data_reader_factory;
    reward::Component::Uptr m_reward_component;

    std::shared_ptr<Wallet_connection> m_wallet_connection;     // connection to nexus wallet
    network::Socket::Sptr m_listen_socket;

    std::shared_ptr<Session_registry> m_session_registry;    // holds all sessions -> each session contains a miner_connection
    chrono::Timer::Uptr m_session_registry_maintenance;
    chrono::Timer::Uptr m_end_round_timer;

    // connection variables
    std::atomic<std::uint32_t> m_current_height;
    std::mutex m_block_mutex;
    LLP::CBlock m_block;
    std::uint32_t m_pool_nBits;

    // variables for block to storage
    struct Submit_block_data
    {
        std::shared_ptr<LLP::CBlock> m_block;
        std::string m_blockfinder;
    };
    std::atomic<std::uint32_t> m_block_map_id;
    std::map<std::uint32_t, Submit_block_data> m_block_map;
};

}

#endif
