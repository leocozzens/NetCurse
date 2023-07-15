#include <common.h>

static volatile _Bool SIGINTFlag = 0;

FORCE_INLINE _Bool check_SIGINT(void) {
    return SIGINTFlag;
}

void graceful_close(int signum) {
    SIGINTFlag = 1;
}

void set_sock_timeout(int socket, int waitTime, int uwaitTime) {
    struct timeval timeout = { waitTime, 0 };
    UTIL_CHECK(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)), -1, "SOCKET setsockopt");
}