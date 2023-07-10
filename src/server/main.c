// Local headers
#include <tasks.h>

#define WORKER_COUNT 30

int main(int argc, char **argv) {
    SockData serverData;
    ActionQueue userActions;
    ServerState state = { &serverData, &userActions };

    if(argc < 2) strcpy(state.serverSock->IPStr, "0.0.0.0");
    else strncpy(state.serverSock->IPStr, argv[1], INET_ADDRSTRLEN - 1);

    init_queue(state.userActions);

    pthread_t connectionIntake;
    pthread_create(&connectionIntake, NULL, connection_loop, (void*) &state);

    // pthread_t workerPool[WORKER_COUNT];
    // for(int i = 0; i < WORKER_COUNT; i ++) {
    //     pthread_create(&workerPool[i], NULL, workerFunc, NULL);
    // }

    pthread_join(connectionIntake, NULL);
    return 0;
}