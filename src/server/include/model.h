#ifndef MODEL_H
#define MODEL_H

// Local headers
#include <common.h>
#include <shared_model.h>

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