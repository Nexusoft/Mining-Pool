#ifndef NEXUSPOOL_POOL_POOL_MANAGER_MOCK_HPP
#define NEXUSPOOL_POOL_POOL_MANAGER_MOCK_HPP

#include <gmock/gmock.h>
#include "pool/pool_manager.hpp"
#include <memory>

namespace nexuspool
{
class Pool_manager_mock : public Pool_manager
{
public:

    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, set_current_height, (std::uint32_t height), (override));
    MOCK_METHOD(void, set_block, (LLP::CBlock const& block), (override));
    MOCK_METHOD(void, add_block_to_storage, (std::uint32_t block_map_id), (override));
    MOCK_METHOD(void, get_block, (Get_block_handler&& handler), (override));
    MOCK_METHOD(void, submit_block, (std::unique_ptr<LLP::CBlock> block, Session_key miner_key, Submit_block_handler handler), (override));
    MOCK_METHOD(std::uint32_t, get_pool_nbits, (), (const override));
};

}

#endif
