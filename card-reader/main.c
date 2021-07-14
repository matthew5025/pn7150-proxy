#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "CommsHandler.h"
#include "CompanionController.h"
#include <threads.h>
#include <string.h>

#define PORT 8096

int main(int argc, char const *argv[]) {
    int server_fd, new_socket = 0;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    thrd_t companionThread;
    strcpy(companionStruct.test, "TEST");
    thrd_create(&companionThread, threadStart, &companionStruct);
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            continue;
        }

        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            perror("setsockopt");
            continue;
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
            perror("bind failed");
            continue;
        }
        if (listen(server_fd, 3) < 0) {
            perror("listen");
            continue;
        }
        printf("Waiting for connection...\r\n");

        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
            perror("accept");
            continue;
        }
        printf("Connected\r\n");
        setSocket(new_socket);

        while (1) {
            int response = readSocket();
            if (response == -1) {
                close(new_socket);
                close(server_fd);
                printf("Client Disconnected.\n");
                break;
            }
        }
    }
#pragma clang diagnostic pop
    return 0;
}
