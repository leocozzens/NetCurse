// Local headers
#include <task_helpers.h>

#define CHECK_FRAME(_FrameState, _SuccessCode) if((_FrameState) == (VERIFIED)) return _SuccessCode; \
                                               else if((_FrameState) == (BAD_FORMAT)) return ENDMSG_CODE

void interpret_msg(char *recvBuffer, size_t buffSize, ssize_t retVal, size_t *offSet, _Bool *terminate, DataCapsule *capsule) {
    *offSet = 0;
    BuffData buffInfo = { offSet, retVal, 0 };

    while(buffInfo.remainingBytes > 0) {
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
                if(tempBuff[FRAME_SIZE] != CNN_ALIVE) *terminate = 1;
                if(buffInfo.fragmented) {
                    free(buffInfo.suppBuff);
                    buffInfo.fragmented = 0;
                    goto ENDLOOP;
                }
                if(*terminate) goto ENDLOOP;
                break;
            case USERDATA_CODE:
                if(buffInfo.fragmented) {
                    make_action(buffInfo.suppBuff, FRAME_SIZE, USERDATA_SIZE, capsule);
                    free(buffInfo.suppBuff);
                    buffInfo.fragmented = 0;
                    goto ENDLOOP;
                }
                else make_action(recvBuffer + headerPos, FRAME_SIZE, USERDATA_SIZE, capsule);
                break;
        }
    }
    ENDLOOP:
    if(buffInfo.fragmented) {
        memcpy(recvBuffer, buffInfo.suppBuff + buffInfo.fragPos, *offSet);
        free(buffInfo.suppBuff);
    }
}

MessageType detect_msg_type(char *recvBuffer, BuffData *buffInfo, DataCapsule *capsule) {
    FrameCode frameState;
    char *dataPos = recvBuffer;
    frameState = verify_frame(&dataPos, buffInfo, FRAME_SIZE, BEAT_SIZE, HEARTBEAT_HEADER, HEARTBEAT_FOOTER, capsule);
    CHECK_FRAME(frameState, HEARTBEAT_CODE);

    frameState = verify_frame(&dataPos, buffInfo, FRAME_SIZE, USERDATA_SIZE, USERDATA_HEADER, USERDATA_FOOTER, capsule);
    CHECK_FRAME(frameState, USERDATA_CODE);
    return ENDMSG_CODE;
}

FrameCode verify_frame(char **dataPos, BuffData *buffInfo, size_t frameWidth, size_t dataSize, const char *header, const char *footer, DataCapsule *capsule) {
    size_t messageDiff = 0;
    size_t objectSize;

    objectSize = frameWidth;
    if(buffInfo->remainingBytes < objectSize) {
        if((strncmp(*dataPos, header, buffInfo->remainingBytes) != 0)) return WRONG_HEADER;
        *dataPos = handle_fragments(*dataPos, buffInfo, objectSize, messageDiff, capsule);
        buffInfo->fragmented = 1;
    }
    if((strncmp(*dataPos, header, frameWidth) != 0)) return WRONG_HEADER;
    buffInfo->remainingBytes -= objectSize;
    messageDiff += objectSize;

    objectSize = frameWidth + dataSize;
    if(buffInfo->remainingBytes < objectSize) {
        *dataPos = handle_fragments(*dataPos, buffInfo, objectSize, messageDiff, capsule);
        buffInfo->fragmented = 1;
    }
    buffInfo->remainingBytes -= objectSize;
    messageDiff += objectSize;

    if(strncmp(*dataPos + objectSize, footer, frameWidth) != 0) return BAD_FORMAT;
    *buffInfo->offSet = 0;
    return VERIFIED;
}

char *handle_fragments(char *dataPos, BuffData *buffInfo, size_t objectSize, size_t bufferDiff, DataCapsule *capsule) {
    *buffInfo->offSet = objectSize - buffInfo->remainingBytes;
    if(buffInfo->fragmented) buffInfo->fragPos = buffInfo->remainingBytes + bufferDiff;
    else buffInfo->fragPos = buffInfo->remainingBytes;

    buffInfo->suppBuff = malloc(objectSize + bufferDiff);
    MEM_ERROR(buffInfo->suppBuff, ALLOC_ERR);

    memcpy(buffInfo->suppBuff, dataPos, buffInfo->fragPos);
    if(recv_full(capsule->clientSock.socket, buffInfo->suppBuff + buffInfo->fragPos, *buffInfo->offSet, 0)) {
        if(buffInfo->fragmented) free(buffInfo->suppBuff);
        CLOSE_RECEIVER(capsule->clientSock.IPStr);
        free(capsule);
        pthread_exit(NULL);
    }

    if(buffInfo->fragmented) free(dataPos);
    buffInfo->remainingBytes = objectSize;
    return buffInfo->suppBuff;
}

_Bool recv_full(int socket, char *buffer, size_t desiredSize, int flags) {
    size_t totalData = 0;
    while(totalData < desiredSize) {
        ssize_t received = recv(socket, buffer + totalData, desiredSize - totalData, flags);
        if(received < 0) {
            perror("SOCKET recv");
            return 1;
        }
        else if(received == 0) return 1;
        totalData += received;
    }
    return 0;
}

void make_action(const char *recvBuffer, size_t frameWidth, size_t dataSize, DataCapsule *capsule) {
    Action *actionBuffer = malloc(sizeof(Action));
    MEM_ERROR(actionBuffer, ALLOC_ERR);

    memcpy(&actionBuffer->userPacket, recvBuffer + frameWidth, dataSize);
    actionBuffer->userPacket.msg[BUFF_SIZE - 1] = '\0';
    strcpy(actionBuffer->actionAddr, capsule->clientSock.IPStr);
    enqueue(capsule->state->userActions, actionBuffer);
}