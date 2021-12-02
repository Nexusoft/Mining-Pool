#ifndef NEXUS_LLP_BLOCK_H
#define NEXUS_LLP_BLOCK_H

#include "LLC/types/uint1024.h"
#include "LLC/hash/SK.h"
#include "utils.hpp"
#include <memory>

#define BEGIN(a)            ((char*)&(a))
#define END(a)              ((char*)&((&(a))[1]))

namespace nexuspool
{
namespace LLP
{

	/** Mock Class for Building Block Hash. **/
	class CBlock
	{
	public:
		using Uptr = std::unique_ptr<CBlock>;
		using Sptr = std::shared_ptr<CBlock>;

		/** Begin of Header.   BEGIN(nVersion) **/
		std::uint32_t  nVersion{ 0 };
		uint1024_t hashPrevBlock{ 0 };
		uint512_t hashMerkleRoot{ 0 };
		std::uint32_t nChannel{ 0 };
		std::uint32_t nHeight{ 0 };
		std::uint32_t nBits{ 0 };
		std::uint64_t nNonce{ 0 };
		/** End of Header.     END(nNonce).
			All the components to build an SK1024 Block Hash. **/

		std::vector<std::uint8_t> serialize() const
		{
			auto const VERSION = nexuspool::uint2bytes(nVersion);
			auto const PREVIOUS = hashPrevBlock.GetBytes();
			auto const MERKLE = hashMerkleRoot.GetBytes();
			auto const  CHANNEL = nexuspool::uint2bytes(nChannel);
			auto const HEIGHT = nexuspool::uint2bytes(nHeight);
			auto const BITS = nexuspool::uint2bytes(nBits);
			auto const NONCE = nexuspool::uint2bytes64(nNonce);

			std::vector<uint8_t> vData;
			vData.insert(vData.end(), VERSION.begin(), VERSION.end());
			vData.insert(vData.end(), PREVIOUS.begin(), PREVIOUS.end());
			vData.insert(vData.end(), MERKLE.begin(), MERKLE.end());
			vData.insert(vData.end(), CHANNEL.begin(), CHANNEL.end());
			vData.insert(vData.end(), HEIGHT.begin(), HEIGHT.end());
			vData.insert(vData.end(), BITS.begin(), BITS.end());
			vData.insert(vData.end(), NONCE.begin(), NONCE.end());

			return vData;
		}

		inline uint1024_t GetHash() const { return nChannel == 1 ? LLC::SK1024(BEGIN(nVersion), END(nBits)) : LLC::SK1024(BEGIN(nVersion), END(nNonce)); }
		inline uint1024_t GetPrime() const { return GetHash() + nNonce; }
	};


	// Convert the Header of a Block into a Byte Stream for Reading and Writing Across Sockets
	CBlock deserialize_block(std::vector<std::uint8_t> const& data);

}
}

#endif