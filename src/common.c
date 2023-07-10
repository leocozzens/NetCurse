#include <common.h>

volatile _Bool SIGINTFlag = 0;

void graceful_close(int signum) {
    SIGINTFlag = 1;
}

_Bool check_SIGINT(void) {
    return SIGINTFlag;
}