#include <ctasks.h>
#include <common.h>

void *beat_loop(void *arg) {
    ConnData *connInfo = arg;
    char buffer[BEAT_SIZE + (FRAME_SIZE * 2)];
    strncpy(buffer, HEARTBEAT_HEADER, FRAME_SIZE);
    strncpy(buffer + (FRAME_SIZE + BEAT_SIZE), HEARTBEAT_FOOTER, FRAME_SIZE);
    while(!check_SIGINT()) {
        send_heartbeat(buffer, sizeof(buffer), connInfo);
        usleep(100000);
    }
    connInfo->connectionState = CNN_DEAD;
    send_heartbeat(buffer, sizeof(buffer), connInfo);

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

void send_heartbeat(char *buffer, int buffSize, ConnData *connInfo) {
    buffer[FRAME_SIZE] = connInfo->connectionState;
    pthread_mutex_lock(&connInfo->sendLock);
    send(connInfo->clientSock, buffer, buffSize, 0);
    pthread_mutex_unlock(&connInfo->sendLock);
}