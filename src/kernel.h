// Copyright (c) 2012-2013 The PPCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef PPCOIN_KERNEL_H
#define PPCOIN_KERNEL_H

#ifndef BITCOIN_MAIN_H
 #include "main.h"
#endif

#ifndef BITCOIN_WALLET_H
 #include "wallet.h"
#endif

// ChainDB upgrade time
extern unsigned int nModifierUpgradeTime;

extern bool fCoinsDataActual;

struct PosMiningStuff {
    uint256 hashBlockFrom;
    ::uint64_t nStakeModifier;
    int nStakeModifierHeight;
    ::int64_t nStakeModifierTime;
};
// MODIFIER_INTERVAL_RATIO:
// ratio of group interval length between the last group and the first group
static const int MODIFIER_INTERVAL_RATIO = 3;

// Whether the given block is subject to new modifier protocol
bool IsFixedModifierInterval(unsigned int nTimeBlock);

// Compute the hash modifier for proof-of-stake
bool ComputeNextStakeModifier(
                              const CBlockIndex* pindexCurrent, 
                              ::uint64_t& nStakeModifier, 
                              bool& fGeneratedStakeModifier
                             );

bool CheckStakeKernelHash(
                          unsigned int nBits, 
                          const CBlock& blockFrom, 
                          unsigned int nTxPrevOffset, 
                          const CTransaction& txPrev, 
                          const COutPoint& prevout, 
                          unsigned int nTimeTx, 
                          uint256& hashProofOfStake, 
                          bool fPrintProofOfStake=false, 
                          PosMiningStuff *miningStuff=NULL
                         );

// The stake modifier used to hash for a stake kernel is chosen as the stake
// modifier about a selection interval later than the coin generating the kernel

// you can't think that the above is an intelligible sentence!  I can't figure out
// what it is saying, so it is pretty useless
bool GetKernelStakeModifier(uint256 hashBlockFrom, ::uint64_t& nStakeModifier);

// Check whether stake kernel meets hash target
// Sets hashProofOfStake on success return
bool CheckStakeKernelHash(
                          unsigned int nBits, 
                          const CBlock& blockFrom, 
                          ::uint32_t nTxPrevOffset, 
                          const CTransaction& txPrev, 
                          const COutPoint& prevout, 
                          ::uint32_t nTimeTx, 
                          uint256& hashProofOfStake, 
                          uint256& targetProofOfStake, 
                          bool fPrintProofOfStake=false
                         );


// Scan given coins set for kernel solution
bool ScanForStakeKernelHash(MetaMap &mapMeta, ::uint32_t nBits, ::uint32_t nTime, ::uint32_t nSearchInterval, CoinsSet::value_type &kernelcoin, ::uint32_t &nTimeTx, ::uint32_t &nBlockTime, ::uint64_t &nKernelsTried, ::uint64_t &nCoinDaysTried);

// Check kernel hash target and coinstake signature
// Sets hashProofOfStake on success return
bool CheckProofOfStake(CValidationState &state, const CTransaction& tx, unsigned int nBits, uint256& hashProofOfStake, uint256& targetProofOfStake);

// Get stake modifier checksum
::uint32_t GetStakeModifierChecksum(const CBlockIndex* pindex);

// Check stake modifier hard checkpoints
bool CheckStakeModifierCheckpoints(int nHeight, ::uint32_t nStakeModifierChecksum);

// Get time weight using supplied timestamps
::int64_t GetWeight(::int64_t nIntervalBeginning, ::int64_t nIntervalEnd);

#endif // PPCOIN_KERNEL_H
