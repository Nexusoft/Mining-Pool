/*__________________________________________________________________________________________
(c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++
(c) Copyright The Nexus Developers 2014 - 2019
Distributed under the MIT software license, see the accompanying
file COPYING or http://www.opensource.org/licenses/mit-license.php.
"ad vocem populi" - To The Voice of The People
____________________________________________________________________________________________*/

#include "TAO/Ledger/difficulty.h"
#include "TAO/Ledger/prime.h"


/* Global TAO namespace. */
namespace TAO
{

    /* Ledger Layer namespace. */
    namespace Ledger
    {
        //given the block hash and channel, return the actual difficulty of the block
        double GetDifficulty(const uint1024_t& block_hash, std::int32_t nChannel)
        {
            double actual_difficulty;
            if (nChannel == 1) //prime
            {
                std::vector<uint8_t> offsets;
                actual_difficulty = TAO::Ledger::GetPrimeDifficulty(block_hash, offsets);
            }
            else //hash
            {
                
                uint32_t nbits_actual = block_hash.GetCompact();
                actual_difficulty = TAO::Ledger::GetDifficulty(nbits_actual, nChannel);
            }
            return actual_difficulty;
            
        }

        


        /* Determines the Decimal of nBits per Channel for a decent "Frame of Reference". */
        double GetDifficulty(std::uint32_t nBits, std::int32_t nChannel)
        {

            /* Prime Channel is just Decimal Held in Integer
            Multiplied and Divided by Significant Digits. */
            if (nChannel == 1)
                return nBits / 10000000.0;

            /* Check for divide by zero. */
            if (nBits == 0)
                return 0.0;

            /* Get the Proportion of the Bits First. */
            double dDiff = (double)0x0000ffff / (double)(nBits & 0x00ffffff);

            /* Calculate where on Compact Scale Difficulty is. */
            uint32_t nShift = nBits >> 24;

            /* Shift down if Position on Compact Scale is above 124. */
            while (nShift > 124)
            {
                dDiff = dDiff / 256.0;
                --nShift;
            }

            /* Shift up if Position on Compact Scale is below 124. */
            while (nShift < 124)
            {
                dDiff = dDiff * 256.0;
                ++nShift;
            }

            /* Offset the number by 64 to give larger starting reference. */
            return dDiff * ((nChannel == 2) ? 64 : 1024 * 1024 * 256);
        }
    }
}