#include <ctasks.h>
#include <common.h>

void *beat_loop(void *arg) {
    ConnData *connInfo = arg;
    size_t packetSize;
    char *buffer = make_packet(BEAT_SIZE, FRAME_SIZE, HEARTBEAT_HEADER, HEARTBEAT_FOOTER, &packetSize);

    while(!check_SIGINT()) {
        send_heartbeat(buffer, packetSize, connInfo);
        usleep(HEARTBEAT_INTERVAL);
    }
    connInfo->connectionState = CNN_DEAD;
    send_heartbeat(buffer, packetSize, connInfo);

    pthread_mutex_lock(&connInfo->sendLock);
    CLOSE_NOW(connInfo->clientSock);
    pthread_mutex_unlock(&connInfo->sendLock);
    exit(0);
}

char *make_packet(size_t dataSize, size_t frameSize, const char *header, const char *footer, size_t *packetSize) {
    *packetSize = (frameSize * 2) + dataSize;
    char *outFrame = malloc(*packetSize);
    strncpy(outFrame, header, frameSize);
    strncpy(outFrame + frameSize + dataSize, footer, frameSize);
    return outFrame;
}

void send_heartbeat(char *buffer, size_t buffSize, ConnData *connInfo) {
    buffer[FRAME_SIZE] = connInfo->connectionState;
    pthread_mutex_lock(&connInfo->sendLock);
    send(connInfo->clientSock, buffer, buffSize, 0);
    pthread_mutex_unlock(&connInfo->sendLock);
}