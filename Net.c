#include "Net.h"

#include "Checksum.h"
#include "NetState.h"
#include "Packet.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>


NetState* ServerCreate(int port) {

    NetState* state = (NetState*)malloc(sizeof(NetState));

    //Create socket
    state->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    state->port = port;

    struct sockaddr_in addr;
    // zero out the structure
    memset((char *) &addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(state->sock , (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
        printf("Server Could not bind\n");
    }

    state->state = STATE_NOT_CONNECTED;

    InitSlidingWindow(&state->slidingWindow);

    return state;
}

NetState* ClientCreate(char* ip, int port) {

    NetState* state = (NetState*)malloc(sizeof(NetState));

    //Create socket
    state->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    state->port = port;
    memcpy(state->ip, ip, sizeof(state->ip));

    InitSlidingWindow(&state->slidingWindow);

    // Perform handshake
    _ClientHandShake(state);

    return state;
}

int ClientPoll(NetState* state) {


    switch(state->state) {
    case STATE_NOT_CONNECTED:
        return POLL_STATUS_NOTHING;
        break;
    case STATE_THREEWAY_HANDSHAKE:

        if(TestGlobalTimeout(state)) {
            // TODO handle propper close

            return POLL_STATUS_GLOBAL_TIMEOUT;
        } else {

        }

        // TODO Fix

        break;
    case STATE_CONNECTED:

        // TODO Fix. Check for UPD message

        break;
    }

    return POLL_STATUS_NOTHING;
}

void ClientSendData(NetState* state, char data) {

    DataPacket packet;
    struct sockaddr_in targetAddr;

    memset((char *) &targetAddr, 0, sizeof(struct sockaddr_in));
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(state->port);

    if (inet_aton(state->ip , &targetAddr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    packet.data = data;


    // TODO send to sliding window function



    // TODO fix checksum





}

int ServerPoll(NetState* state) {
    switch(state->state) {
    case STATE_NOT_CONNECTED:
        // TODO listen for initial message from UDP "Still a CRCPacket"

        return POLL_STATUS_NOTHING;
        break;
    case STATE_THREEWAY_HANDSHAKE:

        if(TestGlobalTimeout(state)) {
            // TODO handle propper close

            return POLL_STATUS_GLOBAL_TIMEOUT;
        } else {

        }

        // TODO Fix

        break;
    case STATE_CONNECTED:

        // TODO Fix

        break;
    }

    return POLL_STATUS_NOTHING;
}

void ServerSendTEST(NetState* state, char data) {
    DataPacket packet;
    struct sockaddr_in targetAddr;

    memset((char *) &targetAddr, 0, sizeof(struct sockaddr_in));
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(state->port);

    if (inet_aton(state->ip , &targetAddr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    packet.data = data;

    if (sendto(state->sock, &packet, sizeof(DataPacket), 0, (struct sockaddr*) &targetAddr, sizeof(targetAddr)) == -1)
    {
        printf("Server send failed\n");
        //Error
    }
}

void _ReadUDP(NetState* state, CrcPacket* packet) {

    CrcPacket tempPacket;

    struct sockaddr_in receiv_addr;
    int length;
    socklen_t slen = sizeof(receiv_addr);

    length = recvfrom(state->sock, &tempPacket, sizeof(CrcPacket), MSG_DONTWAIT, (struct sockaddr*) &receiv_addr, &slen);

    if(!(length == EAGAIN || length == -1)) {
        memcpy(packet, &tempPacket, sizeof(CrcPacket));
    }
}

// Adds the checksum and puts the packet though corruption and queue module
void _SendPacket(NetState* state, SeqPacket* packet) {

    CrcPacket* crcPacket = malloc(sizeof(CrcPacket));
    crcPacket->checksum = 0;
    crcPacket->sequencePacket = *packet;

    // Apply checksum
    crc chekcsum = GetChecksum((unsigned char*)crcPacket, sizeof(CrcPacket));
    crcPacket->checksum = chekcsum;


    // TODO link in actual corruption and queue module
    _UDPSend(state, packet);
}

//Performs the real UDP send.
void _UDPSend(NetState* state, CrcPacket* packet) {

    struct sockaddr_in targetAddr;

    memset((char *) &targetAddr, 0, sizeof(struct sockaddr_in));
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(state->port);

    if (inet_aton(state->ip , &targetAddr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }


    if (sendto(state->sock, &packet, sizeof(CrcPacket), 0, (struct sockaddr*) &targetAddr, sizeof(targetAddr)) == -1)
    {
        printf("UDP Send failed\n");
        exit(1);
    }

    // Free the CRC packet
    free(packet);
}

void _ClientHandShake(NetState* state) {
    // Prepare the timeout timer
    ResetGlobalTimeout(state);
    state->state = STATE_THREEWAY_HANDSHAKE;

    SeqPacket packet;
    packet.dataPacket.data = 0;

    packet.flags = FLAG_SYN;
    packet.sequenceNumber = GetNextSequenceNumber(state);

    SendPacket()


    //FLAG_SYN

    /*
    typedef struct _DataPacket {
        char data;
    } DataPacket;

    typedef struct _SeqPacket {
        DataPacket dataPacket;
        unsigned int sequenceNumber;
    } SeqPacket;

    typedef struct _CrcPacket {
        SeqPacket sequencePacket;
        unsigned char checksum;
    } CrcPacket;
    */

}

void _SeverHandShake(NetState* state) {

}
