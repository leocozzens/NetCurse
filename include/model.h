#ifndef MODEL_H
#define MODEL_H

// POSIX headers
#include <pthread.h>

// Local headers
#include <common.h>

#define USERDATA_SIZE sizeof(UserData)

typedef struct {
    char msg[BUFF_SIZE];
} UserData;

typedef struct Action {
    UserData userPacket;
    char actionAddr[INET_ADDRSTRLEN];
    struct Action *nextAction;
} Action;

typedef struct {
    Action *head;
    Action *tail;
    pthread_mutex_t *tailLock;
} ActionQueue;

void init_queue(ActionQueue *userActions);
void enqueue(ActionQueue *userActions, Action *newAction);
void print_queue(ActionQueue *userActions);
_Bool dequeue(ActionQueue *userActions, Action **retData);
void queue_cleanup(ActionQueue *userActions);

#endif