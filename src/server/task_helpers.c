// Local headers
#include <task_helpers.h>

#define CHECK_FRAME(_FrameState, _SuccessCode) if((_FrameState) == (VERIFIED)) return _SuccessCode; \
                                               else if((_FrameState) == (BAD_FORMAT)) return ENDMSG_CODE

void interpret_msg(char *recvBuffer, size_t buffSize, ssize_t retVal, size_t *offSet, _Bool *terminate, DataCapsule *capsule) {
    *offSet = 0;
    BuffData buffInfo = { offSet, retVal, 0 };

    while(buffInfo.remainingBytes >= FRAME_SIZE) {
        size_t headerPos = retVal - buffInfo.remainingBytes;
        switch(detect_msg_type((recvBuffer + headerPos), &buffInfo, capsule)) {
            case TERMINATE_CODE:
                *terminate = 1;
                goto ENDLOOP;
            case ENDMSG_CODE:
                goto ENDLOOP;
            case HEARTBEAT_CODE:
                char *tempBuff = recvBuffer + headerPos;
                if(buffInfo.fragmented) tempBuff = buffInfo.suppBuff;
                if(tempBuff[FRAME_SIZE] != CNN_ALIVE) {
                    *terminate = 1;
                    goto ENDLOOP;
                }
                if(buffInfo.fragmented) {
                    free(buffInfo.suppBuff);
                    goto ENDLOOP;
                }
                break;
            case USERDATA_CODE:
                if(buffInfo.fragmented) {
                    make_action(buffInfo.suppBuff, FRAME_SIZE, USERDATA_SIZE, capsule);
                    free(buffInfo.suppBuff);
                    goto ENDLOOP;
                }
                else make_action(recvBuffer + headerPos, FRAME_SIZE, USERDATA_SIZE, capsule);
                break;
        }
    }
    ENDLOOP:
    if(*offSet > 0) {
        memcpy(recvBuffer, buffInfo.suppBuff + FRAME_SIZE + buffInfo.remainingBytes, *offSet);
        free(buffInfo.suppBuff);
    }
}

MessageType detect_msg_type(const char *recvBuffer, BuffData *buffInfo, DataCapsule *capsule) {
    FrameCode frameState;

    frameState = verify_frame(recvBuffer, buffInfo, FRAME_SIZE, BEAT_SIZE, HEARTBEAT_HEADER, HEARTBEAT_FOOTER, capsule);
    CHECK_FRAME(frameState, HEARTBEAT_CODE);

    frameState = verify_frame(recvBuffer, buffInfo, FRAME_SIZE, USERDATA_SIZE, USERDATA_HEADER, USERDATA_FOOTER, capsule);
    CHECK_FRAME(frameState, USERDATA_CODE);
    return ENDMSG_CODE;
}

FrameCode verify_frame(const char *recvBuffer, BuffData *buffInfo, size_t frameWidth, size_t dataSize, const char *header, const char *footer, DataCapsule *capsule) {
    const char *dataPos = recvBuffer;

    if((strncmp(recvBuffer, header, frameWidth) != 0)) return WRONG_HEADER; // TODO: Handle a fragmented header
    buffInfo->remainingBytes -= frameWidth;

    if(buffInfo->remainingBytes < (frameWidth + dataSize)) {
        *buffInfo->offSet = (frameWidth + dataSize) - buffInfo->remainingBytes;
        buffInfo->suppBuff = malloc((frameWidth * 2) + dataSize);
        MEM_ERROR(buffInfo->suppBuff, ALLOC_ERR);
        
        memcpy(buffInfo->suppBuff, recvBuffer, frameWidth + buffInfo->remainingBytes);
        recv(capsule->clientSock.socket, buffInfo->suppBuff + frameWidth + buffInfo->remainingBytes, *buffInfo->offSet, 0);

        buffInfo->fragmented = 1;
        dataPos = buffInfo->suppBuff;
    }
    else buffInfo->remainingBytes -= frameWidth + dataSize;

    if(strncmp(dataPos + frameWidth + dataSize, footer, frameWidth) != 0) return BAD_FORMAT;
    *buffInfo->offSet = 0;
    return VERIFIED;
}

void make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule) {
    Action *actionBuffer = malloc(sizeof(Action));
    MEM_ERROR(actionBuffer, ALLOC_ERR);

    memcpy(&actionBuffer->userPacket, recvBuffer + frameWidth, dataSize);
    strcpy(actionBuffer->actionAddr, capsule->clientSock.IPStr);
    enqueue(capsule->state->userActions, actionBuffer);
}