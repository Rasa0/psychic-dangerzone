#include "Net.h"

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

    return state;
}

NetState* ClientCreate(char* ip, int port) {

    NetState* state = (NetState*)malloc(sizeof(NetState));

    //Create socket
    state->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    state->port = port;
    memcpy(state->ip, ip, sizeof(state->ip));

    return state;
}

void ClientSend(NetState* state, char data) {

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
        printf("Client send failed\n");
        //Error
    }

}

void ClientRead(NetState* state) {
    DataPacket packet;

    struct sockaddr_in receiv_addr;
    int length;
    socklen_t slen = sizeof(receiv_addr);

    printf("Read");

    length = recvfrom(state->sock, &packet, sizeof(DataPacket), 0, (struct sockaddr*) &receiv_addr, &slen);

    if(length == EAGAIN || length == -1) {
        printf("ing: No data\n");
    } else {
        printf("ing: %c\n", packet.data);
    }
}

char ServerRead(NetState* state) {
    DataPacket packet;

    struct sockaddr_in receiv_addr;
    int length;
    socklen_t slen = sizeof(receiv_addr);

    printf("Read");


    length = recvfrom(state->sock, &packet, sizeof(DataPacket), MSG_DONTWAIT, (struct sockaddr*) &receiv_addr, &slen);

    if(length == EAGAIN || length == -1) {
        printf("ing: No data\n");
        return ' ';
    } else {
        printf("ing: %c\n", packet.data);

        sendto(state->sock, &packet, sizeof(DataPacket), 0, (struct sockaddr*) &receiv_addr, slen);

        return packet.data;
    }

    /*
    if(length) {
        return packet.data;
    } else {
        return ' ';
    }
    */
}


