#include <tasks.h>

#define CONNECTIONS 5

#define CLOSE_RECEIVER(_IP) printf("Closed connection to [%s]\n", (_IP)); \
                            close(inet_addr(_IP))
#define FREE_CAPSULE(_Capsule) free(capsule->clientSock); \
                               free(capsule)

// void *workerFunc(void *);

void *connection_loop(void *arg) {
    ServerState *state = arg;

    SADDR_IN serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(IN_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(state->serverSock->IPStr);

    state->serverSock->socket = socket(AF_INET, SOCK_STREAM, 0);
    UTIL_CHECK(state->serverSock->socket, -1, "SOCKET socket");

    UTIL_CHECK(bind(state->serverSock->socket, (SADDR*) &serverAddr, serverAddrLen), -1, "SOCKET bind");

    printf("Waiting for connections...\n\n");
    while(1) listen_for(state);
    exit(0);
}

void listen_for(ServerState *state) {
    DataCapsule *capsule = malloc(sizeof(DataCapsule));
    MEM_ERROR(capsule, ALLOC_ERR);
    capsule->clientSock = malloc(sizeof(SockData));
    MEM_ERROR(capsule->clientSock, ALLOC_ERR);
    capsule->state = state;

    SADDR_IN clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    UTIL_CHECK(listen(state->serverSock->socket, CONNECTIONS), -1, "SOCKET listen");
    capsule->clientSock->socket = accept(state->serverSock->socket, (SADDR*) &clientAddr, &clientAddrLen);
    UTIL_CHECK(capsule->clientSock->socket, -1, "SOCKET accept");

    inet_ntop(clientAddr.sin_family, &(clientAddr.sin_addr), capsule->clientSock->IPStr, INET_ADDRSTRLEN);
    printf("Client connected to server from [%s]\n", capsule->clientSock->IPStr);

    pthread_t recvThread;
    pthread_create(&recvThread, NULL, receive_data, capsule);
    pthread_detach(recvThread);
}

void *receive_data(void *arg) {
    DataCapsule *capsule = arg;
    size_t dataSize = sizeof(UserData);
    char buffer[dataSize * MSG_BATCH_SIZE];
    while(1) {
        ssize_t retVal = recv(capsule->clientSock->socket, buffer, dataSize, 0); // TODO: Add close condition if wait time is surpassed
        if(retVal == -1) {
            perror("SOCKET recv");
            CLOSE_RECEIVER(capsule->clientSock->IPStr);
            FREE_CAPSULE(capsule);
            return NULL;
        }
        else if(retVal == 0) {
            CLOSE_RECEIVER(capsule->clientSock->IPStr);
            FREE_CAPSULE(capsule);
            return NULL;
        }
        else if(retVal < dataSize) {
            CLOSE_RECEIVER(capsule->clientSock->IPStr);
            FREE_CAPSULE(capsule);
            return NULL;
        }
        else {
            size_t msgCount = retVal / dataSize;
            size_t remnantBytes = retVal % dataSize;

            for(size_t i = 0; i < msgCount; i++) {
                Action *actionBuffer = malloc(sizeof(Action));
                MEM_ERROR(actionBuffer, ALLOC_ERR);

                memcpy(&actionBuffer->userPacket, buffer + (i * dataSize), dataSize);
                strcpy(actionBuffer->actionAddr, capsule->clientSock->IPStr);
                enqueue(capsule->state->userActions, actionBuffer);
            }

            if(remnantBytes != 0) {
                CLOSE_RECEIVER(capsule->clientSock->IPStr);
                FREE_CAPSULE(capsule);
                return NULL;
            }
        }
    }
}