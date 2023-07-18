#ifndef CTASKS_H
#define CTASKS_H

// C standard libraries
#include <unistd.h>

// Local headers
#include <structs.h>
#include <shared_model.h>

void *keepalive_loop(void *arg);
void send_keepalive(char *buffer, size_t buffSize, ConnData *connInfo);

#endif