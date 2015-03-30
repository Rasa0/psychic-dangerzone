#include <time.h>
#include <stdlib.h>
#include <sys/timeb.h>

Void ReOrderGen(unsigned char* message, int nBytes,int ReOrderRate)
{
    {   struct timeb tmb;
        ftime(&tmb);

        int et = (10+ rand()%70)/100;
        int t = rand()%100;

        if(t < ReOrderRate)
        {
            InsertInQueue(packet,tmb.millitm+et);
        }
        else
           InsertInQueue(message,tmb.millitm+0.01);
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

