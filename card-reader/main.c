#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "CompanionController.h"
#include "CommsHandler.h"
#include "LoggingHandler.h"
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8096

unsigned char buffer[100];
char str[INET_ADDRSTRLEN];

struct sockaddr_in server;
struct sockaddr_in from;
unsigned int fromLen = sizeof from;

unsigned char startMessage[] = {0xBB, 0xAB, 0xDB, 0xCB, 0xEB, 0xFB,
                                0xAA, 0xBA, 0xCA, 0xDA, 0xEA, 0xFA};

unsigned char invalidMessage[] = {0x00, 0xA0, 0xB0, 0xC0, 0x0B};


int checkStartMessage(unsigned char *input) {
    return memcmp(input, startMessage, 12);
}

void *startNewConnection(void *args) {
    int socket = *(int *) args;
    setSocket(socket);
    while (1) {
        int response = readSocket();
        if (response == -1) {
            //close(new_socket);
            //close(server_fd);
            printf("Client Disconnected.\n");
            break;
        }
    }
    return NULL;
}

int main(int argc, char const *argv[]) {
    int opt = 1;
    initBuffer();
    int currSocket = -1;
    pthread_t t1, t2;
    char test [] = "test";
    pthread_create(&t2, NULL, startCompanionThread, &test);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
        int server_fd;
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
            perror("socket failed");
            continue;
        }

        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            perror("setsockopt");
            continue;
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;   /* ok from any machine */
        server.sin_port = htons(PORT);      /* specific port */

        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
            perror("bind failed");
            continue;
        }

        printf("Waiting for connection...\r\n");

        while (1) {
            long n = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &from, &fromLen);
            if (n == 12 && checkStartMessage(buffer) == 0) {
                sendto(server_fd, startMessage, sizeof(startMessage), 0,
                       (const struct sockaddr *) &from, sizeof(struct sockaddr_in));
                if (connect(server_fd, (struct sockaddr *) &from, sizeof(from)) < 0) {
                    printf("Connection Failed \n");
                    continue;
                }
                break;
            } else {
                printf("Invalid start sequence.\r\n");
                sendto(server_fd, invalidMessage, sizeof(invalidMessage), 0,
                       (const struct sockaddr *) &from, sizeof(struct sockaddr_in));

            }
        }

        if(currSocket != -1){
            printf("Terminating previous connection.\r\n");
            close(currSocket);
            pthread_cancel(t1);
            //pthread_join(t1, NULL);
        }

        currSocket = server_fd;
        inet_ntop(AF_INET, &(from.sin_addr), str, INET_ADDRSTRLEN);
        printf("Connected to %s\r\n", str);
        pthread_create(&t1, NULL, startNewConnection, &currSocket);
    }
#pragma clang diagnostic pop
    return 0;
}
