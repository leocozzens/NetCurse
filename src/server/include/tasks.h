#ifndef TASKS_H
#define TASKS_H

// POSIX libraries
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <common.h>
#include <model.h>

typedef struct {
    int clientSock;
    char clientIP[INET_ADDRSTRLEN];
} ClientSockData; // TODO: Merge these

typedef struct {
    int serverSock;
    char hostIP[INET_ADDRSTRLEN];
} ServerSockData;

typedef struct {
    ClientSockData *clientData;
    ServerSockData *serverData;
    pthread_t *recvThread;
    ActionQueue *userActions;
} DataCapsule;

typedef struct {
    ActionQueue userActions;
    const char *hostIP;
} InitialInput;

void *workerFunc(void*);
void *connection_loop(void *arg);
void* listen_for(void *arg);
void *receive_data(void *arg);

#endif