// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_NET_H
#define BITCOIN_NET_H

#include <deque>
#ifndef Q_MOC_RUN
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/signals2/signal.hpp>
#endif
#include <openssl/rand.h>

#ifdef _MSC_VER
#include <stdint.h>
#endif
#ifndef WIN32
#include <arpa/inet.h>
#endif

#ifndef YACOIN_YASSERT_H
#include "Yassert.h"
#endif

#ifndef BITCOIN_MRUSET_H
#include "mruset.h"
#endif

#ifndef BITCOIN_NETBASE_H
#include "netbase.h"
#endif

#ifndef __INCLUDED_PROTOCOL_H__
#include "protocol.h"
#endif

#ifndef _BITCOIN_ADDRMAN
#include "addrman.h"
#endif

#ifndef BITCOIN_HASH_H
#include "hash.h"
#endif

class CRequestTracker;
class CNode;
class CBlockIndex;

// extern CCriticalSection cs_net;

const ::int64_t nSecondsPerMinute = 60, nMinutesPerHour = 60,
                nOneHourInSeconds = nSecondsPerMinute * nMinutesPerHour,
                nTwoHoursInSeconds = 2 * nOneHourInSeconds,
                nTwelveHoursInSeconds = 12 * nOneHourInSeconds,
                nOneDayInSeconds = 24 * nOneHourInSeconds;

const int DEFAULT_HTTP_PORT = 80, DEFAULT_HTTPS_PORT = 443,
          DEFAULT_char_offset = 3, nOneMinuteInSeconds = 60,
          nTenMilliseconds = 10, nOneHundredMilliseconds = 100;

const ::uint32_t nOneMillisecond = 1, nMillisecondsPerSecond = 1000,
                 nSecondsperMinute = 60, nMinutesperHour = 60,
                 nSecondsPerHour = nSecondsperMinute * nMinutesperHour,
                 nHoursPerDay = 24,
                 nSecondsPerDay = nHoursPerDay * nSecondsPerHour;

const ::uint32_t
    nAverageBlocksPerMinute = 1,
    nNumberOfDaysPerYear = 365,
    nNumberOfBlocksPerYear =
        (nAverageBlocksPerMinute * nMinutesperHour * nHoursPerDay *
         nNumberOfDaysPerYear) + // that 1/4 of a day for leap years
        (nAverageBlocksPerMinute * nMinutesperHour * (nHoursPerDay / 4));

const double nInflation = 0.02; // 2%

static const int
#ifdef WIN32
    nDEFAULT_BAN_SCORE = 1000,
    nDEFAULT_BAN_TIME_in_seconds = 3 * nSecondsperMinute;
#else
    nDEFAULT_BAN_SCORE = 100,
    nDEFAULT_BAN_TIME_in_seconds = nHoursPerDay * nSecondsPerHour; // one day
#endif

extern ::int64_t nUpTimeStart;
extern const unsigned int nStakeMaxAge, nOnedayOfAverageBlocks;
extern unsigned int // not const because of fTestNet
    nStakeMinAge,
    nStakeTargetSpacing, nModifierInterval;

inline ::uint64_t ReceiveBufferSize()
{
    return 1000 * GetArg("-maxreceivebuffer", 5 * 1000);
}
inline ::uint64_t SendBufferSize()
{
    return 1000 * GetArg("-maxsendbuffer", 1 * 1000);
}

void AddOneShot(std::string strDest);
bool RecvLine(SOCKET hSocket, std::string &strLine);
//#ifdef WIN32
class CProvider
{
public:
    std::string sDomain, sPriceRatioKey, sApi;
    int nOffset;
    // static const int
    // nOffset = DEFAULT_char_offset,
    //    nPort = DEFAULT_HTTP_PORT;
    int
        //    nOffset;
        nPort;
};
extern std::vector<CProvider> vBTCtoYACProviders;
extern std::vector<CProvider> vUSDtoBTCProviders;

