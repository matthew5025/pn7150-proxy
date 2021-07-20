//
// Created by natsa on 12/07/2021.
//

#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include "CompanionController.h"

#define PORT 8097

_Noreturn void* startCompanionThread(void* thr_data){
    printf("%s\n", (char *)thr_data);
    int server_fd, new_socket = 0;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    while(1){
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            continue;
        }

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

        sleep(5);
        close(server_fd);
        close(new_socket);
    }

}
