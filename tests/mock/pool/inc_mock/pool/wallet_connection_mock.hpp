#ifndef NEXUSPOOL_POOL_WALLET_CONNECTION_MOCK_HPP
#define NEXUSPOOL_POOL_WALLET_CONNECTION_MOCK_HPP

#include <gmock/gmock.h>
#include "pool/wallet_connection.hpp"

namespace nexuspool
{

class Wallet_connection_mock : public Wallet_connection
{
public:

    MOCK_METHOD(bool, connect, (network::Endpoint const& wallet_endpoint), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, submit_block, (network::Shared_payload&& block_data, std::uint32_t block_map_id, Submit_block_handler&& handler), (override));
    MOCK_METHOD(void, get_block, (Get_block_handler&& handler), (override));
};

}

#endif
