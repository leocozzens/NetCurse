#ifndef CTASKS_H
#define CTASKS_H

// C standard libraries
#include <unistd.h>

// Local headers
#include <structs.h>
#include <shared_model.h>

void *beat_loop(void *arg);
void send_heartbeat(char *buffer, size_t buffSize, ConnData *connInfo);
char *make_packet(size_t dataSize, size_t frameSize, const char *header, const char *footer, size_t *packetSize);

#endif