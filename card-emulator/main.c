#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "EmulationCommsHandler.h"

#define PORT 8096

int main(int argc, char const *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Socket creation error \n");
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "192.168.4.1", &serv_addr.sin_addr) <= 0) {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        printf("Connecting...\r\n");
        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            printf("Connection Failed \n");
            sleep(3);
        } else {
            printf("Connected\r\n");
            setSocket(sock);
            int resp = initComms();
            if (resp == -1) {
                printf("Disconnected!\n");
                break;
            }
            while (1) {
                int response = readSocket();
                if (response == -1) {
                    close(sock);
                    printf("Disconnected!\n");
                    break;
                }
            }

        }
    }
#pragma clang diagnostic pop
    return 0;
}
