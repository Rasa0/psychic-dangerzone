#ifndef NET_H
#define NET_H

#define DEBUG_PORT 123534

#define POLL_STATUS_NOTHING 0
#define POLL_STATUS_GLOBAL_TIMEOUT 1
#define POLL_STATUS_MESSAGE_RECEIVED 2

#include "NetState.h"
#include "NetShared.h"
#include "Packet.h"

NetState* ServerCreate(int port);
NetState* ClientCreate(char* ip, int port);

int ClientPoll(NetState* state);

//Send a data packet to the server
void ClientSendData(NetState* state, char data);

int ServerPoll(NetState* state);

void ServerSendTEST(NetState* state, char data);

void _ReadUDP(NetState* state, CrcPacket* packet);

// Adds the checksum and puts the packet though corruption and queue module
void _SendPacket(NetState* state, SeqPacket* packet);
//Performs the real UDP send
void _SendUDP(NetState* state, CrcPacket* packet);

void _ClientHandShake(NetState* state);
void _SeverHandShake(NetState* state);

#endif
