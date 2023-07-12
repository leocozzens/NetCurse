#ifndef TASKS_H
#define TASKS_H

// C standard libraries
#include <unistd.h>
#include <errno.h>

// Local Headers
#include <task_helpers.h>

// void *workerFunc(void*);
void *connection_loop(void *arg);
void listen_for(ServerState *state);
void *receive_data(void *arg);

#endif