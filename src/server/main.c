#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFF_SIZE 256
#define CONNECTIONS 1

int main(void) {
    char servMsg[BUFF_SIZE] = "Server reached!";

    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9002);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSock, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    listen(serverSock, CONNECTIONS);

    int clientSock = accept(serverSock, NULL, NULL);

    send(clientSock, servMsg, sizeof(servMsg), 0);
    close(serverSock);
    return 0;
}