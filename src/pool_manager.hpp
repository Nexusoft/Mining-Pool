#ifndef NEXUSPOOL_POOL_MANAGER_HPP
#define NEXUSPOOL_POOL_MANAGER_HPP

#include "network/socket_factory.hpp"
#include "network/types.hpp"
#include "chrono/timer_factory.hpp"
#include "session.hpp"
#include "block.hpp"
#include "types.hpp"
#include "reward/component.hpp"
#include "persistance/data_writer_factory.hpp"
#include "persistance/data_reader_factory.hpp"
#include "persistance/types.hpp"
#include "config/config.hpp"

#include <memory>
#include <mutex>
#include <atomic>

namespace asio { class io_context; }
namespace spdlog { class logger; }

namespace nexuspool
{
class Wallet_connection;

class Pool_manager : public std::enable_shared_from_this<Pool_manager>
{
public:

    Pool_manager(std::shared_ptr<asio::io_context> io_context, 
        std::shared_ptr<spdlog::logger> logger,
        config::Config::Sptr config, 
        persistance::Config_data storage_config_data,
        network::Socket_factory::Sptr socket_factory,
        persistance::Data_writer_factory::Sptr data_writer_factory,
        persistance::Data_reader_factory::Sptr data_reader_factory);

    void start();
    void stop();

    // Methods towards wallet_connection
    void set_current_height(std::uint32_t height);
    void set_block(LLP::CBlock const& block);
    void add_block_to_storage(LLP::CBlock const& block);

    // Methods towards miner_connection
    void get_block(Get_block_handler&& handler);
    void submit_block(std::unique_ptr<LLP::CBlock> block, std::vector<std::uint8_t> const& block_merkle_root, std::uint64_t nonce, Submit_block_handler handler);
    std::uint32_t get_pool_nbits() const;

private:

    chrono::Timer::Handler session_registry_maintenance_handler(std::uint16_t session_registry_maintenance_interval);
    chrono::Timer::Handler end_round_handler();
    void end_round();

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

    Session_registry m_session_registry;    // holds all sessions -> each session contains a miner_connection
    chrono::Timer::Uptr m_session_registry_maintenance;
    chrono::Timer::Uptr m_end_round_timer;

    // connection variables
    std::atomic<std::uint32_t> m_current_height;
    std::mutex m_block_mutex;
    LLP::CBlock m_block;
    std::uint32_t m_pool_nBits;
};
}

#endif
