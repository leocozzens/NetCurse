// Local headers
#include <tasks.h>

#define WORKER_COUNT 30

int main(int argc, char **argv) {
    InitialInput entry;

    if(argc < 2) entry.hostIP = "0.0.0.0";
    else entry.hostIP = argv[1];

    init_queue(&entry.userActions);

    pthread_t connectionIntake;
    pthread_create(&connectionIntake, NULL, connection_loop, (void*) &entry);

    // pthread_t workerPool[WORKER_COUNT];
    // for(int i = 0; i < WORKER_COUNT; i ++) {
    //     pthread_create(&workerPool[i], NULL, workerFunc, NULL);
    // }

    pthread_join(connectionIntake, NULL);
    return 0;
}