#include "Net.h"

#include "Checksum.h"
#include "NetState.h"
#include "NetShared.h"
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
#include <time.h>


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

    // TODO FIX
    //InitSlidingWindow(&state->slidingWindow);

    return state;
}

NetState* ClientCreate(char* ip, int port) {

    NetState* state = (NetState*)malloc(sizeof(NetState));
    struct in_addr addr;

    //Create socket
    state->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    state->port = port;
    memcpy(state->ip, ip, sizeof(state->ip));
    inet_aton(ip , &addr);
    state->netIp = addr.s_addr;


    // TODO FIX
    //InitSlidingWindow(&state->slidingWindow);

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

    //DataPacket packet;
    struct sockaddr_in targetAddr;

    memset((char *) &targetAddr, 0, sizeof(struct sockaddr_in));
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(state->port);
    targetAddr.sin_addr.s_addr = state->netIp;

    //packet.data = data;

    // DURRRRR WARNING
    data = 4;

    // TODO send to sliding window function



    // TODO fix checksum





}

int ServerPoll(NetState* state) {
    SeqPacket packet;

    switch(state->state) {
    case STATE_NOT_CONNECTED:

        if(_ServerReadFirstPacket(state, &packet)) {

            if(packet.flags == FLAG_SYN) {
                ResetGlobalTimeout(state);

                printf("Received Syn: %d\n", packet.sequenceNumber);
                state->nextRemoteSequenceNumber = packet.sequenceNumber;

                _ServerSendSynAck(state);




                printf("Switching state to handshake\n");
                state->state = STATE_THREEWAY_HANDSHAKE;
            } else {
                printf("Expected Syn but received: %d\n", packet.flags);
            }
        }

        return POLL_STATUS_NOTHING;
        break;
    case STATE_THREEWAY_HANDSHAKE:


        if(TestGlobalTimeout(state)) {
            // Connection closed. Return to not connected state
            state->state = STATE_NOT_CONNECTED;
            return POLL_STATUS_GLOBAL_TIMEOUT;
        } else {


            // TODO Fix

        }



        break;
    case STATE_CONNECTED:

        // TODO Fix

        break;
    }

    return POLL_STATUS_NOTHING;
}

int _ReadUDP(NetState* state, CrcPacket* packet) {

    CrcPacket tempPacket;

    struct sockaddr_in receiv_addr;
    int length;
    socklen_t slen = sizeof(receiv_addr);

    length = recvfrom(state->sock, &tempPacket, sizeof(CrcPacket), MSG_DONTWAIT, (struct sockaddr*) &receiv_addr, &slen);

    if(!(length == EAGAIN || length == -1)) {
        // Verify ip and port
        if(state->netIp == receiv_addr.sin_addr.s_addr && state->port == htons(receiv_addr.sin_port)) {
            printf("Received UDP with flags: %d\n", tempPacket.sequencePacket.flags);
            memcpy(packet, &tempPacket, sizeof(CrcPacket));
            return 1;
        } else {
            printf("Message received from unknown source\n");
        }
    }

    return 0;
}

int _ReadPacket(NetState* state, SeqPacket* packet) {

    CrcPacket tempPacket;
    if(_ReadUDP(state, &tempPacket)) {

        // CRC valid?
        if(!CheckChecksum((unsigned char*)(&tempPacket), sizeof(tempPacket))) {

            memcpy(packet, &(tempPacket.sequencePacket), sizeof(SeqPacket));

            printf("UPD message\n");
            return 1;
        } else {
             printf("UPD message but was corrupt %d  %d\n", tempPacket.sequencePacket.flags, tempPacket.checksum);
        }
    } else {
        printf("No UPD message\n");
    }

    return 0;
}

