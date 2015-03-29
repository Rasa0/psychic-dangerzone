#ifndef PACKET_H
#define PACKET_H

#define FLAG_SYN 1
#define FLAG_ACK 2
#define FLAG_FIN 4

typedef struct _DataPacket {
    char data;
    char padding[3]; // Padding to align memory to prevent wrong Checksum calculation
} DataPacket;

typedef struct _SeqPacket {
    DataPacket dataPacket;
    unsigned int flags;
    unsigned int sequenceNumber;    // Used for sequence number and SYN
    unsigned int ackSequenceNumber; // Used when ACK sent for sequence number
} SeqPacket;

typedef struct _CrcPacket {
    SeqPacket sequencePacket;
    char padding[3]; // Padding to align memory to prevent wrong Checksum calculation
    unsigned char checksum;
} CrcPacket;

#endif
