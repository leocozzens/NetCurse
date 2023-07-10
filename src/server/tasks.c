#include <tasks.h>

#define CONNECTIONS 5

#define CLOSE_RECEIVER(_IP) printf("Closed connection to [%s]\n", (_IP)); \
                            close(inet_addr(_IP))

void *workerFunc(void *);


void *connection_loop(void *arg) {
    ServerState *state = arg;

    SADDR_IN serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(IN_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(state->serverSock->IPStr);

    state->serverSock->socket = socket(AF_INET, SOCK_STREAM, 0);

    if(bind(state->serverSock->socket, (SADDR*) &serverAddr, serverAddrLen) == -1) {
        perror("SOCKET bind");
        exit(1);
    }

    while(1) listen_for(state);
}

void* listen_for(ServerState *state) {
    DataCapsule *capsule = malloc(sizeof(DataCapsule));
    capsule->clientSock = malloc(sizeof(SockData));
    capsule->state = state;

    SADDR_IN clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    if(listen(state->serverSock->socket, CONNECTIONS) == -1) {
        perror("SOCKET listen");
        exit(1);
    }
    capsule->clientSock->socket = accept(state->serverSock->socket, (SADDR*) &clientAddr, &clientAddrLen);

    inet_ntop(clientAddr.sin_family, &(clientAddr.sin_addr), capsule->clientSock->IPStr, INET_ADDRSTRLEN);
    printf("Client connected to server from [%s]\n", capsule->clientSock->IPStr);

    pthread_t recvThread;
    pthread_create(&recvThread, NULL, receive_data, capsule);
    pthread_detach(recvThread);
    return NULL;
}

void *receive_data(void *arg) {
    printf("Receiving\n");
    DataCapsule *capsule = arg;
    while(1) {
        UserData *userBuffer = malloc(sizeof(UserData));
        size_t retVal = recv(capsule->clientSock->socket, userBuffer, sizeof(UserData), 0);
        if(retVal == -1) {
            perror("SOCKET recv");
            CLOSE_RECEIVER(capsule->clientSock->IPStr);
            free(capsule->clientSock);
            free(capsule);
            return NULL;
        }
        else if(retVal == 0) {
            CLOSE_RECEIVER(capsule->clientSock->IPStr);
            free(capsule->clientSock);
            free(capsule);
            return NULL;
        }
        enqueue(capsule->state->userActions, userBuffer);
    }
}