#ifndef TASKS_H
#define TASKS_H

// void *workerFunc(void*);
void *connection_loop(void *arg);
void *receive_data(void *arg);
void *process_queue(void * arg);
void *socket_timeout(void *arg);

#endif