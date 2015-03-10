#ifndef NET_STATE_H
#define NET_STATE_H

typedef struct _NetState {

    int sock;
    int port;
    char ip[16];

} NetState;

#endif
