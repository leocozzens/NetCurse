// Local headers
#include <task_helpers.h>

#define CHECK_FRAME(_FrameState, _SuccessCode) if((_FrameState) == (VERIFIED)) return _SuccessCode; \
                                               else if((_FrameState) == (BAD_FORMAT)) return ENDMSG_CODE

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
    strcpy(actionBuffer->actionAddr, capsule->clientSock.IPStr);
    enqueue(capsule->state->userActions, actionBuffer);
    return 0;
}