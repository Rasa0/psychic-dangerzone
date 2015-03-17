#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "Net.h"
#include "Checksum.h"

// Thread that keep polling the network, eventhough client input is blocking
void* PollThread(void* args) {
    NetState* state = (NetState*)args;

    while(1) {
        ClientPoll(state);
        usleep(10 * 1000);
    }
}

typedef struct _Meh {
    unsigned int stuffMEH;

    unsigned char padding[3];
    unsigned char check;
} Meh;


int main(void)
{


    NetState* state = ClientCreate("127.0.0.1", DEBUG_PORT);
    pthread_t thread;
    pthread_attr_t threadAttr;



    if(state) {
        printf("Connection Complete!\n");
        pthread_attr_init(&threadAttr);

        if(pthread_create(&thread,&threadAttr, &PollThread, (void*)state) < 0) {
            printf("Couldn't create thread\n");

            // TODO fix graceful exit
        } else {

            while(1) {

                char c = getchar();
                fflush(stdin);

                ClientSendData(state, c);

                //ClientReadTEST(state);
            }
    }
    } else {
        printf("Could not connect to server!\n");
    }


/*
    printf("Meh test\n----------\n");

    printf("Size of Meh: %d\n", sizeof(Meh));

    Meh meh;

    meh.check = 0;
    crc sum = GetChecksum((unsigned char*)&meh, sizeof(Meh));
    meh.check = sum;

    printf("%d\n", CheckChecksum((unsigned char*)&meh, sizeof(Meh)));


    printf("\nPack test\n----------\n");


    printf("Sizeof Data: %d \n", sizeof(DataPacket));
    printf("Sizeof Seq : %d \n", sizeof(SeqPacket));
    printf("Sizeof Crc : %d \n", sizeof(CrcPacket));

    CrcPacket pack;

pack.padding[0] = 4;
    sum = GetChecksum((unsigned char*)&pack, sizeof(CrcPacket));
    pack.checksum = sum;

    printf("After sum: %d\n", CheckChecksum((unsigned char*)&pack, sizeof(CrcPacket)));
*/

/*
    typedef struct _DataPacket {
        char data;
        char padding[3];
    } DataPacket;

    typedef struct _SeqPacket {
        DataPacket dataPacket;
        unsigned int flags;
        unsigned int sequenceNumber;    // Used for sequence number and SYN
        unsigned int ackSequenceNumber; // Used when ACK sent for sequence number
    } SeqPacket;

    typedef struct _CrcPacket {
        SeqPacket sequencePacket;
        char padding[3];
        unsigned char checksum;
    } CrcPacket;
*/





    return 0;
}

