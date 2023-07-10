// POSIX headers
#include <pthread.h>

// C standard libraries
#include <unistd.h>
#include <errno.h>

// Local headers
#include <model.h>
#include <input.h>
#include <ctasks.h>

int main(void) {
    HANDLE_SIGINT;
    int clientSock;
    SADDR_IN serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(IN_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.11");

    clientSock = socket(AF_INET, SOCK_STREAM, 0); // TODO: Add IPv6 support
    UTIL_CHECK(clientSock, -1, "SOCKET socket");
    UTIL_CHECK(connect(clientSock, (struct sockaddr*) &serverAddr, serverAddrLen), -1, "SOCKET connect");

    UserData outData;
    while(!check_SIGINT()) {
        pthread_t sigChecker;
        pthread_create(&sigChecker, NULL, lookup_loop, &clientSock);
        printf("Enter the vaue you'd like to send to the server: ");
        get_input(outData.msg, BUFF_SIZE);
        pthread_cancel(sigChecker);
        pthread_detach(sigChecker); // Necessary for system to clean up local thread stack
        send(clientSock, &outData, sizeof(outData), 0);

        printf("Data sent:\n%s\n", outData.msg);
    }
    CLOSE_NOW(clientSock); // TODO: Send message that indicates socket closure
}