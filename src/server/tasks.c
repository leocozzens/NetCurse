#include <tasks.h>

#define CONNECTIONS 5

#define CLOSE_RECEIVER(_IP, _Socket) printf("Closed connection to [%s]\n", (_IP)); \
                                              close(_Socket)

void *workerFunc(void *);


void *connection_loop(void *arg) {
    InitialInput *entry = arg;
    while(1) {
        DataCapsule *capsule = malloc(sizeof(DataCapsule)); // TODO: Remember to free
        ClientSockData *clientData = malloc(sizeof(ClientSockData));
        ServerSockData *serverData = malloc(sizeof(ServerSockData));

        capsule->userActions = &entry->userActions;
        strncpy(serverData->hostIP, entry->hostIP, INET_ADDRSTRLEN - 1);
        
        pthread_t recvThread;
        //pthread_t listenThread;

        capsule->clientData = clientData;
        capsule->serverData = serverData;
        capsule->recvThread = &recvThread;

        listen_for((void*) capsule);
        //pthread_create(&listenThread, NULL, listen_for, &capsule);
        //printf("\nWaiting for connections...\n");


        //pthread_join(listenThread, NULL);
    }
}

void* listen_for(void *arg) {
    DataCapsule *capsule = arg;

    SADDR_IN serverAddr;
    SADDR_IN clientAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);
    socklen_t clientAddrLen = sizeof(clientAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(IN_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(capsule->serverData->hostIP);

    capsule->serverData->serverSock = socket(AF_INET, SOCK_STREAM, 0);
    bind(capsule->serverData->serverSock, (SADDR*) &serverAddr, serverAddrLen);
    listen(capsule->serverData->serverSock, CONNECTIONS);
    capsule->clientData->clientSock = accept(capsule->serverData->serverSock, (SADDR*) &clientAddr, &clientAddrLen);

    inet_ntop(clientAddr.sin_family, &(clientAddr.sin_addr), capsule->clientData->clientIP, INET_ADDRSTRLEN);
    printf("Client connected to server from [%s]\n", capsule->clientData->clientIP);

    pthread_create(capsule->recvThread, NULL, receive_data, capsule);
    pthread_detach(*capsule->recvThread);
    return NULL;
}

void *receive_data(void *arg) {
    DataCapsule *data = arg;
    while(1) {
        UserData *userBuffer = malloc(sizeof(UserData));
        size_t retVal = recv(data->clientData->clientSock, userBuffer, sizeof(UserData), 0);
        if(retVal == -1) {
            perror("SOCKET ERROR");
            CLOSE_RECEIVER(data->clientData->clientIP, inet_addr(data->serverData->hostIP));
            return NULL;
        }
        else if(retVal == 0) {
            CLOSE_RECEIVER(data->clientData->clientIP, inet_addr(data->serverData->hostIP));
            return NULL;
        }
        printf("Message received from [%s]\nMSG: %s\n", data->clientData->clientIP, userBuffer->msg);
        enqueue(data->userActions, userBuffer);
    }
}