// Local headers
#include <tasks.h>

// #define WORKER_COUNT 30

int main(int argc, char **argv) {
    HANDLE_SIGINT;
    SockData serverSock;
    ActionQueue userActions;
    ServerState state = { &serverSock, &userActions };

    if(argc < 2) strcpy(state.serverSock->IPStr, "0.0.0.0");
    else strncpy(state.serverSock->IPStr, argv[1], INET_ADDRSTRLEN - 1);

    init_queue(state.userActions);

    pthread_t connectionIntake;
    pthread_create(&connectionIntake, NULL, connection_loop, (void*) &state);

    // pthread_t workerPool[WORKER_COUNT];
    // for(int i = 0; i < WORKER_COUNT; i ++) {
    //     pthread_create(&workerPool[i], NULL, workerFunc, NULL);
    // }

    while(1) {
        Action *inData;
        if(dequeue(state.userActions, &inData)) {
            printf("Message received from [%s]\nMSG: %s\n", inData->actionAddr, inData->userPacket.msg);
            free(inData);
        }
        if(check_SIGINT()) {
            queue_cleanup(state.userActions);
            CLOSE_NOW(state.serverSock->socket);
        }
    }

    pthread_join(connectionIntake, NULL);
    CLOSE_NOW(serverSock.socket);
}