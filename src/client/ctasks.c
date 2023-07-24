// POSIX headers
#include <pthread.h>
#include <sys/socket.h>

// Local headers
#include <ctasks.h>
#include <common.h>

void *keepalive_loop(void *arg) {
    ConnData *connInfo = arg;
    size_t packetSize;
    char *buffer = make_packet(KEEPALIVE_SIZE, FRAME_SIZE, KEEPALIVE_HEADER, KEEPALIVE_FOOTER, &packetSize);
    MEM_ERROR(buffer, ALLOC_ERR);

    while(!check_SIGINT()) {
        send_keepalive(buffer, packetSize, connInfo);
        usleep(KEEPALIVE_INTERVAL);
    }
    connInfo->connectionState = CNN_DEAD;
    send_keepalive(buffer, packetSize, connInfo);

    pthread_mutex_lock(&connInfo->sendLock);
    CLOSE_NOW(connInfo->clientSock);
    pthread_mutex_unlock(&connInfo->sendLock);
    exit(0);
}

void send_keepalive(char *buffer, size_t buffSize, ConnData *connInfo) {
    buffer[FRAME_SIZE] = connInfo->connectionState;
    pthread_mutex_lock(&connInfo->sendLock);
    send(connInfo->clientSock, buffer, buffSize, 0);
    pthread_mutex_unlock(&connInfo->sendLock);
}