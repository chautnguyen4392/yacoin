// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifdef _MSC_VER
    #include "msvc_warnings.push.h"
#endif

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#ifndef BITCOIN_CHECKPOINT_H
 #include "checkpoints.h"
#endif

#ifndef BITCOIN_TXDB_H
 #include "txdb.h"
#endif
#include "streams.h"
#include "reverse_iterator.h"

namespace Checkpoints
{
    typedef std::map<int, std::pair<uint256, unsigned int> > MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
#ifndef LOW_DIFFICULTY_FOR_DEVELOPMENT
        ( 0, std::make_pair(uint256("0x0000060fc90618113cde415ead019a1052a9abc43afcccff38608ff8751353e5"), 1367991220) )
#else
        ( 0, std::make_pair(uint256("0x1ddf335eb9c59727928cabf08c4eb1253348acde8f36c6c4b75d0b9686a28848"), 1367991220) )
#endif
        ;

    // TestNet has no checkpoints

// YACOIN TODO CHANGE
    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = mapCheckpoints;
        MapCheckpoints::const_iterator 
            i = checkpoints.find(nHeight);

        if (i == checkpoints.end()) 
            return true;
        return (hash == i->second.first);
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = mapCheckpoints;
        return checkpoints.rbegin()->first;
    }

    unsigned int GetLastCheckpointTime()
    {
        MapCheckpoints& checkpoints = mapCheckpoints;
        return checkpoints.rbegin()->second.second;
    }

    CBlockIndex* GetLastCheckpoint(const BlockMap& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = mapCheckpoints;
        for (const MapCheckpoints::value_type& i : reverse_iterate(checkpoints)) {
            const uint256& hash = i.second.first;
            BlockMap::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return nullptr;
    }
}

#ifdef _MSC_VER
    #include "msvc_warnings.pop.h"
#endif