extern void initialize_price_vectors(int &nIndexBtcToYac, int &nIndexUsdToBtc);
extern bool GetMyExternalWebPage1(int &nIndex, std::string &strBuffer,
                                  double &dPrice);
extern bool GetMyExternalWebPage2(int &nIndex, std::string &strBuffer,
                                  double &dPrice);
//#endif
bool GetMyExternalIP(CNetAddr &ipRet);
void AddressCurrentlyConnected(const CService &addr);
// CNode* FindNode(const CNetAddr& ip);
// CNode* FindNode(const CService& ip);
CNode *ConnectNode(CAddress addrConnect, const char *strDest = NULL,
                   ::int64_t nTimeout = 0);
bool OpenNetworkConnection(const CAddress &addrConnect,
                           CSemaphoreGrant *grantOutbound = NULL,
                           const char *strDest = NULL, bool fOneShot = false);
void MapPort();
unsigned short GetListenPort();
bool BindListenPort(const CService &bindAddr,
                    std::string &strError = REF(std::string()));
void StartNode(void *parg);
bool StopNode();

typedef int NodeId;

// Signals for message handling
struct CNodeSignals
{
    boost::signals2::signal<bool (CNode*)> ProcessMessages;
    boost::signals2::signal<bool (CNode*, bool)> SendMessages;
    boost::signals2::signal<void (NodeId, const CNode*)> InitializeNode;
    boost::signals2::signal<void (NodeId)> FinalizeNode;
};
CNodeSignals& GetNodeSignals();

enum
{
    LOCAL_NONE,   // unknown
    LOCAL_IF,     // address a local interface listens on
    LOCAL_BIND,   // address explicit bound to
    LOCAL_UPNP,   // address reported by UPnP
    LOCAL_IRC,    // address reported by IRC (deprecated)
    LOCAL_HTTP,   // address reported by whatismyip.com and similar
    LOCAL_MANUAL, // address explicitly specified (-externalip=)

    LOCAL_MAX
};

extern void clearLocalSocketError(SOCKET hSocket);

void SetLimited(enum Network net, bool fLimited = true);
bool IsLimited(enum Network net);
bool IsLimited(const CNetAddr &addr);
bool AddLocal(const CService &addr, int nScore = LOCAL_NONE);
bool AddLocal(const CNetAddr &addr, int nScore = LOCAL_NONE);
bool SeenLocal(const CService &addr);
bool IsLocal(const CService &addr);
bool GetLocal(CService &addr, const CNetAddr *paddrPeer = NULL);
bool IsReachable(const CNetAddr &addr);
void SetReachable(enum Network net, bool fFlag = true);
CAddress GetLocalAddress(const CNetAddr *paddrPeer = NULL);

enum
{
    MSG_TX = 1,
    MSG_BLOCK,
};

class CRequestTracker
{
public:
    void (*fn)(void *, CDataStream &);
    void *param1;

    explicit CRequestTracker(void (*fnIn)(void *, CDataStream &) = NULL,
                             void *param1In = NULL)
    {
        fn = fnIn;
        param1 = param1In;
    }

    bool IsNull() { return fn == NULL; }
};

/** Thread types */
enum threadId
{
    THREAD_SOCKETHANDLER,
    THREAD_OPENCONNECTIONS,
    THREAD_MESSAGEHANDLER,
    THREAD_MINER,
    THREAD_RPCLISTENER,
    THREAD_UPNP,
    THREAD_DNSSEED,
    THREAD_ADDEDCONNECTIONS,
    THREAD_DUMPADDRESS,
    THREAD_RPCHANDLER,
    THREAD_MINTER,
    THREAD_SCRIPTCHECK,
    THREAD_HASHCALCULATION,

    THREAD_MAX
};

