// C standard libraries
#include <unistd.h>
#include <errno.h>

// Local headers
#include <input.h>
#include <ctasks.h>

int main(void) {
    HANDLE_SIGINT;
    ConnData connInfo;
    SADDR_IN serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(IN_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.11");

    connInfo.clientSock = socket(AF_INET, SOCK_STREAM, 0); // TODO: Add IPv6 support
    UTIL_CHECK(connInfo.clientSock, -1, "SOCKET socket");
    UTIL_CHECK(connect(connInfo.clientSock, (SADDR*) &serverAddr, serverAddrLen), -1, "SOCKET connect");
    set_sock_timeout(connInfo.clientSock, DEFAULT_WAIT_TIME, DEFAULT_WAIT_TIME_U);
    connInfo.connectionState = CNN_ALIVE;

    char outFrame[sizeof(UserData) + (FRAME_SIZE * 2)];
    strncpy(outFrame, USERDATA_HEADER, FRAME_SIZE);
    strncpy(outFrame + (FRAME_SIZE + sizeof(UserData)), USERDATA_FOOTER, FRAME_SIZE);
    UserData *outData = (UserData*) (outFrame + FRAME_SIZE);

    pthread_t heartBeat;
    pthread_mutex_init(&connInfo.sendLock, NULL);
    pthread_create(&heartBeat, NULL, beat_loop, &connInfo);
    pthread_detach(heartBeat);
    while(1) {
        printf("Enter the value you'd like to send to the server: ");
        get_input(outData->msg, BUFF_SIZE);

        pthread_mutex_lock(&connInfo.sendLock); // TODO: Check conenction state before sending message, and attempt to restablish
        send(connInfo.clientSock, outFrame, sizeof(outFrame), 0);
        pthread_mutex_unlock(&connInfo.sendLock);
        printf("Data sent:\n%s\n", outData->msg);
    }
}