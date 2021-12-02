#ifndef NEXUSPOOL_WALLET_CONNECTION_HPP
#define NEXUSPOOL_WALLET_CONNECTION_HPP

#include "network/endpoint.hpp"
#include "network/types.hpp"
#include "pool/types.hpp"
#include "common/types.hpp"
#include <memory>

namespace nexuspool
{

class Wallet_connection
{
public:

    virtual ~Wallet_connection() = default;

    virtual bool connect(network::Endpoint const& wallet_endpoint) = 0;

    // Close connection
    virtual void stop() = 0;

    virtual void submit_block(network::Shared_payload&& block_data, std::uint32_t block_map_id, Submit_block_handler&& handler) = 0;
    virtual void get_block(Get_block_handler&& handler) = 0;
};

}

#endif
