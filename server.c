#include <stdio.h>
#include <unistd.h>

#include "Net.h"

int main(void)
{
    NetState* state = ServerCreate(DEBUG_PORT);

    //printf("Reading\n");
    while(1) {



        char data = ServerRead(state);

        //printf("%c", data);



        usleep(1000 * 1000);
    }

    return 0;
}

