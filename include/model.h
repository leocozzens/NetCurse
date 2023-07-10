#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>

#include <common.h>

typedef struct {
    char msg[BUFF_SIZE];
} UserData;

typedef struct Action {
    UserData *userPacket;
    struct Action *nextAction;
} Action;

typedef struct {
    Action *head;
    Action *tail;
} ActionQueue;

void init_queue(ActionQueue *userActions);
void enqueue(ActionQueue *userActions, UserData *data);
void print_queue(ActionQueue *userActions);

#endif