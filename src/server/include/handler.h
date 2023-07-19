#ifndef TASK_HELPERS_H
#define TASK_HELPERS_H

// C standard libraries
#include <unistd.h>

// Local headers
#include <model.h>

#define LISTEN_BUFF_SIZE (FRAME_SIZE * 2 + USERDATA_SIZE) * MSG_BATCH_SIZE

#define CLOSE_RECEIVER(_SockStruct) printf("Closed connection to [%s]\n", (_SockStruct.IPStr)); \
                            close(_SockStruct.socket)

typedef enum {
    TERMINATE_CODE = -1,
    ENDMSG_CODE,
    KEEPALIVE_CODE,
    USERDATA_CODE
} MessageType;

typedef enum {
    SOCKET_FAIL = -1,
    WRONG_HEADER,
    BAD_FORMAT,
    VERIFIED
} FrameCode;

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

typedef struct {
    size_t *offSet;
    size_t remainingBytes;
    _Bool fragmented;
    size_t fragPos;
    char *suppBuff;
} BuffData;

void interpret_msg(char *recvBuffer, size_t buffSize, ssize_t retVal, size_t *offSet, KeepAliveStat *connStatus, DataCapsule *capsule);
_Bool recv_full(int socket, char *buffer, size_t desiredSize, int flags);
void send_keepalive(KeepAliveStat *connStatus, int clientSock);

#endif