extern bool fClient;
extern bool fDiscover;
extern bool fUseUPnP;
extern ::uint64_t nLocalServices;
extern ::uint64_t nLocalHostNonce;
extern CAddress addrSeenByPeer;
extern boost::array<int, THREAD_MAX> vnThreadsRunning;
extern CAddrMan addrman;

extern std::vector<CNode *> vNodes;
extern CCriticalSection cs_vNodes;
extern std::vector<std::string> vAddedNodes;
extern CCriticalSection cs_vAddedNodes;
extern std::map<CInv, CDataStream> mapRelay;
extern std::deque<std::pair< ::int64_t, CInv> > vRelayExpiration;
extern CCriticalSection cs_mapRelay;
extern std::map<CInv, ::int64_t> mapAlreadyAskedFor;
extern NodeId nLastNodeId;
extern CCriticalSection cs_nLastNodeId;

class CNodeStats
{
public:
    NodeId nodeid;
    ::uint64_t nServices;
    ::int64_t nLastSend;
    ::int64_t nLastRecv;
    ::int64_t nTimeConnected;
    std::string addrName;
    ::int32_t nVersion;
    std::string strSubVer;
    bool fInbound;
    ::int64_t nReleaseTime;
    ::int32_t nStartingHeight;
    ::uint64_t nSendBytes;
    ::uint64_t nRecvBytes;
};

/** Information about a peer */
class CNode
{
public:
    // socket
    ::uint64_t nServices;
    SOCKET hSocket;
    CDataStream vSend;
    CDataStream vRecv;
    ::uint64_t nSendBytes;
    ::uint64_t nRecvBytes;
    CCriticalSection cs_vSend;
    CCriticalSection cs_vRecv;
    ::int64_t nLastSend;
    ::int64_t nLastRecv;
    ::int64_t nLastSendEmpty;
    ::int64_t nTimeConnected;
    ::int32_t nHeaderStart;
    ::uint32_t nMessageStart;
    CAddress addr;
    std::string addrName;
    CService addrLocal;
    ::int32_t nVersion;
    std::string strSubVer;
    bool fOneShot;
    bool fClient;
    bool fInbound;
    bool fNetworkNode;
    bool fSuccessfullyConnected;
    bool fDisconnect;
    CSemaphoreGrant grantOutbound;
    NodeId id;

protected:
    int nRefCount;

    // Denial-of-service detection/prevention
    // Key is IP address, value is banned-until-time
    static std::map<CNetAddr, ::int64_t> setBanned;
    static CCriticalSection cs_setBanned;

public:
    ::int64_t nReleaseTime;
    std::map<uint256, CRequestTracker> mapRequests;
    CCriticalSection cs_mapRequests;
    uint256 hashContinue;
    CBlockIndex *pindexLastGetBlocksBegin;
    uint256 hashLastGetBlocksEnd;
    ::int32_t nStartingHeight;
    bool fStartSync;

    // flood relay
    std::vector<CAddress> vAddrToSend;
    std::set<CAddress> setAddrKnown;
    bool fGetAddr;
    std::set<uint256> setKnown;
    uint256 hashCheckpointKnown; // ppcoin: known sent sync-checkpoint

    // inventory based relay
    mruset<CInv> setInventoryKnown;
    std::vector<CInv> vInventoryToSend;
    CCriticalSection cs_inventory;
    std::multimap< ::int64_t, CInv> mapAskFor;

