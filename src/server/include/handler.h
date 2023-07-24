#ifndef TASK_HELPERS_H
#define TASK_HELPERS_H

// Local headers
#include <model.h>

#define CLOSE_RECEIVER(_SockStruct) printf("Closed connection to [%s]\n", (_SockStruct.IPStr)); \
                            close(_SockStruct.socket)

typedef struct {
    int socket;
    char IPStr[INET_ADDRSTRLEN];
} SockData;

typedef struct {
    SockData *serverSock;
    ActionQueue *userActions;
} ServerState;

typedef struct {
    SockData clientSock;
    ServerState *state;
} DataCapsule;

typedef struct {
    _Bool terminate;
    _Bool messageReceived;
    int clientSock;
    size_t kaSize;
    char *kaOut;
} KeepAliveStat;

void interpret_msg(char *recvBuffer, size_t buffSize, ssize_t retVal, size_t *offSet, KeepAliveStat *connStatus, DataCapsule *capsule);
_Bool recv_full(int socket, char *buffer, size_t desiredSize, int flags);
void send_keepalive(KeepAliveStat *connStatus, int clientSock);

#endif