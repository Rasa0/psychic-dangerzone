#ifndef PACKET_H
#define PACKET_H

#define FLAG_SYN 1
#define FLAG_ACK 2
#define FLAG_FIN 4

typedef struct _DataPacket {
    char data;
} DataPacket;

typedef struct _SeqPacket {
    DataPacket dataPacket;
    unsigned char flags;
    unsigned int sequenceNumber;
} SeqPacket;

typedef struct _CrcPacket {
    SeqPacket sequencePacket;
    unsigned char checksum;
} CrcPacket;

#endif