    CNode(SOCKET hSocketIn, CAddress addrIn, std::string addrNameIn = "",
          bool fInboundIn = false)
        : vSend(SER_NETWORK, MIN_PROTO_VERSION),
          vRecv(SER_NETWORK, MIN_PROTO_VERSION)
    {
        nServices = 0;
        hSocket = hSocketIn;
        nLastSend = 0;
        nLastRecv = 0;
        nSendBytes = 0;
        nRecvBytes = 0;
        nLastSendEmpty = GetTime();
        nTimeConnected = GetTime();
        nHeaderStart = -1;
        nMessageStart = -1;
        addr = addrIn;
        addrName = addrNameIn == "" ? addr.ToStringIPPort() : addrNameIn;
        nVersion = 0;
        strSubVer = "";
        fOneShot = false;
        fClient = false; // set by version message
        fInbound = fInboundIn;
        fNetworkNode = false;
        fSuccessfullyConnected = false;
        fDisconnect = false;
        nRefCount = 0;
        nReleaseTime = 0;
        hashContinue = 0;
        nStartingHeight = -1;
        fGetAddr = false;
        hashCheckpointKnown = 0;
        setInventoryKnown.max_size((size_t)SendBufferSize() / 1000);
        {
            LOCK(cs_nLastNodeId);
            id = nLastNodeId++;
        }

        // Be shy and don't send version until we hear
        if (
            //(hSocket != INVALID_SOCKET) && // TEST<<<<<<<<<<<<<<<<<<<< 1/16/16
            !fInbound)
            PushVersion();
        GetNodeSignals().InitializeNode(GetId(), this);
    }

    ~CNode()
    {
        if (hSocket != INVALID_SOCKET)
        {
            (void)closesocket(hSocket);
            hSocket = INVALID_SOCKET;
        }
        GetNodeSignals().FinalizeNode(GetId());
    }

private:
    // Network usage totals
    static CCriticalSection cs_totalBytesRecv;

    static CCriticalSection cs_totalBytesSent;

    static ::uint64_t nTotalBytesRecv;

    static ::uint64_t nTotalBytesSent;

    CNode(const CNode &);

    CNode &operator=(const CNode &);

public:
    NodeId GetId() const {
      return id;
    }

    int GetRefCount()
    {
        return std::max(nRefCount, 0) + (GetTime() < nReleaseTime ? 1 : 0);
    }

    CNode *AddRef(::int64_t nTimeout = 0)
    {
        if (nTimeout != 0)
            nReleaseTime = std::max(nReleaseTime, GetTime() + nTimeout);
        else
            ++nRefCount;
        return this;
    }

    void Release() { --nRefCount; }

    void AddAddressKnown(const CAddress &addr) { setAddrKnown.insert(addr); }

    void PushAddress(const CAddress &addr)
    {
        // Known checking here is only to save space from duplicates.
        // SendMessages will filter it again for knowns that were added
        // after addresses were pushed.
        if (addr.IsValid() && !setAddrKnown.count(addr))
            vAddrToSend.push_back(addr);
    }

    void AddInventoryKnown(const CInv &inv)
    {
        {
            LOCK(cs_inventory);
            setInventoryKnown.insert(inv);
        }
    }

    void PushInventory(const CInv &inv)
    {
        {
            LOCK(cs_inventory);
            if (!setInventoryKnown.count(inv))
                vInventoryToSend.push_back(inv);
        }
    }

    void AskFor(const CInv &inv)
    {
        // We're using mapAskFor as a priority queue,
        // the key is the earliest time the request can be sent
        ::int64_t &nRequestTime = mapAlreadyAskedFor[inv];
        if (fDebugNet)
            printf("askfor %s   %" PRId64 " (%s)\n", inv.ToString().c_str(),
                   nRequestTime,
                   DateTimeStrFormat("%H:%M:%S", nRequestTime / 1000000).c_str());

        // Make sure not to reuse time indexes to keep things in the same order
        ::int64_t nNow = GetTimeMicros() - 1000000;
        static ::int64_t nLastTime;
        ++nLastTime;
        nNow = std::max(nNow, nLastTime);
        nLastTime = nNow;

        // Each retry is 2 minutes after the last
        nRequestTime = std::max(nRequestTime + 2 * 60 * 1000000, nNow);
        mapAskFor.insert(std::make_pair(nRequestTime, inv));
    }

