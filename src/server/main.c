#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <common.h>

#define CONNECTIONS 1

#define CLOSED(_IP) printf("Closed connection to [%s]\n", _IP)

typedef struct {
    int clientSock;
    char *clientIP;
    char *recvBuff;
    int buffSize;
} SockData;

void *receive_data(void *arg);

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

    SockData data = { clientSock, clientIP, recvBuff, BUFF_SIZE };
    pthread_t recvThread;
    pthread_create(&recvThread, NULL, receive_data, &data);

    pthread_join(recvThread, NULL);
    close(serverSock);
    return 0;
}

void *receive_data(void *arg) {
    SockData *data = arg;
    while(1) {
        size_t retVal = recv(data->clientSock, data->recvBuff, data->buffSize, 0);
        if(retVal == -1) {
            perror("SOCKET ERROR");
            CLOSED(data->clientIP);
            return NULL;
        }
        else if(retVal == 0) {
            CLOSED(data->clientIP);
            return NULL;
        }
        printf("Message received from [%s]\nMSG: %s\n", data->clientIP, data->recvBuff);
    }
}