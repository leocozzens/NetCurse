#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFF_SIZE 256

int main(void) {
    // Create a socket
    int netSock = socket(AF_INET, SOCK_STREAM, 0);

    // Specify an address for the socket
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; // IPV4
    serverAddr.sin_port = htons(9002);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(connect(netSock, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) {
        perror("SOCKET ERROR");
        exit(-1);
    }

    // Receive data from the server
    char recvBuff[BUFF_SIZE];
    recv(netSock, recvBuff, sizeof(recvBuff), 0);

    printf("Data received:\n%s\n", recvBuff);

    close(netSock);
    return 0;
}