    void BeginMessage(const char *pszCommand)
    {
        ENTER_CRITICAL_SECTION(cs_vSend);
        if (nHeaderStart != -1)
            AbortMessage();
        nHeaderStart = (::int32_t)vSend.size();
        vSend << CMessageHeader(pszCommand, 0);
        nMessageStart = (::uint32_t)vSend.size();
        if (fDebug)
            printf("sending: %s to node %s", pszCommand, addrName.c_str());
    }

    void AbortMessage()
    {
        if (nHeaderStart < 0)
            return;
        vSend.resize(nHeaderStart);
        nHeaderStart = -1;
        nMessageStart = -1;
        LEAVE_CRITICAL_SECTION(cs_vSend);

        if (fDebug)
            printf("(aborted)\n");
    }

    void EndMessage()
    {
        if (mapArgs.count("-dropmessagestest") &&
            GetRand(atoi(mapArgs["-dropmessagestest"])) == 0)
        {
            printf("dropmessages DROPPING SEND MESSAGE\n");
            AbortMessage();
            return;
        }

        if (nHeaderStart < 0)
            return;

        // Set the size
        ::uint32_t nSize = (::uint32_t)vSend.size() - nMessageStart;

        memcpy((char *)&vSend[nHeaderStart] + CMessageHeader::MESSAGE_SIZE_OFFSET,
               &nSize, sizeof(nSize));

        // Set the checksum
        uint256 hash = Hash(vSend.begin() + nMessageStart, vSend.end());

        ::uint32_t nChecksum = 0;

        memcpy(&nChecksum, &hash, sizeof(nChecksum));
        Yassert(nMessageStart - nHeaderStart >=
                CMessageHeader::CHECKSUM_OFFSET + sizeof(nChecksum));
        memcpy((char *)&vSend[nHeaderStart] + CMessageHeader::CHECKSUM_OFFSET,
               &nChecksum, sizeof(nChecksum));

        if (fDebug)
        {
            printf("(%d bytes)\n", nSize);
        }

        nHeaderStart = -1;
        nMessageStart = -1;
        LEAVE_CRITICAL_SECTION(cs_vSend);
    }

    void EndMessageAbortIfEmpty()
    {
        if (nHeaderStart < 0)
            return;

        int nSize = (int)vSend.size() - nMessageStart;
        if (nSize > 0)
            EndMessage();
        else
            AbortMessage();
    }

    void PushVersion();

