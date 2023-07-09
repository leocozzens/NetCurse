#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <common.h>

int main(void) {

    char outBuff[BUFF_SIZE];
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
        printf("Enter the vaue you'd like to send to the server: ");
        scanf("%255s", outBuff);
        send(netSock, outBuff, sizeof(outBuff), 0);

        printf("Data sent:\n%s\n", outBuff);
    }
    close(netSock);
    return 0;
}