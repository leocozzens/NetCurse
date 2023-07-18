// C standard libraries
#include <unistd.h>
#include <errno.h>

// Local headers
#include <input.h>
#include <ctasks.h>
#include <utils.h>

int main(int argc, char **argv) { // TODO: Make terminal options
    signal(SIGINT, graceful_close);
    ConnData connInfo;
    char serverIP[INET_ADDRSTRLEN];

    if(argc < 2) strcpy(serverIP, "0.0.0.0");
    else strncpy(serverIP, argv[1], INET_ADDRSTRLEN - 1);

    connect_to_server(serverIP, &connInfo);

    size_t packetSize;
    char *outFrame = make_packet(USERDATA_SIZE, FRAME_SIZE, USERDATA_HEADER, USERDATA_FOOTER, &packetSize);
    UserData *outData = (UserData*) (outFrame + FRAME_SIZE);

    pthread_t keepAlive;
    pthread_mutex_init(&connInfo.sendLock, NULL);
    pthread_create(&keepAlive, NULL, keepalive_loop, &connInfo);
    pthread_detach(keepAlive);
    while(1) {
        printf("-> ");
        get_input(outData->msg, BUFF_SIZE);

        pthread_mutex_lock(&connInfo.sendLock); // TODO: Check connection state before sending message, and attempt to restablish
        send(connInfo.clientSock, outFrame, packetSize, 0);
        pthread_mutex_unlock(&connInfo.sendLock);
        printf("Data sent:\n%s\n", outData->msg);
    }
}