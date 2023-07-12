#include <utils.h>

void connect_to_server(const char *serverIP, ConnData *connInfo) {
    SADDR_IN serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(IN_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    connInfo->clientSock = socket(AF_INET, SOCK_STREAM, 0); // TODO: Add IPv6 support
    UTIL_CHECK(connInfo->clientSock, -1, "SOCKET socket");
    UTIL_CHECK(connect(connInfo->clientSock, (SADDR*) &serverAddr, serverAddrLen), -1, "SOCKET connect");
    set_sock_timeout(connInfo->clientSock, DEFAULT_WAIT_TIME, DEFAULT_WAIT_TIME_U);
    connInfo->connectionState = CNN_ALIVE;
}