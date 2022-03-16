#ifndef NEXUSPOOL_MINER_CONNECTION_HPP
#define NEXUSPOOL_MINER_CONNECTION_HPP

#include "network/connection.hpp"
#include "pool/session.hpp"
#include <memory>

namespace spdlog { class logger; }

namespace nexuspool
{
namespace LLP { class CBlock; }
class Pool_manager;

class Miner_connection
{
public:

    using Sptr = std::shared_ptr<Miner_connection>;
    virtual ~Miner_connection() = default;

    virtual void stop() = 0;
    virtual void send_work(LLP::CBlock const& block) = 0;
    virtual network::Connection::Handler connection_handler() = 0;
    virtual void get_hashrate() = 0;
    virtual void send_pool_notification(std::string message) = 0;
};

Miner_connection::Sptr create_miner_connection(std::shared_ptr<spdlog::logger> logger,
    network::Connection::Sptr connection,
    std::weak_ptr<Pool_manager> pool_manager,
    Session_key session_key,
    Session_registry::Sptr session_registry);

}

#endif
