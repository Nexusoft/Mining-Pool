#ifndef NEXUSPOOL_TYPES_HPP
#define NEXUSPOOL_TYPES_HPP

#include <memory>
#include <cstdint>
#include <functional>
#include "block.hpp"
#include "chrono/timer.hpp"


namespace nexuspool
{
    using Get_block_handler = std::function<void(LLP::CBlock const& block)>;

    enum class Submit_block_result : std::uint8_t
    {
        accept = 0, 
        reject, 
        block_found 
    };
    using Submit_block_handler = std::function<void(Submit_block_result result)>;
}

#endif