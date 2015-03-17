#include <time.h>
#include <stdlib.h>

Void ReOrderGen(unsigned char* message, int nBytes,int ReOrderRate)
{
    int et = 10+ rand()%70;
    int t = rand()%100;

    if(t < ReOrderRate)
    {
        InsertInQueue(packet,time+et);
    }
    else
       InsertInQueue(message,time+10);
}

Void PaketLossGen(unsigned char* message, int nBytes,int PaketLossRate,int ReOrderRate)
{
    int s = rand()%100;
    
    if(s < PaketLossRate)
    {
        return null;
    }
    else
        ReOrderGen(message,nBytes,ReOrderRate);
}

// Before using this function. Remember to seed rand
Void Corruptor(unsigned char* message, int nBytes,int BitFailRate,int PaketLossRate,int ReOrderRate)
{ 
    int r = rand()%100;
    
    if(r < BitFailRate)
    {
        if( (message << nBytes-1) = 1){
            message << nBytes-1 = 0;
            PaketLossRate(message,nBytes,PaketLossRate,ReOrderRate);
        }
        else{
            message << nBytes-1 = 1;
            PaketLossRate(message,nBytes,PaketLossRate,ReOrderRate);
        }
        
    }
    else 
        PaketLossRate(message,nBytes,PaketLossRate,ReOrderRate);
}

