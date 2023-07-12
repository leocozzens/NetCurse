#include <tasks.h>

#define CONNECTIONS 50
#define MSG_BATCH_SIZE 10

#define CLOSE_RECEIVER(_IP) printf("Closed connection to [%s]\n", (_IP)); \
                            close(inet_addr(_IP))
#define FREE_CAPSULE(_Capsule) free(capsule->clientSock); \
                               free(capsule)
#define CHECK_FRAME(_FrameState, _SuccessCode) if((_FrameState) == (VERIFIED)) return _SuccessCode; \
                                               else if((_FrameState) == (BAD_FORMAT)) return ENDMSG_CODE

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
    char recvBuffer[USERDATA_SIZE * MSG_BATCH_SIZE];
    while(1) {
        ssize_t retVal = recv(capsule->clientSock->socket, recvBuffer, sizeof(recvBuffer), 0);
        _Bool terminate = 0;

        if(retVal == -1) {
            perror("SOCKET recv");
            terminate = 1;
        }
        else if(retVal == 0) terminate = 1;
        else interpret_msg(retVal, recvBuffer, capsule, &terminate);

        if(terminate) {
            CLOSE_RECEIVER(capsule->clientSock->IPStr);
            FREE_CAPSULE(capsule);
            return NULL;
        }
    }
}

void interpret_msg(size_t retVal, const char *recvBuffer, DataCapsule *capsule, _Bool *terminate) {
    size_t remainingBytes = retVal;

    while(remainingBytes > (FRAME_SIZE * 2)) {
        size_t headerPos = retVal - remainingBytes;
        switch(detect_msg_type(&remainingBytes, (recvBuffer + headerPos), capsule)) {
            case TERMINATE_CODE:
                *terminate = 1;
                break;
            case ENDMSG_CODE:
                goto ENDLOOP;
            case HEARTBEAT_CODE:
                if(recvBuffer[headerPos + FRAME_SIZE] != CNN_ALIVE) *terminate = 1;
                break;
            case USERDATA_CODE:
                if(make_action((recvBuffer + headerPos), FRAME_SIZE, USERDATA_SIZE, capsule)) {
                    *terminate = 1;
                    goto ENDLOOP;
                }
                break;
        }
    }
    ENDLOOP:
}

MessageType detect_msg_type(size_t *remainingBytes, const char *recvBuffer, DataCapsule *capsule) {
    MessageCode frameState;

    frameState = verify_frame(recvBuffer, remainingBytes, FRAME_SIZE, BEAT_SIZE, HEARTBEAT_HEADER, HEARTBEAT_FOOTER);
    CHECK_FRAME(frameState, HEARTBEAT_CODE);

    frameState = verify_frame(recvBuffer, remainingBytes, FRAME_SIZE, USERDATA_SIZE, USERDATA_HEADER, USERDATA_FOOTER);
    CHECK_FRAME(frameState, USERDATA_CODE);
    return ENDMSG_CODE;
}

MessageCode verify_frame(const char *recvBuffer, size_t *remainingBytes, size_t frameWidth, size_t dataSize, const char *header, const char *footer) {
    if((strncmp(recvBuffer, header, frameWidth) == 0)) {
        *remainingBytes -= frameWidth;
        if(*remainingBytes < (frameWidth + dataSize)) return BAD_FORMAT;
        if(strncmp(recvBuffer + frameWidth + dataSize, footer, frameWidth) != 0) return BAD_FORMAT;
        *remainingBytes -= frameWidth + dataSize;
        return VERIFIED;
    }
    return WRONG_HEADER;
}

_Bool make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule) {
    Action *actionBuffer = malloc(sizeof(Action));
    if(actionBuffer == NULL) return 1;

    memcpy(&actionBuffer->userPacket, recvBuffer + frameWidth, dataSize);
    strcpy(actionBuffer->actionAddr, capsule->clientSock->IPStr);
    pthread_mutex_lock(capsule->state->userActions->tailLock);
    enqueue(capsule->state->userActions, actionBuffer);
    pthread_mutex_unlock(capsule->state->userActions->tailLock);
    return 0;
}