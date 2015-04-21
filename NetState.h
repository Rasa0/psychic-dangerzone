#ifndef NET_STATE_H
#define NET_STATE_H

#include "SlidingWindow.h"

#include <netinet/in.h>
#include <time.h>

#define STATE_NOT_CONNECTED 0
#define STATE_THREEWAY_HANDSHAKE 1
#define STATE_CONNECTED 2
#define STATE_TEARDOWN 3




typedef struct _NetState {

    int sock;
    int port;
    char ip[16];
    in_addr_t netIp;

    struct sockaddr_in remoteAddr;

    unsigned int windowSize;
    unsigned int connectionId;

    unsigned int nextSequenceNumber;
    unsigned int nextRemoteSequenceNumber;

    unsigned int state;

    time_t globalTimeout;

    //SlidingWindow slidingWindow;

} NetState;

#endif
