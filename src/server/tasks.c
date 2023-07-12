// Local headers
#include <tasks.h>

#define CONNECTIONS 50

#define CLOSE_RECEIVER(_IP) printf("Closed connection to [%s]\n", (_IP)); \
                            close(inet_addr(_IP))
#define FREE_P_INFO(_PInfo) free((_PInfo->capsule)); \
                            free((_PInfo))

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
}


void listen_for(ServerState *state) {
    DataCapsule *capsule = malloc(sizeof(DataCapsule));
    MEM_ERROR(capsule, ALLOC_ERR);
    capsule->state = state;

    SADDR_IN clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    capsule->clientSock.socket = accept(state->serverSock->socket, (SADDR*) &clientAddr, &clientAddrLen);
    UTIL_CHECK(capsule->clientSock.socket, -1, "SOCKET accept");
    set_sock_timeout(capsule->clientSock.socket, DEFAULT_WAIT_TIME, DEFAULT_WAIT_TIME_U);

    inet_ntop(clientAddr.sin_family, &(clientAddr.sin_addr), capsule->clientSock.IPStr, INET_ADDRSTRLEN);
    printf("Client connected to server from [%s]\n", capsule->clientSock.IPStr);

    pthread_t recvThread;
    pthread_create(&recvThread, NULL, receive_data, capsule);
    pthread_detach(recvThread);
}

void *receive_data(void *arg) {
    DataCapsule *capsule = arg;

    Context *processingInfo;
    _Bool terminate = 0;
    while(1) {
        processingInfo = malloc(sizeof(Context));
        MEM_ERROR(processingInfo, ALLOC_ERR);
        processingInfo->terminate = &terminate;

        if(terminate) break;
        processingInfo->retVal = recv(capsule->clientSock.socket, processingInfo->recvBuffer, sizeof(processingInfo->recvBuffer), 0);
        if(terminate) break;

        if(processingInfo->retVal == -1) {
            perror("SOCKET recv");
            break;
        }
        else if(processingInfo->retVal == 0) break;
        else {
            processingInfo->capsule = malloc(sizeof(DataCapsule));
            memcpy(processingInfo->capsule, capsule, sizeof(DataCapsule));
            MEM_ERROR(processingInfo->capsule, ALLOC_ERR);

            pthread_t interpretThread;
            pthread_create(&interpretThread, NULL, interpret_msg, processingInfo);
            pthread_detach(interpretThread);
        }
    }
    CLOSE_RECEIVER(capsule->clientSock.IPStr);
    free(processingInfo);
    free(capsule);
    return NULL;
}

void *interpret_msg(void *arg) {
    Context *processingInfo = arg;
    size_t remainingBytes = processingInfo->retVal;

    while(remainingBytes > (FRAME_SIZE * 2)) {
        size_t headerPos = processingInfo->retVal - remainingBytes;
        switch(detect_msg_type(&remainingBytes, (processingInfo->recvBuffer + headerPos), processingInfo->capsule)) {
            case TERMINATE_CODE:
                *processingInfo->terminate = 1;
                goto ENDLOOP;
            case ENDMSG_CODE:
                goto ENDLOOP;
            case HEARTBEAT_CODE:
                if(processingInfo->recvBuffer[headerPos + FRAME_SIZE] != CNN_ALIVE) {
                    *processingInfo->terminate = 1;
                    goto ENDLOOP;
                }
                break;
            case USERDATA_CODE:
                if(make_action((processingInfo->recvBuffer + headerPos), FRAME_SIZE, USERDATA_SIZE, processingInfo->capsule)) {
                    *processingInfo->terminate = 1;
                    goto ENDLOOP;
                }
                break;
        }
    }
    ENDLOOP:
    FREE_P_INFO(processingInfo);
    return NULL;
}

void *process_queue(void * arg) {
    ServerState *state = arg;
    while(1) {
        Action *inData;
        dequeue(state->userActions, &inData);
        printf("Message received from [%s]\nMSG: %s\n", inData->actionAddr, inData->userPacket.msg);
        free(inData);
    }
}