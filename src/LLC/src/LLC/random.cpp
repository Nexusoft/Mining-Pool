/*__________________________________________________________________________________________
            (c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++
            (c) Copyright The Nexus Developers 2014 - 2019
            Distributed under the MIT software license, see the accompanying
            file COPYING or http://www.opensource.org/licenses/mit-license.php.
            "ad vocem populi" - To the Voice of the People
____________________________________________________________________________________________*/

#include "LLC/random.h"
#include <openssl/rand.h>

namespace LLC
{
    uint64_t GetRand(uint64_t nMax)
    {
        if (nMax == 0)
            return 0;

        // The range of the random source must be a multiple of the modulus
        // to give every possible output value an equal possibility
        uint64_t nRange = (std::numeric_limits<uint64_t>::max() / nMax) * nMax;
        uint64_t nRand = 0;
        do
        {
            RAND_bytes((uint8_t*)&nRand, sizeof(nRand));
        } while (nRand >= nRange);

        return (nRand % nMax);
    }

    int GetRandInt(int nMax)
    {
        return (int)GetRand(nMax);
    }


    uint128_t GetRand128()
    {
        uint128_t hash;
        RAND_bytes((uint8_t*)&hash, sizeof(hash));
        return hash;
    }


    uint256_t GetRand256()
    {
        uint256_t hash;
        RAND_bytes((uint8_t*)&hash, sizeof(hash));
        return hash;
    }


    uint512_t GetRand512()
    {
        uint512_t hash;
        RAND_bytes((uint8_t*)&hash, sizeof(hash));
        return hash;
    }


    uint1024_t GetRand1024()
    {
        uint1024_t hash;
        RAND_bytes((uint8_t*)&hash, sizeof(hash));
        return hash;
    }

}