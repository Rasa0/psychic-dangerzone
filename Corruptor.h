#include <time.h>
#include <stdlib.h>

Void ReOrderGen(unsigned char* message, int nBytes,int ReOrderRate);


Void PaketLossGen(unsigned char* message, int nBytes,int PaketLossRate,int ReOrderRate);

// Before using this function. Remember to seed rand
Void Corruptor(unsigned char* message, int nBytes,int BitFailRate,int PaketLossRate,int ReOrderRate);


