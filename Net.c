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

    return state;
}

NetState* ClientCreate(char* ip, int port) {

    NetState* state = (NetState*)malloc(sizeof(NetState));
    //struct in_addr addr;

    //Create socket
    state->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    state->port = port;
    memcpy(state->ip, ip, sizeof(state->ip));
    //inet_aton(ip , &addr);
    //state->netIp = addr.s_addr;

    memset((char *) &state->remoteAddr, 0, sizeof(struct sockaddr_in));
    state->remoteAddr.sin_family = AF_INET;
    state->remoteAddr.sin_port = htons(state->port);

    if (inet_aton(state->ip , &state->remoteAddr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    // TODO FIX. Nahh. The size of the window is obtained in the handshake
    //InitSlidingWindow(&state->slidingWindow);

    // Perform handshake
    if(!_ClientHandShake(state)) {
        // Failed connection?
        close(state->sock);
        free(state);
        state = 0;
    }

    return state;
}

void ServerClose(NetState* state) {
    close(state->sock);
    free(state);
}

void ClientClose(NetState* state) {

    // Prepare the timeout timer
    ResetGlobalTimeout(state);
    state->state = STATE_TEARDOWN;

    _ClientSendFin(state);

    // Timer for resend timeout
    time_t timeout;
    time_t currtime;
    time(&timeout);

    //Listen for syn ack.
    while(1) {

        if(TestGlobalTimeout(state)) {
            // Global time out triggered?
            printf("Global timeout while performing handshake\n");
            break;
        }

        // Listen for response
        SeqPacket received;
        if(_ReadPacket(state, &received)) {

            // Reveived fin from server?
            if(received.flags ==  (FLAG_FIN)) {
                printf("Reveiced Fin: %d Ack: %d\n",received.sequenceNumber, received.ackSequenceNumber);
                // Is this the Fin we want?
                //if(received.ackSequenceNumber == state->nextSequenceNumber) {

                    // Tell the server that the fin was received
                    _ClientSendAck(state); // NOTE verify that this send the correct ack

                    // Close connection
                    close(state->sock);
                    free(state);
                    return;
                //}

            }
        }

        time(&currtime);

        if(difftime(currtime, timeout) > HANDSHAKE_RESEND_TIMEOUT) {
             printf("Handshake resend triggered\n");
             time(&timeout);
            _ClientSendFin(state);
        }

        // Short sleep so we dont burn the CPU =)
        usleep(5 * 1000);
    }

    // Close connection
    close(state->sock);
    free(state);
    return;
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
        {
            // Timer for resend timeout
            time_t timeout;
            time_t currtime;
            time(&timeout);

            //Listen for syn ack.
            while(1) {

                if(TestGlobalTimeout(state)) {
                    // Global time out triggered?
                    printf("Global timeout while performing handshake\n");
                    state->state = STATE_NOT_CONNECTED;
                    return POLL_STATUS_GLOBAL_TIMEOUT;
                }

                // Listen for response
                SeqPacket received;
                if(_ReadPacket(state, &received)) {

                    // Reveived ack from client?
                    if(received.flags ==  (FLAG_ACK)) {
                        printf("Reveiced Ack: %d\n", received.ackSequenceNumber);
                        // Is this the Ack we want?
                        if(received.ackSequenceNumber == state->nextSequenceNumber) {

                            printf("Received correct ack!\n");

                            // TODO Init window now
                            //InitSlidingWindow(state->)

                            // Go to connected state
                            state->state = STATE_CONNECTED;
                            return POLL_STATUS_NOTHING;
                        }

                    }
                }

                time(&currtime);

                if(difftime(currtime, timeout) > HANDSHAKE_RESEND_TIMEOUT) {
                     printf("Handshake resend triggered\n");
                     time(&timeout);
                    _ServerSendSynAck(state);
                }

                // Short sleep so we dont burn the CPU =)
                usleep(5 * 1000);
            }
        }

        break;
    case STATE_TEARDOWN:

        // NOTE should this be here?

        break;
    case STATE_CONNECTED:


        //usleep(200 * 1000);
        // TODO Fix

        break;
    }

    return POLL_STATUS_NOTHING;
}

void ClientSendData(NetState* state, char data) {

    // TODO send to sliding window function



    // TODO fix checksum





}

int _ReadUDP(NetState* state, CrcPacket* packet) {

    CrcPacket tempPacket;

    struct sockaddr_in receiv_addr;
    int length;
    socklen_t slen = sizeof(receiv_addr);

    length = recvfrom(state->sock, &tempPacket, sizeof(CrcPacket), MSG_DONTWAIT, (struct sockaddr*) &receiv_addr, &slen);

    if(!(length == EAGAIN || length == -1)) {
        printf("Received UDP from %d\n", htons(receiv_addr.sin_port));
        memcpy(packet, &tempPacket, sizeof(CrcPacket));

        return 1;
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

// Used to read the first Syn message from the client. To catch target ip and port
int _ServerReadFirstPacket(NetState* state, SeqPacket* packet) {

    int validPacket = 0;
    CrcPacket tempPacket;

    //struct sockaddr_in receiv_addr;
    int length;
    socklen_t slen = sizeof(state->remoteAddr);

    length = recvfrom(state->sock, &tempPacket, sizeof(CrcPacket), MSG_DONTWAIT, (struct sockaddr*) &state->remoteAddr, &slen);

    if(!(length == EAGAIN || length == -1)) {
        validPacket = 1;

        char* ip;
        ip = inet_ntoa(state->remoteAddr.sin_addr);
        memcpy(state->ip, ip, 16);
        state->netIp = state->remoteAddr.sin_addr.s_addr;
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


/*
    struct sockaddr_in targetAddr;

    memset((char *) &targetAddr, 0, sizeof(struct sockaddr_in));
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(state->port);
    //targetAddr.sin_addr.s_addr = state->netIp;

    if(!inet_aton(state->ip , &targetAddr.sin_addr)) {
        printf("inet_aton failed\n");
    }
*/



    if (sendto(state->sock, packet, sizeof(CrcPacket), 0, (struct sockaddr*) &state->remoteAddr, sizeof(state->remoteAddr)) == -1)
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

void _ClientSendFin(NetState* state) {
    SeqPacket packet;
    packet.dataPacket.data = 0;
    packet.flags = FLAG_FIN;
    // Mark the Fin with a sequence number
    packet.sequenceNumber = state->nextSequenceNumber;

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

    // Tell the client a window size and a connection id
    windowSize = packet.windowSize = SLIDING_WINDOW_SIZE;
    connectionId = packet.connectionId = rand() % 32;

    _SendPacket(state, &packet);
}

int _ClientHandShake(NetState* state) {
    // Prepare the timeout timer
    ResetGlobalTimeout(state);
    state->state = STATE_THREEWAY_HANDSHAKE;

    _ClientSendSyn(state);

    // Timer for resend timeout
    time_t timeout;
    time_t currtime;
    time(&timeout);

    //Listen for syn ack.
    while(1) {

        if(TestGlobalTimeout(state)) {
            // Global time out triggered?
            printf("Global timeout while performing handshake\n");
            break;
        }

        // Listen for response
        SeqPacket received;
        if(_ReadPacket(state, &received)) {

            // Reveived syn ack from server?
            if(received.flags ==  (FLAG_SYN | FLAG_ACK)) {
                printf("Reveiced Syn: %d Ack: %d\n",received.sequenceNumber, received.ackSequenceNumber);
                // Is this the Ack we want?
                if(received.ackSequenceNumber == state->nextSequenceNumber) {
                    // Rember the sequenc number of the servers Syn
                    state->nextRemoteSequenceNumber = received.sequenceNumber;

                    // Remember window size and connection id determined by server
                    state->windowSize = received.windowSize;
                    state->connectionId = received.connectionId;

                    // TODO Init window now
                    //InitSlidingWindow(state->)

                    _ClientSendAck(state);

                    // Go to connected state
                    state->state = STATE_CONNECTED;
                    return 1;
                }
            }
        }

        time(&currtime);

        if(difftime(currtime, timeout) > HANDSHAKE_RESEND_TIMEOUT) {
             printf("Handshake resend triggered\n");
             time(&timeout);
            _ClientSendSyn(state);
        }

        // Short sleep so we dont burn the CPU =)
        usleep(5 * 1000);
    }

    // TODO REMEMBER TO LISTEN FOR resent Syn - Ack
    // And remember that the server might assign a new connection id to.
    state->state = STATE_NOT_CONNECTED;
    return 0;
}

