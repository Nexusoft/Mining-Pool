#ifndef NEXUSPOOL_LLP_UTILS_H
#define NEXUSPOOL_LLP_UTILS_H

#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>

namespace nexuspool
{

/** Determines the Decimal of nBits per Channel for a decent "Frame of Reference".
Has no functionality in Network Operation. **/
inline double get_difficulty(uint32_t nBits, int nChannel)
{
	/** Prime Channel is just Decimal Held in Integer
		Multiplied and Divided by Significant Digits. **/
	if (nChannel == 1)
		return nBits / 10000000.0;

	/** Get the Proportion of the Bits First. **/
	double dDiff = (double)0x0000ffff / (double)(nBits & 0x00ffffff);

	/** Calculate where on Compact Scale Difficulty is. **/
	int nShift = nBits >> 24;

	/** Shift down if Position on Compact Scale is above 124. **/
	while (nShift > 124)
	{
		dDiff = dDiff / 256.0;
		nShift--;
	}

	/** Shift up if Position on Compact Scale is below 124. **/
	while (nShift < 124)
	{
		dDiff = dDiff * 256.0;
		nShift++;
	}

	/** Offset the number by 64 to give larger starting reference. **/
	return dDiff * ((nChannel == 2) ? 64 : 1024 * 1024 * 256);
}

//Estimate the network hashrate or prime search rate.
//Hash channel units are hashes/second. Prime channel units is range searched per second. 
//nBits is the network nBits.  avg_seconds_between_blocks is the average time between blocks for that channel. This value is available from the wallet. 
inline double get_network_hash_rate(uint32_t nBits, int nChannel, double avg_seconds_between_blocks = 155.0)
{
	double difficulty = get_difficulty(nBits, nChannel);
	//Prime
	if (nChannel == 1)
	{
		double search_rate = 7.47101117 * std::exp(4.31557609 * difficulty) / avg_seconds_between_blocks;
		return search_rate;
	}
	else
	//Hash
	{
		constexpr uint64_t C = (1ull << 34);
		double hash_rate = C * difficulty / avg_seconds_between_blocks;
		return hash_rate;
	}
}

//Estimate the hash rate for a worker based on the rate of shares submitted.
//prime_shares_to_blocks_ratio is only required for the prime channel.  It is dependent on the miner implementation and the pool difficulty setting.
//prime_shares_to_blocks_ratio can be measured directly as total shares/total blocks if the pool has enough history. 
inline double get_miner_hash_rate(uint32_t pool_nBits, uint32_t network_nBits, int nChannel, double avg_seconds_between_shares, double prime_shares_to_blocks_ratio = 352.0)
{
	//Prime
	if (nChannel == 1)
	{

		double estimated_seconds_between_blocks = avg_seconds_between_shares * prime_shares_to_blocks_ratio;
		double search_rate = get_network_hash_rate(network_nBits, nChannel, estimated_seconds_between_blocks);
		return search_rate;
	}
	else
	//Hash
	{
		double hash_rate = get_network_hash_rate(pool_nBits, nChannel, avg_seconds_between_shares);
		return hash_rate;
	}

}

/** Convert a 32 bit Unsigned Integer to Byte Vector using Bitwise Shifts. **/
inline std::vector<uint8_t> uint2bytes(uint32_t UINT)
{
	std::vector<uint8_t> BYTES(4, 0);
	BYTES[0] = UINT >> 24;
	BYTES[1] = UINT >> 16;
	BYTES[2] = UINT >> 8;
	BYTES[3] = UINT;

	return BYTES;
}


/** Convert a byte stream into unsigned integer 32 bit. **/
inline uint32_t bytes2uint(std::vector<uint8_t> const& BYTES, int nOffset = 0)
{
	if (BYTES.size() < nOffset + 4)
		return 0;

	return (BYTES[0 + nOffset] << 24) + (BYTES[1 + nOffset] << 16) + (BYTES[2 + nOffset] << 8) + BYTES[3 + nOffset];
}

/** Convert a 64 bit Unsigned Integer to Byte Vector using Bitwise Shifts. **/
inline std::vector<uint8_t> uint2bytes64(uint64_t UINT)
{
	std::vector<uint8_t> INTS[2];
	INTS[0] = uint2bytes(static_cast<uint32_t>(UINT));
	INTS[1] = uint2bytes(static_cast<uint32_t>(UINT >> 32));

	std::vector<uint8_t> BYTES;
	BYTES.insert(BYTES.end(), INTS[0].begin(), INTS[0].end());
	BYTES.insert(BYTES.end(), INTS[1].begin(), INTS[1].end());

	return BYTES;
}


/** Convert a byte Vector into unsigned integer 64 bit. **/
inline uint64_t bytes2uint64(std::vector<uint8_t> const& BYTES, int nOffset = 0) 
{
	return (bytes2uint(BYTES, nOffset) | (static_cast<uint64_t>(bytes2uint(BYTES, nOffset + 4)) << 32));
}

/** Convert Standard String into Byte Vector. **/
inline std::vector<uint8_t> string2bytes(std::string const& STRING)
{
	std::vector<uint8_t> BYTES(STRING.begin(), STRING.end());
	return BYTES;
}


/** Convert Byte Vector into Standard String. **/
inline std::string bytes2string(std::vector<uint8_t> const& BYTES, int nOffset = 0)
{
	std::string STRING(BYTES.begin() + nOffset, BYTES.end());
	return STRING;
}

/** Convert double into Byte Vector **/
inline std::vector<uint8_t> double2bytes(double DOUBLE)
{
	union {
		double DOUBLE;
		uint64_t UINT64;
	} u;
	u.DOUBLE = DOUBLE;

	return uint2bytes64(u.UINT64);
}

/** Convert Byte Vector into double **/
inline double bytes2double(std::vector<uint8_t> const& BYTES)
{
	uint64_t n64 = bytes2uint64(BYTES);
	union {
		double DOUBLE;
		uint64_t UINT64;
	} u;
	u.UINT64 = n64;
	return u.DOUBLE;
}

}

#endif
