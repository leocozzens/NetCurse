#ifndef TASKS_H
#define TASKS_H

// C standard libraries
#include <unistd.h>
#include <errno.h>

// Local Headers
#include <task_helpers.h>

#define MSG_BATCH_SIZE 10

typedef struct {
    ssize_t retVal;
    char recvBuffer[(FRAME_SIZE * 2 + USERDATA_SIZE) * MSG_BATCH_SIZE];
    DataCapsule *capsule;
    _Bool *terminate;
} Context;

// void *workerFunc(void*);
void *connection_loop(void *arg);
void listen_for(ServerState *state);
void *receive_data(void *arg);
void *interpret_msg(void *arg);
void *process_queue(void * arg);

#endif