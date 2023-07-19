#include <common.h>

volatile static _Bool SIGINTFlag = 0;

FORCE_INLINE _Bool check_SIGINT(void) {
    return SIGINTFlag;
}

void graceful_close(int signum) {
    SIGINTFlag = 1;
}

void set_sock_timeout(int socket, int waitTime, int uwaitTime) {
    struct timeval timeout = { waitTime, uwaitTime };
    UTIL_CHECK(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)), -1, "SOCKET setsockopt");
}

char *make_packet(size_t dataSize, size_t frameSize, const char *header, const char *footer, size_t *packetSize) {
    *packetSize = (frameSize * 2) + dataSize;
    char *outFrame = malloc(*packetSize);
    strncpy(outFrame, header, frameSize);
    strncpy(outFrame + frameSize + dataSize, footer, frameSize);
    return outFrame;
}