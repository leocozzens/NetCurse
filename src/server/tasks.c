#include <tasks.h>

#define CONNECTIONS 50

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
    UTIL_CHECK(listen(state->serverSock->socket, CONNECTIONS), -1, "SOCKET listen");

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

    capsule->clientSock->socket = accept(state->serverSock->socket, (SADDR*) &clientAddr, &clientAddrLen);
    UTIL_CHECK(capsule->clientSock->socket, -1, "SOCKET accept");
    set_sock_timeout(capsule->clientSock->socket, DEFAULT_WAIT_TIME, DEFAULT_WAIT_TIME_U);

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
        ssize_t retVal = recv(capsule->clientSock->socket, buffer, sizeof(buffer), 0); // TODO: Add close condition if wait time is surpassed
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
        else {
            size_t remainingBytes = retVal;
            _Bool terminate = 0;

            while(remainingBytes > (FRAME_SIZE * 2)) {
                size_t headerPos = retVal - remainingBytes;

                if((strncmp(buffer + headerPos, USERDATA_HEADER, FRAME_SIZE) == 0)) {
                    remainingBytes -= FRAME_SIZE;
                    if(remainingBytes < (FRAME_SIZE + dataSize)) break;
                    if(strncmp(buffer + (headerPos + (FRAME_SIZE + dataSize)), USERDATA_FOOTER, FRAME_SIZE) != 0) break;

                    remainingBytes -= FRAME_SIZE + dataSize;
                    Action *actionBuffer = malloc(sizeof(Action));
                    MEM_ERROR(actionBuffer, ALLOC_ERR);

                    memcpy(&actionBuffer->userPacket, buffer + (headerPos + FRAME_SIZE), dataSize);
                    strcpy(actionBuffer->actionAddr, capsule->clientSock->IPStr);
                    pthread_mutex_lock(capsule->state->userActions->tailLock);
                    enqueue(capsule->state->userActions, actionBuffer);
                    pthread_mutex_unlock(capsule->state->userActions->tailLock);
                }
                else if(strncmp(buffer + headerPos, HEARTBEAT_HEADER, FRAME_SIZE) == 0) {
                    remainingBytes -= FRAME_SIZE;
                    if(remainingBytes < (FRAME_SIZE + BEAT_SIZE)) break;
                    if(strncmp(buffer + (headerPos + (FRAME_SIZE + BEAT_SIZE)), HEARTBEAT_FOOTER, FRAME_SIZE) != 0) break;

                    if(*(buffer + (headerPos + FRAME_SIZE)) == CNN_ALIVE) remainingBytes -= FRAME_SIZE + BEAT_SIZE;
                    else if(*(buffer + (headerPos + FRAME_SIZE)) == CNN_DEAD) {
                        terminate = 1;
                        break;
                    }
                    else break;
                }
            }

            if(terminate) {
                CLOSE_RECEIVER(capsule->clientSock->IPStr);
                FREE_CAPSULE(capsule);
                return NULL;
            }
        }
    }
}