// Used to read the first Syn message from the clinet. To catch target ip and port
int _ServerReadFirstPacket(NetState* state, SeqPacket* packet) {

    int validPacket = 0;
    CrcPacket tempPacket;

    struct sockaddr_in receiv_addr;
    int length;
    socklen_t slen = sizeof(receiv_addr);

    length = recvfrom(state->sock, &tempPacket, sizeof(CrcPacket), MSG_DONTWAIT, (struct sockaddr*) &receiv_addr, &slen);

    if(!(length == EAGAIN || length == -1)) {
        validPacket = 1;

        char* ip;
        ip = inet_ntoa(receiv_addr.sin_addr);
        memcpy(state->ip, ip, 16);
        state->netIp = receiv_addr.sin_addr.s_addr;
        state->port = ntohs(state->port);
    }

    if(validPacket) {
        // CRC valid?
        if(!CheckChecksum((unsigned char*)(&tempPacket), sizeof(tempPacket))) {

            memcpy(packet, &(tempPacket.sequencePacket), sizeof(SeqPacket));

            printf("Received initial message from %s:%d\n", state->ip, state->port);
            return 1;
        } else {
             printf("UPD message but was corrupt %d  %d\n", tempPacket.sequencePacket.flags, tempPacket.checksum);
             return 0;
        }
    } else {
        printf("No Initial message\n");
    }

    return validPacket;
}

//Performs the real UDP send.
void _SendUDP(NetState* state, CrcPacket* packet) {

    struct sockaddr_in targetAddr;

    memset((char *) &targetAddr, 0, sizeof(struct sockaddr_in));
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(state->port);
    //targetAddr.sin_addr.s_addr = state->netIp;

    if(!inet_aton(state->ip , &targetAddr.sin_addr)) {
        printf("inet_aton failed\n");
    }




    if (sendto(state->sock, packet, sizeof(CrcPacket), 0, (struct sockaddr*) &targetAddr, sizeof(targetAddr)) == -1)
    {
        printf("UDP Send failed\n");
        exit(1);
    } else {
        printf("Sent UDP to: %s:%d\n", state->ip, state->port);
    }

    // Free the CRC packet
    free(packet);
}

// Adds the checksum and puts the packet though corruption and queue module
void _SendPacket(NetState* state, SeqPacket* packet) {

    CrcPacket* crcPacket = malloc(sizeof(CrcPacket));
    crcPacket->checksum = 0;
    crcPacket->sequencePacket = *packet;

    // Apply checksum
    crc checksum = GetChecksum((unsigned char*)crcPacket, sizeof(CrcPacket));
    crcPacket->checksum = checksum;

    // TODO link in actual corruption and queue module
    _SendUDP(state, crcPacket);
}

void _ClientSendSyn(NetState* state) {
    SeqPacket packet;
    packet.dataPacket.data = 0;
    packet.flags = FLAG_SYN;
    // Syn with our next sequence number
    packet.sequenceNumber = state->nextSequenceNumber;

    printf("Send Syn: %d\n", packet.sequenceNumber);
    _SendPacket(state, &packet);
}

void _ClientSendAck(NetState* state) {
    SeqPacket packet;
    packet.dataPacket.data = 0;
    packet.flags = FLAG_ACK;
    // Ack the remote sequence number received from the previous received Syn
    packet.ackSequenceNumber = state->nextRemoteSequenceNumber;

    _SendPacket(state, &packet);
}

void _ServerSendSynAck(NetState* state) {
    SeqPacket packet;
    packet.dataPacket.data = 0;
    packet.flags = FLAG_SYN | FLAG_ACK;
    // Syn with our next sequence number
    packet.sequenceNumber = state->nextSequenceNumber;
    // Ack the remote sequence number received from the previous received Syn
    packet.ackSequenceNumber = state->nextRemoteSequenceNumber;

    _SendPacket(state, &packet);
}

void _ClientHandShake(NetState* state) {
    // Prepare the timeout timer
    ResetGlobalTimeout(state);
    state->state = STATE_THREEWAY_HANDSHAKE;

    _ClientSendSyn(state);

    //time_t globalTimeout;

    while(1) {
        usleep(1000* 1000);

        SeqPacket received;
        if(_ReadPacket(state, &received)) {

            printf("Some message received...\n");

            // Reveived syn ack from server?
            if(received.flags ==  (FLAG_SYN | FLAG_ACK)) {
                printf("Reveiced Syn: %d Ack: %d\n",received.sequenceNumber, received.ackSequenceNumber);
                // Is this the Ack we want?
                if(received.ackSequenceNumber == state->nextSequenceNumber) {
                    // Rember the sequenc number of the servers Syn
                    state->nextRemoteSequenceNumber = received.sequenceNumber;

                    _ClientSendAck(state);

                    // Go to connected state
                    break;
                }

            }
        }
    }


    // TODO REMEMBER TO LISTEN FOR resent Syn - Ack

}

void _SeverHandShake(NetState* state) {

}
