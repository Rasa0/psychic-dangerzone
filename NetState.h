#ifndef NET_STATE_H
#define NET_STATE_H

#include "SlidingWindow.h"

#include <time.h>

#define STATE_NOT_CONNECTED 0
#define STATE_THREEWAY_HANDSHAKE 1
#define STATE_CONNECTED 2




typedef struct _NetState {

    int sock;
    int port;
    char ip[16];

    unsigned int nextSequenceNumber;

    unsigned int state;

    time_t globalTimeout;

    SlidingWindow slidingWindow;

} NetState;

#endif
