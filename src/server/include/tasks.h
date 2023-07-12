#ifndef TASKS_H
#define TASKS_H

// C standard libraries
#include <unistd.h>
#include <errno.h>

// Local headers
#include <task_helpers.h>

// void *workerFunc(void*);
void *connection_loop(void *arg);
void *receive_data(void *arg);

#endif