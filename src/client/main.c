#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <common.h>

int main(void) {

    SADDR_IN serverAddr;
    int netSock;
    socklen_t serverAddrLen = sizeof(serverAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(IN_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    netSock = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(netSock, (struct sockaddr*) &serverAddr, serverAddrLen) == -1) {
        perror("SOCKET ERROR");
        exit(-1);
    }

    while(1) {
        char recvBuff[BUFF_SIZE];
        recv(netSock, recvBuff, sizeof(recvBuff), 0);

        printf("Data received:\n%s\n", recvBuff);
    }
    close(netSock);
    return 0;
}