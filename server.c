#include <stdio.h>
#include <unistd.h>

#include "Net.h"

int main(void)
{
    NetState* state = ServerCreate(DEBUG_PORT);

    printf("Reading\n");
    while(1) {



        char data = ServerRead(state);

        printf("%c", data);


        //sleep(10);
    }

    return 0;
}

