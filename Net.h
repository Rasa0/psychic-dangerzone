#ifndef NET_H
#define NET_H

#define DEBUG_PORT 12351

#include "NetState.h"

NetState* ServerCreate(int port);

NetState* ClientCreate(char* ip, int port);

void ClientSend(NetState* state, char data);

char ServerRead(NetState* state);

#endif
