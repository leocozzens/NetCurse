// POSIX headers
#include <pthread.h>

// Local headers
#include <tasks.h>
#include <handler.h>
#include <unistd.h>

// #define WORKER_COUNT 30

int main(int argc, char **argv) {
    sigset_t mask;
    sigset_t origMask;
    HANDLE_SIGINT(mask, origMask);

    SockData serverSock;
    ActionQueue userActions;
    ServerState state = { &serverSock, &userActions };

    if(argc < 2) strcpy(state.serverSock->IPStr, "0.0.0.0");
    else strncpy(state.serverSock->IPStr, argv[1], INET_ADDRSTRLEN - 1);

    init_queue(state.userActions);

    pthread_t connectionIntake;
    pthread_create(&connectionIntake, NULL, connection_loop, &state);

    // pthread_t workerPool[WORKER_COUNT];
    // for(int i = 0; i < WORKER_COUNT; i ++) {
    //     pthread_create(&workerPool[i], NULL, workerFunc, NULL);
    // }

    pthread_t queueLoop;
    pthread_create(&queueLoop, NULL, process_queue, &state);

    while (!check_SIGINT()) sigsuspend(&origMask);
    CLOSE_NOW(state.serverSock->socket);
    return 0;
}