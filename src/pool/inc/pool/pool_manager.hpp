#ifndef NEXUSPOOL_POOL_MANAGER_HPP
#define NEXUSPOOL_POOL_MANAGER_HPP

#include "block.hpp"
#include "pool/types.hpp"
#include "reward/component.hpp"
#include "persistance/data_writer_factory.hpp"
#include "persistance/data_reader_factory.hpp"
#include "persistance/types.hpp"
#include "chrono/timer_factory.hpp"
#include "config/config.hpp"
#include "network/socket_factory.hpp"

#include <memory>

namespace asio { class io_context; }
namespace spdlog { class logger; }

namespace nexuspool
{
class Pool_manager
{
public:

    using Sptr = std::shared_ptr<Pool_manager>;
    virtual ~Pool_manager() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    // Methods towards wallet_connection
    virtual void set_current_height(std::uint32_t height) = 0;
    virtual void set_block(LLP::CBlock const& block) = 0;
    virtual void add_block_to_storage(std::uint32_t block_map_id) = 0;

    // Methods towards miner_connection
    virtual void get_block(Get_block_handler&& handler) = 0;
    virtual void submit_block(std::unique_ptr<LLP::CBlock> block, std::string const& blockfinder, Submit_block_handler handler) = 0;
    virtual std::uint32_t get_pool_nbits() const = 0;
};

Pool_manager::Sptr create_pool_manager(std::shared_ptr<asio::io_context> io_context,
    std::shared_ptr<spdlog::logger> logger,
    config::Config::Sptr config,
    chrono::Timer_factory::Sptr timer_factory,
    network::Socket_factory::Sptr socket_factory,
    persistance::Data_writer_factory::Sptr data_writer_factory,
    persistance::Data_reader_factory::Sptr data_reader_factory);

}

#endif
