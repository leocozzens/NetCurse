#ifndef TASKS_H
#define TASKS_H

// POSIX headers
#include <pthread.h>

// C standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Local headers
#include <common.h>
#include <model.h>

typedef enum {
    TERMINATE_CODE = -1,
    ENDMSG_CODE,
    HEARTBEAT_CODE,
    USERDATA_CODE
} MessageType;

typedef enum {
    WRONG_HEADER,
    BAD_FORMAT,
    VERIFIED
} MessageCode;

typedef struct {
    int socket;
    char IPStr[INET_ADDRSTRLEN];
} SockData;

typedef struct {
    SockData *serverSock;
    ActionQueue *userActions;
} ServerState;

typedef struct {
    SockData *clientSock;
    ServerState *state;
} DataCapsule;

// void *workerFunc(void*);
void *connection_loop(void *arg);
void listen_for(ServerState *state);
void *receive_data(void *arg);
void interpret_msg(size_t retVal, const char *recvBuffer, DataCapsule *capsule, _Bool *terminate);
MessageType detect_msg_type(size_t *remainingBytes, const char *recvBuffer, DataCapsule *capsule);
MessageCode verify_frame(const char *recvBuffer, size_t *remainingBytes, size_t frameWidth, size_t dataSize, const char *header, const char *footer);
_Bool make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule);

#endif