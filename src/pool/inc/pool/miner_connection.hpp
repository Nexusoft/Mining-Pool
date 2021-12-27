#ifndef NEXUSPOOL_MINER_CONNECTION_HPP
#define NEXUSPOOL_MINER_CONNECTION_HPP

#include "network/connection.hpp"
#include "network/connection.hpp"
#include "pool/session.hpp"
#include <memory>

namespace spdlog { class logger; }

namespace nexuspool
{
class Pool_manager;

class Miner_connection
{
public:

    using Sptr = std::shared_ptr<Miner_connection>;
    virtual ~Miner_connection() = default;

    virtual void stop() = 0;
    virtual network::Connection::Handler connection_handler() = 0;
    virtual void set_current_height(std::uint32_t height) = 0;
    virtual void get_hashrate() = 0;
};

Miner_connection::Sptr create_miner_connection(std::shared_ptr<spdlog::logger> logger,
    network::Connection::Sptr connection,
    std::weak_ptr<Pool_manager> pool_manager,
    Session_key session_key,
    Session_registry::Sptr session_registry);

}

#endif
