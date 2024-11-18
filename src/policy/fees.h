// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef YACOIN_POLICYFEE_H
#define YACOIN_POLICYFEE_H

#include "amount.h"

static const ::int64_t MIN_TX_FEE = CENT;
static const ::int64_t MIN_RELAY_TX_FEE = MIN_TX_FEE;

::int64_t GetMinFee(unsigned int nBytes)
{
    // Fee-per-kilobyte amount considered the same as "free"
    // Be careful setting this: if you set it to zero then
    // a transaction spammer can cheaply fill blocks using
    // 1-satoshi-fee transactions. It should be set above the real
    // cost to you of processing a transaction.
    return (nBytes * MIN_TX_FEE) / 1000;
}

#endif
