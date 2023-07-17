// Local headers
#include <tasks.h>

#define CONNECTIONS 50
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

    char recvBuffer[LISTEN_BUFF_SIZE];
    size_t buffSize = sizeof(recvBuffer);
    size_t offSet = 0;
    _Bool terminate = 0;
    while(1) {
        ssize_t retVal = recv(capsule->clientSock.socket, recvBuffer + offSet, buffSize - offSet, 0); // TODO: Only reset timer on complete msg received
        if(retVal < 0) {
            perror("SOCKET recv");
            break;
        }
        else if(retVal == 0) break;
        else {
            retVal += offSet;
            interpret_msg(recvBuffer, buffSize, retVal, &offSet, &terminate, capsule);
        }
        if(terminate) break;
    }
    CLOSE_RECEIVER(capsule->clientSock.IPStr);
    free(capsule);
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