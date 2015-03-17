#include "NetShared.h"

#include <time.h>

// Returns one if the global time out have triggered
int TestGlobalTimeout(NetState* state) {

    time_t currentTime;
    time(&currentTime);

    double seconds = difftime(currentTime, state->globalTimeout);

    return seconds > GLOBAL_TIMEOUT_SEC;
}

//Resets the global timeout timer
void ResetGlobalTimeout(NetState* state) {
    time(&state->globalTimeout);
}

// Returns the next sequence number and increment the counter
unsigned int GetNextSequenceNumber(NetState* state) {
    return state->nextSequenceNumber++;
}
