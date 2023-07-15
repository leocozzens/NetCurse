#ifndef TASK_HELPERS_H
#define TASK_HELPERS_H

// Local headers
#include <model.h>

#define LISTEN_BUFF_SIZE (FRAME_SIZE * 2 + USERDATA_SIZE) * MSG_BATCH_SIZE

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
    size_t *offSet;
    size_t remainingBytes;
    _Bool fragmented;
    char *suppBuff;
} BuffData;

void interpret_msg(char *recvBuffer, size_t buffSize, ssize_t retVal, size_t *offSet, _Bool *terminate, DataCapsule *capsule);
MessageType detect_msg_type(const char *recvBuffer, BuffData *buffInfo, DataCapsule *capsule);
FrameCode verify_frame(const char *recvBuffer, BuffData *buffInfo, size_t frameWidth, size_t dataSize, const char *header, const char *footer, DataCapsule *capsule);
void make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule);

#endif