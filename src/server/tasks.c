// C standard libraries
#include <time.h>
#include <unistd.h>
#include <errno.h>

// POSIX headers
#include <pthread.h>

// Local headers
#include <tasks.h>
#include <handler.h>

#define CONNECTIONS 50
#define MSG_BATCH_SIZE 10
#define LISTEN_BUFF_SIZE (FRAME_SIZE * 2 + USERDATA_SIZE) * MSG_BATCH_SIZE
#define FALLBACK_WAIT DEFAULT_WAIT_TIME * 3
#define FALLBACK_WAIT_U DEFAULT_WAIT_TIME_U * 3

static void listen_for(ServerState *state);

void *connection_loop(void *arg) {
    ServerState *state = arg;

    SADDR_IN serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(IN_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(state->serverSock->IPStr);

    state->serverSock->socket = socket(AF_INET, SOCK_STREAM, 0);
    UTIL_CHECK(state->serverSock->socket, UTIL_ERRVAL, "SOCKET socket");

    UTIL_CHECK(bind(state->serverSock->socket, (SADDR*) &serverAddr, serverAddrLen), -1, "SOCKET bind");
    UTIL_CHECK(listen(state->serverSock->socket, CONNECTIONS), UTIL_ERRVAL, "SOCKET listen");

    printf("Waiting for connections...\n\n");
    while(1) listen_for(state);
}


static void listen_for(ServerState *state) {
    DataCapsule *capsule = malloc(sizeof(DataCapsule));
    while(capsule == NULL) {
        fprintf(stderr, "Trouble accepting new connection: %s\n", ALLOC_ERR);
        sleep(1);
        capsule = malloc(sizeof(DataCapsule));
    }
    capsule->state = state;

    SADDR_IN clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    capsule->clientSock.socket = accept(state->serverSock->socket, (SADDR*) &clientAddr, &clientAddrLen);
    if(capsule->clientSock.socket == UTIL_ERRVAL) {
        fprintf(stderr, "Trouble accepting new connection: SOCKET accept - %s\n", strerror(errno));
        return;
    }

    set_sock_timeout(capsule->clientSock.socket, FALLBACK_WAIT, FALLBACK_WAIT_U);
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
    KeepAliveStat connStatus = { 0, 0, capsule->clientSock.socket };

    pthread_t sockTimeout;
    pthread_create(&sockTimeout, NULL, socket_timeout, &connStatus);
    connStatus.kaOut = make_packet(KEEPALIVE_SIZE, FRAME_SIZE, KEEPALIVE_HEADER, KEEPALIVE_FOOTER, &connStatus.kaSize);
    while(1) {
        ssize_t retVal = recv(capsule->clientSock.socket, recvBuffer + offSet, buffSize - offSet, 0);
        if(retVal < 1) { // Check for bad recv call
            connStatus.terminate = 1;
            break;
        }
        else {
            retVal += offSet;
            interpret_msg(recvBuffer, buffSize, retVal, &offSet, &connStatus, capsule);
        }
        if(connStatus.terminate) break;
    }
    send_keepalive(&connStatus, capsule->clientSock.socket);
    CLOSE_RECEIVER(capsule->clientSock);
    pthread_join(sockTimeout, NULL);
    free(capsule);
    free(connStatus.kaOut);
    return NULL;
}

void *process_queue(void *arg) {
    ServerState *state = arg;
    while(1) {
        Action *inData;
        dequeue(state->userActions, &inData);
        printf("Message received from [%s]\nMSG: %s\n", inData->actionAddr, inData->userPacket.msg);
        free(inData);
    }
}

void *socket_timeout(void *arg) {
    KeepAliveStat *connStatus = arg;
    struct timespec waitTime = { DEFAULT_WAIT_TIME, DEFAULT_WAIT_TIME_U };
    while(!connStatus->terminate) {
        nanosleep(&waitTime, NULL);
        if(connStatus->messageReceived) connStatus->messageReceived = 0;
        else break;
    }
    connStatus->terminate = 1;
    shutdown(connStatus->clientSock, SHUT_RDWR); // TODO: When send functions are in place incorporate send locks here
    return NULL;
}