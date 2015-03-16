#include "SlidingWindow.h"

//Initialize the window
void InitSlidingWindow(SlidingWindow* window) {

    window->inOutBuffer = 0;

    start = 0;
    end = 0;

    // Initialize sequence number
    window->nextSequenceNumber = time(0);
}

void AddDataToWindow(DataPacket* packet) {


}
