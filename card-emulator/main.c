#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "EmulationCommsHandler.h"
#include <string.h>

#define PORT 8096

unsigned int len;
unsigned char buffer[100];
unsigned char startMessage[] = {0xBB, 0xAB, 0xDB, 0xCB, 0xEB, 0xFB,
                                0xAA, 0xBA, 0xCA, 0xDA, 0xEA, 0xFA};

int checkStartMessage(unsigned char *input) {
    return memcmp(input, startMessage, 12);
}

int main(int argc, char const *argv[]) {
    int sock;
    struct sockaddr_in serverAddr;
    unsigned int serverLen;
    struct hostent *server;
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            printf("\n Socket creation error \n");
            return -1;
        }

        bzero((char *) &serverAddr, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "192.168.4.1", &serverAddr.sin_addr.s_addr) <= 0) {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        printf("Connecting...\r\n");
        if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
            printf("Connection Failed \n");
            sleep(3);
        } else {

            if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
                perror("timeout set failed");
            }

            while (1) {
                len = send(sock, startMessage, sizeof(startMessage), 0);
                printf("Start message sent!\r\n");
                if (len != -1) {
                    len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &serverAddr, &serverLen);
                    if (len == 12) {
                        if (checkStartMessage(buffer) == 0) {
                            break;
                        } else {
                            printf("Invalid start sequence.\r\n");
                        }
                    } else {
                        perror("Error receiving start sequence");
                    }
                } else {
                    perror("Error sending start sequence");
                }
                sleep(1);
            }

            timeout.tv_sec = 0;
            timeout.tv_usec = 0;

            if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
                perror("timeout set failed");
            }


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
