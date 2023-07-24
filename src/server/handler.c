// POSIX headers
#include <sys/socket.h>

// Local headers
#include <handler.h>

#define CHECK_FRAME(_FrameState, _SuccessCode) if((_FrameState) == (VERIFIED)) return _SuccessCode; \
                                               else if((_FrameState) == (BAD_FORMAT)) return ENDMSG_CODE; \
                                               else if((_FrameState) == (SOCKET_FAIL)) return TERMINATE_CODE

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
    size_t *offSet;
    size_t remainingBytes;
    _Bool fragmented;
    size_t fragPos;
    char *suppBuff;
} BuffData;

static MessageType detect_msg_type(char **recvBuffer, BuffData *buffInfo, DataCapsule *capsule);
static FrameCode verify_frame(char **dataPos, BuffData *buffInfo, size_t frameWidth, size_t dataSize, const char *header, const char *footer, DataCapsule *capsule);
static char *handle_fragments(char *recvBuffer, BuffData *buffInfo, size_t objectSize, size_t bufferDiff, DataCapsule *capsule);
static void make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule);

void interpret_msg(char *recvBuffer, size_t buffSize, ssize_t retVal, size_t *offSet, KeepAliveStat *connStatus, DataCapsule *capsule) {
    *offSet = 0;
    BuffData buffInfo = { offSet, retVal, 0 };

    while(buffInfo.remainingBytes > 0 && !connStatus->terminate) {
        size_t headerPos = retVal - buffInfo.remainingBytes;
        char *currentMessage = recvBuffer + headerPos;

        switch(detect_msg_type(&currentMessage, &buffInfo, capsule)) {
            case TERMINATE_CODE:
                connStatus->terminate = 1;
                goto ENDLOOP;
            case ENDMSG_CODE:
                goto ENDLOOP;
            case KEEPALIVE_CODE:
                if(currentMessage[FRAME_SIZE] == CNN_ALIVE) connStatus->messageReceived = 1;
                else {
                    connStatus->terminate = 1;
                    goto ENDLOOP;
                }
                send_keepalive(connStatus, capsule->clientSock.socket);
                break;
            case USERDATA_CODE:
                connStatus->messageReceived = 1;
                make_action(currentMessage, FRAME_SIZE, USERDATA_SIZE, capsule);
                break;
        }
    }
    ENDLOOP:
    if(buffInfo.fragmented) {
        if(*offSet > 0) memcpy(recvBuffer, buffInfo.suppBuff + buffInfo.fragPos, *offSet);
        free(buffInfo.suppBuff);
    }
}

static MessageType detect_msg_type(char **recvBuffer, BuffData *buffInfo, DataCapsule *capsule) {
    FrameCode frameState;
    frameState = verify_frame(recvBuffer, buffInfo, FRAME_SIZE, KEEPALIVE_SIZE, KEEPALIVE_HEADER, KEEPALIVE_FOOTER, capsule);
    CHECK_FRAME(frameState, KEEPALIVE_CODE);

    frameState = verify_frame(recvBuffer, buffInfo, FRAME_SIZE, USERDATA_SIZE, USERDATA_HEADER, USERDATA_FOOTER, capsule);
    CHECK_FRAME(frameState, USERDATA_CODE);
    return ENDMSG_CODE;
}

static FrameCode verify_frame(char **dataPos, BuffData *buffInfo, size_t frameWidth, size_t dataSize, const char *header, const char *footer, DataCapsule *capsule) {
    size_t messageDiff = 0;
    size_t objectSize;

    objectSize = frameWidth;
    if(buffInfo->remainingBytes < objectSize) {
        if((memcmp(*dataPos, header, buffInfo->remainingBytes) != 0)) return WRONG_HEADER;
        *dataPos = handle_fragments(*dataPos, buffInfo, objectSize, messageDiff, capsule);
        if(*dataPos == NULL) return SOCKET_FAIL;
    }
    if((memcmp(*dataPos, header, frameWidth) != 0)) return WRONG_HEADER;
    buffInfo->remainingBytes -= objectSize;
    messageDiff += objectSize;

    objectSize = frameWidth + dataSize;
    if(buffInfo->remainingBytes < objectSize) {
        *dataPos = handle_fragments(*dataPos, buffInfo, objectSize, messageDiff, capsule);
        if(*dataPos == NULL) return SOCKET_FAIL;
    }
    buffInfo->remainingBytes -= objectSize;
    messageDiff += objectSize;

    if(memcmp(*dataPos + objectSize, footer, frameWidth) != 0) return BAD_FORMAT;
    *buffInfo->offSet = 0;
    return VERIFIED;
}

static char *handle_fragments(char *dataPos, BuffData *buffInfo, size_t objectSize, size_t bufferDiff, DataCapsule *capsule) {
    *buffInfo->offSet = objectSize - buffInfo->remainingBytes;
    if(buffInfo->fragmented) buffInfo->fragPos = buffInfo->remainingBytes + bufferDiff;
    else buffInfo->fragPos = buffInfo->remainingBytes;

    buffInfo->suppBuff = malloc(objectSize + bufferDiff);
    if(buffInfo->suppBuff == NULL) {
        *buffInfo->offSet = 0;
        return NULL;
    }

    memcpy(buffInfo->suppBuff, dataPos, buffInfo->fragPos);
    if(buffInfo->fragmented) free(dataPos);
    if(recv_full(capsule->clientSock.socket, buffInfo->suppBuff + buffInfo->fragPos, *buffInfo->offSet, 0)) return NULL;

    buffInfo->remainingBytes = objectSize;
    buffInfo->fragmented = 1;
    return buffInfo->suppBuff;
}

static void make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule) {
    Action *actionBuffer = malloc(sizeof(Action));
    if(actionBuffer == NULL) {
        fprintf(stderr, "Discarding message: %s\n", ALLOC_ERR);
        return;
    }

    memcpy(&actionBuffer->userPacket, recvBuffer + frameWidth, dataSize);
    actionBuffer->userPacket.msg[BUFF_SIZE - 1] = '\0'; // TODO: Unpack msg into string and null terminate there instead
    strcpy(actionBuffer->actionAddr, capsule->clientSock.IPStr);
    enqueue(capsule->state->userActions, actionBuffer);
}

_Bool recv_full(int socket, char *buffer, size_t desiredSize, int flags) {
    size_t totalData = 0;
    while(totalData < desiredSize) {
        ssize_t received = recv(socket, buffer + totalData, desiredSize - totalData, flags);
        if(received < 1) return 1; // Check for bed recv call
        totalData += received;
    }
    return 0;
}

void send_keepalive(KeepAliveStat *connStatus, int clientSock) {
    connStatus->messageReceived = 1;
    if(connStatus->terminate) connStatus->kaOut[FRAME_SIZE] = CNN_DEAD;
    else connStatus->kaOut[FRAME_SIZE] = CNN_ALIVE;
    if(send(clientSock, connStatus->kaOut, connStatus->kaSize, MSG_NOSIGNAL) == -1) {
        connStatus->terminate = 1;
        shutdown(clientSock, SHUT_RDWR);
    }
}