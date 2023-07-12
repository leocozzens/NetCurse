#ifndef STRUCTS_H
#define STRUCTS_H

#include <common.h>

typedef struct {
    int clientSock;
    char connectionState;
    pthread_mutex_t sendLock;
} ConnData;

#endif