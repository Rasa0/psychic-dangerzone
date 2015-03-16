#ifndef NET_SHARED_H
#define NET_SHARED_H

#include "NetState.h"

#define GLOBAL_TIMEOUT_SEC 10.0

int TestGlobalTimeout(NetState* state);
void ResetGlobalTimeout(NetState* state);

// Returns the next sequence number and increment the counter
unsigned int GetNextSequenceNumber(NetState* state);

#endif
