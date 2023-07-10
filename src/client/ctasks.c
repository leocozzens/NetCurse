#include <ctasks.h>
#include <common.h>

void *lookup_loop(void *arg) {
    int *clientSocket = arg;
    while(!check_SIGINT()) usleep(100000);
    printf("\nSIGINT detected: Exiting gracefully\n");
    close(*clientSocket);
    exit(0);
}