// Copyright (c) 2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_VERSION_H
#define BITCOIN_VERSION_H

#ifndef CLIENTVERSION_H
 #include "clientversion.h"
#endif

#include <string>

//
// client versioning
//
extern const int
    DISPLAY_VERSION_MAJOR_for_Qt,   
    DISPLAY_VERSION_MINOR_for_Qt,   
    DISPLAY_VERSION_REVISION_for_Qt,
    DISPLAY_VERSION_BUILD_for_Qt,
    DISPLAY_VERSION_TESTING_for_Qt;   


static const int CLIENT_VERSION =
                           1000000 * CLIENT_VERSION_MAJOR
                         +   10000 * CLIENT_VERSION_MINOR
                         +     100 * CLIENT_VERSION_REVISION
                         +       1 * CLIENT_VERSION_BUILD;

extern const std::string CLIENT_NAME;
extern const std::string CLIENT_BUILD;
extern const std::string CLIENT_DATE;

//
// database format versioning
//
static const int DATABASE_VERSION = 70507;

//
// network protocol versioning
//

static const int PROTOCOL_VERSION = 60009;

// earlier versions not supported as of Feb 2012, and are disconnected
static const int MIN_PROTO_VERSION = 209;

// In this version, 'getheaders' was introduced.
static const int GETHEADERS_VERSION = 31800;

// disconnect from peers older than this proto version
static const int MIN_PEER_PROTO_VERSION = GETHEADERS_VERSION;

// disconnect buggy clients
static const int MIN_PEER_BUGGY_VERSION = 60006;

// nTime field added to CAddress, starting with this version;
// if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 31402;

// only request blocks from nodes outside this range of versions
static const int NOBLKS_VERSION_START = 60002;
static const int NOBLKS_VERSION_END = 60005;

// BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 60000;

// "mempool" command, enhanced "getdata" behavior starts with this version:
static const int MEMPOOL_GD_VERSION = 60002;

#endif
