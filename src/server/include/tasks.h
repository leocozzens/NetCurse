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
    int socket;
    char IPStr[INET_ADDRSTRLEN];
} SockData;

typedef struct {
    SockData *serverSock;
    ActionQueue *userActions;
} ServerState;

typedef struct {
    SockData *clientSock;
    ServerState *state;
} DataCapsule;

void *workerFunc(void*);
void *connection_loop(void *arg);
void* listen_for(ServerState *state);
void *receive_data(void *arg);

#endif