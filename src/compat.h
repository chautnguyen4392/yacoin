// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef _BITCOIN_COMPAT_H
#define _BITCOIN_COMPAT_H 1

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN 1
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _MSC_VER
#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE 1024 // max number of fds in fd_set
#endif
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <cstdint>  // For uint32_t
#include <cstdio>   // For printf
#else
#include <sys/types.h>
#include <sys/socket.h>
#ifdef ANDROID
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <unistd.h>

typedef u_int SOCKET;
#endif

#ifdef WIN32
#define MSG_NOSIGNAL        0
#define MSG_DONTWAIT        0
typedef int socklen_t;
#else
#include "errno.h"
#define WSAGetLastError()   errno
#define WSAEINVAL           EINVAL
#define WSAEALREADY         EALREADY
#define WSAEWOULDBLOCK      EWOULDBLOCK
#define WSAEMSGSIZE         EMSGSIZE
#define WSAEINTR            EINTR
#define WSAEINPROGRESS      EINPROGRESS
#define WSAEADDRINUSE       EADDRINUSE
#define WSAENOTSOCK         EBADF
#define INVALID_SOCKET      (SOCKET)(~0)
#define SOCKET_ERROR        -1
#endif

#if HAVE_DECL_STRNLEN == 0
size_t strnlen( const char *start, size_t max_len);
#endif // HAVE_DECL_STRNLEN

extern bool 
    fDebug;

inline int myclosesocket(SOCKET& hSocket)
{
#ifdef WIN32
    static ::uint32_t 
        nHowManyValidCloses = 0,
        nHowManyCloses = 0;

    ++nHowManyCloses;
#endif
    if (hSocket == INVALID_SOCKET)
        return WSAENOTSOCK;
#ifdef WIN32
    ++nHowManyValidCloses;
    int 
        ret = closesocket(hSocket);

    if( fDebug )
    {
        if( ret != 0 )
            (void)printf(
                        "\n"
                        "myclosesocket (count, valid %u, %u) returns %d"
                        "\n"
                        "\n"
                        , nHowManyCloses
                        , nHowManyValidCloses
                        , ret
                        );
    }
    //int ret = 0;    //closesocket(hSocket);
#else
    int ret = close(hSocket);
#endif
    hSocket = INVALID_SOCKET;
    return ret;
}
#define closesocket(s)      myclosesocket(s)


#endif