    void PushMessage(const char *pszCommand)
    {
        try
        {
            BeginMessage(pszCommand);
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template <typename T1>
    void PushMessage(const char *pszCommand, const T1 &a1)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template <typename T1, typename T2>
    void PushMessage(const char *pszCommand, const T1 &a1, const T2 &a2)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template <typename T1, typename T2, typename T3>
    void PushMessage(const char *pszCommand, const T1 &a1, const T2 &a2,
                     const T3 &a3)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2 << a3;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template <typename T1, typename T2, typename T3, typename T4>
    void PushMessage(const char *pszCommand, const T1 &a1, const T2 &a2,
                     const T3 &a3, const T4 &a4)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2 << a3 << a4;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    void PushMessage(const char *pszCommand, const T1 &a1, const T2 &a2,
                     const T3 &a3, const T4 &a4, const T5 &a5)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2 << a3 << a4 << a5;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5,
              typename T6>
    void PushMessage(const char *pszCommand, const T1 &a1, const T2 &a2,
                     const T3 &a3, const T4 &a4, const T5 &a5, const T6 &a6)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2 << a3 << a4 << a5 << a6;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5,
              typename T6, typename T7>
    void PushMessage(const char *pszCommand, const T1 &a1, const T2 &a2,
                     const T3 &a3, const T4 &a4, const T5 &a5, const T6 &a6,
                     const T7 &a7)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2 << a3 << a4 << a5 << a6 << a7;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5,
              typename T6, typename T7, typename T8>
    void PushMessage(const char *pszCommand, const T1 &a1, const T2 &a2,
                     const T3 &a3, const T4 &a4, const T5 &a5, const T6 &a6,
                     const T7 &a7, const T8 &a8)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5,
              typename T6, typename T7, typename T8, typename T9>
    void PushMessage(const char *pszCommand, const T1 &a1, const T2 &a2,
                     const T3 &a3, const T4 &a4, const T5 &a5, const T6 &a6,
                     const T7 &a7, const T8 &a8, const T9 &a9)
    {
        try
        {
            BeginMessage(pszCommand);
            vSend << a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8 << a9;
            EndMessage();
        }
        catch (...)
        {
            AbortMessage();
            throw;
        }
    }

    void PushRequest(const char *pszCommand, void (*fn)(void *, CDataStream &),
                     void *param1)
    {
        uint256 hashReply;

        RAND_bytes((unsigned char *)&hashReply, sizeof(hashReply));

        {
            LOCK(cs_mapRequests);
            mapRequests[hashReply] = CRequestTracker(fn, param1);
        }

        PushMessage(pszCommand, hashReply);
    }

    template <typename T1>
    void PushRequest(const char *pszCommand, const T1 &a1,
                     void (*fn)(void *, CDataStream &), void *param1)
    {
        uint256 hashReply;
        RAND_bytes((unsigned char *)&hashReply, sizeof(hashReply));

        {
            LOCK(cs_mapRequests);
            mapRequests[hashReply] = CRequestTracker(fn, param1);
        }

        PushMessage(pszCommand, hashReply, a1);
    }

    template <typename T1, typename T2>
    void PushRequest(const char *pszCommand, const T1 &a1, const T2 &a2,
                     void (*fn)(void *, CDataStream &), void *param1)
    {
        uint256 hashReply;
        RAND_bytes((unsigned char *)&hashReply, sizeof(hashReply));

        {
            LOCK(cs_mapRequests);
            mapRequests[hashReply] = CRequestTracker(fn, param1);
        }

        PushMessage(pszCommand, hashReply, a1, a2);
    }

    void PushGetBlocks(CBlockIndex *pindexBegin, uint256 hashEnd);
    bool IsSubscribed(unsigned int nChannel);
    void Subscribe(unsigned int nChannel, unsigned int nHops = 0);
    void CancelSubscribe(unsigned int nChannel);
    void CloseSocketDisconnect();
    void Cleanup();

    // Denial-of-service detection/prevention
    // The idea is to detect peers that are behaving
    // badly and disconnect/ban them, but do it in a
    // one-coding-mistake-won't-shatter-the-entire-network
    // way.
    // IMPORTANT:  There should be nothing I can give a
    // node that it will forward on that will make that
    // node's peers drop it. If there is, an attacker
    // can isolate a node and/or try to split the network.
    // Dropping a node for sending stuff that is invalid
    // now but might be valid in a later version is also
    // dangerous, because it can cause a network split
    // between nodes running old code and nodes running
    // new code.
    static void ClearBanned(); // needed for unit testing
    static bool IsBanned(CNetAddr ip);
    static bool Ban(const CNetAddr &ip);
    void copyStats(CNodeStats &stats);
    // Network stats
    static void RecordBytesRecv(::uint64_t bytes);
    static void RecordBytesSent(::uint64_t bytes);

    static ::uint64_t GetTotalBytesRecv();
    static ::uint64_t GetTotalBytesSent();
};

inline void RelayInventory(const CInv &inv)
{
    // Put on lists to offer to the other nodes
    {
        LOCK(cs_vNodes);
        BOOST_FOREACH (CNode *pnode, vNodes)
            pnode->PushInventory(inv);
    }
}

class CTransaction;
void RelayTransaction(const CTransaction &tx, const uint256 &hash);
void RelayTransaction(const CTransaction &tx, const uint256 &hash,
                      const CDataStream &ss);

// just for test purposes
// void do_https_test();
//_____________________________________________________________________________
#endif
