#ifndef TASK_HELPERS_H
#define TASK_HELPERS_H

// C standard libraries
#include <unistd.h>

// Local headers
#include <model.h>

#define LISTEN_BUFF_SIZE (FRAME_SIZE * 2 + USERDATA_SIZE) * MSG_BATCH_SIZE

#define CLOSE_RECEIVER(_IP) printf("Closed connection to [%s]\n", (_IP)); \
                            close(inet_addr(_IP))

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
    size_t fragPos;
    char *suppBuff;
} BuffData;

void interpret_msg(char *recvBuffer, size_t buffSize, ssize_t retVal, size_t *offSet, _Bool *terminate, DataCapsule *capsule);
MessageType detect_msg_type(char *recvBuffer, BuffData *buffInfo, DataCapsule *capsule);
FrameCode verify_frame(char **dataPos, BuffData *buffInfo, size_t frameWidth, size_t dataSize, const char *header, const char *footer, DataCapsule *capsule);
char *handle_fragments(char *recvBuffer, BuffData *buffInfo, size_t objectSize, size_t bufferDiff, DataCapsule *capsule);
_Bool recv_full(int socket, char *buffer, size_t desiredSize, int flags);
void make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule);

#endif