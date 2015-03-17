typedef struct _PacketQueue {
    void* packet;
    unsigned int timestamp;

   _PacketQueue* next;
} PacketQueue;

PacketQueue* gPacketQueue = 0;

void InsertInQueue(void* packet, unsigned int time) {


    if( gPacketQueue == 0)
    {
        gPacketQueue = (struct PacketQueue *) malloc( sizeof(struct PacketQueue) );
        gPacketQueue->packet = packet;
        gPacketQueue->timestamp = time;
        gPacketQueue->next = 0;
    }

    else if(time < gPacketQueue->timestamp)
    {
        PacketQueue* temp = gPacketQueue;
        gPacketQueue = (struct PacketQueue *) malloc( sizeof(struct PacketQueue) );
        gPacketQueue->packet = packet;
        gPacketQueue->timestamp = time;
        gPacketQueue->next = temp;
    }
    else
    {
        InsertInQueueCon(packet,time,gPacketQueue);
    }
        //remmembrer malloc and free to allocate and free memory
}


void InsertInQueueCon( Void* packet, unsigned int time, PacketQueue* next)
{
    if (next->next == 0)
    {
        next->next = (struct PacketQueue *) malloc( sizeof(struct PacketQueue) );
        next->next->packet = packet;
        next->next->timestamp = time;
        next->next->next = 0;
    }
    else if(time < next->next->timestamp)
    {
        PacketQueue* temp = next->next;
        next->next = (struct PacketQueue *) malloc( sizeof(struct PacketQueue) );
        next->next->packet = packet;
        next->next->timestamp = time;
        next->next->next = temp;
    }
    else
    {
        InsertInQueueCon(packet,time,next->next);
    }

}

void TrySend()
{
    if(gPacketQueue != 0 && gPacketQueue->timestamp < time)
    {   
        PacketQueue* PToSend = gPacketQueue->packet;
        gPacketQueue = gPacketQueue->next;
        
        PToSend/*send this data*/        

        //send message
    }
    else
        printf("listan är tom");//fix to work with server client messega thingie
}
