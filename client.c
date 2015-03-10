#include <stdio.h>

#include "Net.h"

int main(void)
{
    NetState* state = ClientCreate("127.0.0.1", DEBUG_PORT);




    while(1) {

        char c = getchar();
        fflush(stdin);

        ClientSend(state, c);

    }


    printf("Hello World!\n");
    return 0;
}

