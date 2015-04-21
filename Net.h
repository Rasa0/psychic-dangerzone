#ifndef NET_H
#define NET_H

#define DEBUG_PORT 1234
#define CLIENT_CONNECTION_ID 1

#define POLL_STATUS_NOTHING 0
#define POLL_STATUS_GLOBAL_TIMEOUT 1
#define POLL_STATUS_MESSAGE_RECEIVED 2

#define HANDSHAKE_RESEND_TIMEOUT 1.0

#include "NetState.h"
#include "NetShared.h"
#include "Packet.h"

NetState* ServerCreate(int port);
NetState* ClientCreate(char* ip, int port);

void ServerClose(NetState* state);
void ClientClose(NetState* state);

int ClientPoll(NetState* state);

int ServerPoll(NetState* state);

//Send a data packet to the server
void ClientSendData(NetState* state, char data);

//Return 1 if a packet was received, and put it in packet
int _ReadUDP(NetState* state, CrcPacket* packet);

// Return 1 if a packet was received. This also perform CRC validation. And removes the message if
// CRC is invalid
int _ReadPacket(NetState* state, SeqPacket* packet);

// Used to read the first Syn message from the clinet. To catch target ip and port
int _ServerReadFirstPacket(NetState* state, SeqPacket* packet);

//Performs the real UDP send
void _SendUDP(NetState* state, CrcPacket* packet);
// Adds the checksum and puts the packet though corruption and queue module
void _SendPacket(NetState* state, SeqPacket* packet);


void _ClientSendSyn(NetState* state);
void _ClientSendAck(NetState* state);
void _ClientSendFin(NetState* state);

void _ServerSendSynAck(NetState* state);

// Return 1 if handshake succeded
int _ClientHandShake(NetState* state);

#endif
