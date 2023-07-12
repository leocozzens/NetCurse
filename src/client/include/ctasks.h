#ifndef CTASKS_H
#define CTASKS_H

// C standard libraries
#include <unistd.h>

// Local headers
#include <model_structs.h>

typedef struct {
    int clientSock;
    char connectionState;
    pthread_mutex_t sendLock;
} ConnData;

void *beat_loop(void *arg);
void send_heartbeat(char *buffer, int buffSize, ConnData *connInfo);

#endif