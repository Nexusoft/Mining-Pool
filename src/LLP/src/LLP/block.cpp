
//#include "../hash/templates.h"
#include "LLP/block.hpp"

namespace nexuspool
{
namespace LLP
{
CBlock deserialize_block(std::vector<std::uint8_t> const& data)
{
	CBlock block;
	block.nVersion = nexuspool::bytes2uint(std::vector<uint8_t>(data.begin(), data.begin() + 4));

	block.hashPrevBlock.SetBytes(std::vector<uint8_t>(data.begin() + 4, data.begin() + 132));
	block.hashMerkleRoot.SetBytes(std::vector<uint8_t>(data.begin() + 132, data.end() - 20));

	block.nChannel = nexuspool::bytes2uint(std::vector<uint8_t>(data.end() - 20, data.end() - 16));
	block.nHeight = nexuspool::bytes2uint(std::vector<uint8_t>(data.end() - 16, data.end() - 12));
	block.nBits = nexuspool::bytes2uint(std::vector<uint8_t>(data.end() - 12, data.end() - 8));
	block.nNonce = nexuspool::bytes2uint64(std::vector<uint8_t>(data.end() - 8, data.end()));

	return block;
}
}
}