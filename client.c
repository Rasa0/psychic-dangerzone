#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "Net.h"
#include "Checksum.h"

int run = 1;

// Thread that keep polling the network, eventhough client input is blocking
void* PollThread(void* args) {
    NetState* state = (NetState*)args;

    while(run) {
        ClientPoll(state);
        usleep(1000 * 1000);
    }

    return 0;
}

int main(void)
{


    NetState* state = ClientCreate("127.0.0.1", DEBUG_PORT);
    pthread_t thread;
    pthread_attr_t threadAttr;



    if(state) {
        printf("Connection Complete!\n");
        pthread_attr_init(&threadAttr);

        if(pthread_create(&thread,&threadAttr, &PollThread, (void*)state) < 0) {
            printf("Couldn't create thread\n");

            // TODO fix graceful exit
        } else {

            while(1) {

                char c = getchar();
                fflush(stdin);

                if(c == 'q') {
                    break;
                } else {
                    ClientSendData(state, c);
                }

                //ClientReadTEST(state);
            }
        }
    } else {
        printf("Could not connect to server!\n");
    }

    run = 0;
    pthread_join(thread, NULL);
    ClientClose(state);

    return 0;
}

