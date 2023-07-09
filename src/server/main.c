#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <common.h>

#define CONNECTIONS 1

int main(void) {
    
    SADDR_IN serverAddr;
    SADDR_IN clientAddr;
    int serverSock;
    int clientSock;
    socklen_t serverAddrLen = sizeof(serverAddr);
    socklen_t clientAddrLen = sizeof(clientAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(IN_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    char recvBuff[BUFF_SIZE];

    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    bind(serverSock, (SADDR*) &serverAddr, serverAddrLen);
    listen(serverSock, CONNECTIONS);
    clientSock = accept(serverSock, (SADDR*) &clientAddr, &clientAddrLen);

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(clientAddr.sin_family, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Client connected to server from [%s]\n", clientIP);

    while(1) {
        recv(clientSock, recvBuff, sizeof(recvBuff), 0);
        printf("Message received from [%s]\nMSG: %s\n", clientIP, recvBuff);
    }
    close(serverSock);
    return 0;
}