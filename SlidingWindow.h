#ifndef SLIDING_WINDOW_H
#define SLIDING_WINDOW_H

#include "Packet.h"
#include "NetState.h"

#define SLIDING_WINDOW_SIZE 1

// Buffer used to contain the packets before they are put in the window
// Or when they have been received by the server
typedef struct _SlidingWindowInOutBuffer {


} SlidingWindowInOutBuffer;


typedef struct _SlidingWindowEntry {




} SlidingWindowEntry;


typedef struct _SlidingWindow {

    SlidingWindowInOutBuffer* inOutBuffer;

    //Circular array for the sliding window
    SeqPacket windowArray[SLIDING_WINDOW_SIZE * 2];

    unsigned int start;
    unsigned int end;

    unsigned int nextSequenceNumber;


} SlidingWindow;

//Initialize the window
void InitSlidingWindow(SlidingWindow* window);

void AddDataToWindow(DataPacket* packet);

void AddSynPacketToWindow();



#endif
