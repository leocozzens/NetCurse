#ifndef TASKS_H
#define TASKS_H

// C standard libraries
#include <errno.h>

// Local Headers
#include <handler.h>

#define MSG_BATCH_SIZE 10

// void *workerFunc(void*);
void *connection_loop(void *arg);
void listen_for(ServerState *state);
void *receive_data(void *arg);
void *process_queue(void * arg);
void *socket_timeout(void *arg);

#endif