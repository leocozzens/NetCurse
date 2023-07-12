#ifndef TASK_HELPERS_H
#define TASK_HELPERS_H

// Local headers
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
    SockData *clientSock;
    ServerState *state;
} DataCapsule;

void interpret_msg(size_t retVal, const char *recvBuffer, DataCapsule *capsule, _Bool *terminate);
MessageType detect_msg_type(size_t *remainingBytes, const char *recvBuffer, DataCapsule *capsule);
FrameCode verify_frame(const char *recvBuffer, size_t *remainingBytes, size_t frameWidth, size_t dataSize, const char *header, const char *footer);
_Bool make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule);

#endif