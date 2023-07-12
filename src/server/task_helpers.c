// Local headers
#include <task_helpers.h>

#define CHECK_FRAME(_FrameState, _SuccessCode) if((_FrameState) == (VERIFIED)) return _SuccessCode; \
                                               else if((_FrameState) == (BAD_FORMAT)) return ENDMSG_CODE

void interpret_msg(size_t retVal, const char *recvBuffer, DataCapsule *capsule, _Bool *terminate) {
    size_t remainingBytes = retVal;

    while(remainingBytes > (FRAME_SIZE * 2)) {
        size_t headerPos = retVal - remainingBytes;
        switch(detect_msg_type(&remainingBytes, (recvBuffer + headerPos), capsule)) {
            case TERMINATE_CODE:
                *terminate = 1;
                goto ENDLOOP;
                break;
            case ENDMSG_CODE:
                goto ENDLOOP;
            case HEARTBEAT_CODE:
                if(recvBuffer[headerPos + FRAME_SIZE] != CNN_ALIVE) {
                    *terminate = 1;
                    goto ENDLOOP;
                }
                break;
            case USERDATA_CODE:
                if(make_action((recvBuffer + headerPos), FRAME_SIZE, USERDATA_SIZE, capsule)) {
                    *terminate = 1;
                    goto ENDLOOP;
                }
                break;
        }
    }
    ENDLOOP:
}

MessageType detect_msg_type(size_t *remainingBytes, const char *recvBuffer, DataCapsule *capsule) {
    FrameCode frameState;

    frameState = verify_frame(recvBuffer, remainingBytes, FRAME_SIZE, BEAT_SIZE, HEARTBEAT_HEADER, HEARTBEAT_FOOTER);
    CHECK_FRAME(frameState, HEARTBEAT_CODE);

    frameState = verify_frame(recvBuffer, remainingBytes, FRAME_SIZE, USERDATA_SIZE, USERDATA_HEADER, USERDATA_FOOTER);
    CHECK_FRAME(frameState, USERDATA_CODE);
    return ENDMSG_CODE;
}

FrameCode verify_frame(const char *recvBuffer, size_t *remainingBytes, size_t frameWidth, size_t dataSize, const char *header, const char *footer) {
    if((strncmp(recvBuffer, header, frameWidth) == 0)) {
        *remainingBytes -= frameWidth;
        if(*remainingBytes < (frameWidth + dataSize)) return BAD_FORMAT;
        if(strncmp(recvBuffer + frameWidth + dataSize, footer, frameWidth) != 0) return BAD_FORMAT;
        *remainingBytes -= frameWidth + dataSize;
        return VERIFIED;
    }
    return WRONG_HEADER;
}

_Bool make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule) {
    Action *actionBuffer = malloc(sizeof(Action));
    if(actionBuffer == NULL) return 1;

    memcpy(&actionBuffer->userPacket, recvBuffer + frameWidth, dataSize);
    strcpy(actionBuffer->actionAddr, capsule->clientSock->IPStr);
    pthread_mutex_lock(capsule->state->userActions->tailLock);
    enqueue(capsule->state->userActions, actionBuffer);
    pthread_mutex_unlock(capsule->state->userActions->tailLock);
    return 0;
}