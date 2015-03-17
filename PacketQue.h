typedef struct _PacketQueue {
    void* packet;
    unsigned int timestamp;

   _PacketQueue* next;
} PacketQueue;

PacketQueue* gPacketQueue = 0;

void InsertInQueue(void* packet, unsigned int time); 

void InsertInQueueCon( Void* packet, unsigned int time, PacketQueue* next);

void TrySend();
