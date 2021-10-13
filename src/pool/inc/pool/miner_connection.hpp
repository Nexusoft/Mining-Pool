#ifndef NEXUSPOOL_MINER_CONNECTION_HPP
#define NEXUSPOOL_MINER_CONNECTION_HPP

#include "network/connection.hpp"
#include <memory>

namespace nexuspool
{
class Miner_connection
{
public:

    virtual ~Miner_connection() = default;

    virtual void stop() = 0;
    virtual network::Connection::Handler connection_handler() = 0;
    virtual void set_current_height(std::uint32_t height) = 0;
};
}

#endif
