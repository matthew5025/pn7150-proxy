//
// Created by natsa on 20/06/2021.
//

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "EmulationCommsHandler.h"
#include "NfcEmulHandler.h"

enum MessageType{
    ECHO = 0x01,
    ECHO_REPLY = 0x02,
    TAG_INFO_REQ = 0x03,
    TAG_INFO_REPLY = 0x04,
    TAG_CMD = 0x05,
    TAG_CMD_REPLY = 0x06
};

enum ConnectionState{NEW_CONN, CONN_EST, CARD_RDY};

unsigned char inBuffer[1024];
unsigned char outBuffer[1024];
unsigned char tempBuffer[512];
int comSocket = -1;
unsigned char header[] = {0xAB, 0xBB, 0xCB};

struct MessagePacket {
    unsigned char type;
    uint16_t length;
    unsigned char message [512];
};

struct MessagePacket inputMessage, outputMessage;
enum ConnectionState connectionState = NEW_CONN;

int checkHeader(unsigned char *input) {
    return memcmp(input, header, 3);
}

int sendMessage() {
    memcpy(outBuffer, header, 3);
    outBuffer[3] = outputMessage.type;
    memcpy(&outBuffer[4], &outputMessage.length, sizeof (uint16_t));
    memcpy(outBuffer + 6, outputMessage.message, outputMessage.length);
    printf("Sending message of type %#04x with length %u\r\n", outputMessage.type, outputMessage.length);
    return send(comSocket, outBuffer, outputMessage.length + 6, 0);
}

void genRandomData(unsigned char* buffer, unsigned int bufferSize){
    srand(time(NULL));
    for (int i = 0; i < bufferSize; i++) {
        buffer[i] = rand() % 256;
    }
}

void sendTagCommand(){
    memcpy(outputMessage.message, sharedBuffer, sharedBufferLen);
    outputMessage.type = TAG_CMD;
    outputMessage.length = sharedBufferLen;
    sendMessage();
}


void initComms(){
    setOnDataCallback(sendTagCommand);
    outputMessage.type = ECHO;
    outputMessage.length = sizeof(outputMessage.message);
    genRandomData(outputMessage.message, outputMessage.length);
    sendMessage();
}



void messageHandler() {
    switch (inputMessage.type) {
        case ECHO:
            outputMessage.length = inputMessage.length;
            memcpy(outputMessage.message, inputMessage.message, inputMessage.length);
            outputMessage.type = ECHO_REPLY;
            break;
        case ECHO_REPLY:
            if(connectionState == NEW_CONN){
                printf("Communications established. Requesting for card info...\r\n");
                if(memcmp(inputMessage.message, outputMessage.message, sizeof(outputMessage.message)) == 0){
                    connectionState = CONN_EST;
                    outputMessage.length = 0;
                    outputMessage.type = TAG_INFO_REQ;
                }else{
                    return;
                }
            }else{
                return;
            }
        case TAG_INFO_REQ:
            break;
        case TAG_INFO_REPLY:
            sharedBufferLen = inputMessage.length;
            memcpy(&sharedBuffer, &inputMessage.message, sharedBufferLen);
            startEmulation();
            return;
        case TAG_CMD_REPLY:
            sharedBufferLen = inputMessage.length;
            memcpy(&sharedBuffer, &inputMessage.message, sharedBufferLen);
            hceResponse();
            return;

    }
    sendMessage();
}


int readSocket() {
    int bytesRead = recv(comSocket, inBuffer, 1024, 0);
    while (bytesRead < 5) {
        int messageLen = recv(comSocket, inBuffer + bytesRead, 1024 - bytesRead, 0);
        bytesRead = messageLen + bytesRead;
    }
    if (checkHeader(inBuffer) == 0) {
        inputMessage.type = inBuffer[3];
        inputMessage.length = *(uint16_t*)&inBuffer[4];
        if (inputMessage.length == 0) {
            memset(inputMessage.message, 0, sizeof (inputMessage.message));
        }
        int remainingBytes = inputMessage.length - (bytesRead - 6);
        while (remainingBytes > 0) {
            int messageLen = recv(comSocket, inBuffer + bytesRead, remainingBytes, 0);
            bytesRead = messageLen + bytesRead;
            remainingBytes = inputMessage.length - (bytesRead - 6);
        }
        memcpy(inputMessage.message, &inBuffer[6], inputMessage.length);
        printf("Got message of type %#04x with length %u\r\n", inputMessage.type, inputMessage.length);
        messageHandler();
    } else {
        return -1;
    }
    return 0;
}

void setSocket(int in_sock) {
    comSocket = in_sock;
}