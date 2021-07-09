//
// Created by natsa on 20/06/2021.
//

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "CommsHandler.h"
#include "NfcHandler.h"
#include <stdint.h>
#include <stdio.h>

enum MessageType{
    ECHO = 0x01,
    ECHO_REPLY = 0x02,
    TAG_INFO_REQ = 0x03,
    TAG_INFO_REPLY = 0x04,
    TAG_CMD = 0x05,
    TAG_CMD_REPLY = 0x06,
    CARD_GONE = 0x07,
    READER_GONE = 0x08
};

unsigned char inBuffer[1024];
unsigned char outBuffer[1024];
int comSocket = -1;
unsigned char header[] = {0xAB, 0xBB, 0xCB};

struct MessagePacket {
    unsigned char type;
    uint16_t length;
    unsigned char message [512];
};

struct MessagePacket inputMessage, outputMessage;


int checkHeader(unsigned char *input) {
    return memcmp(input, header, 3);
}

long sendMessage() {
    memcpy(outBuffer, header, 3);
    outBuffer[3] = outputMessage.type;
    memcpy(&outBuffer[4], &outputMessage.length, sizeof (uint16_t));
    memcpy(outBuffer + 6, outputMessage.message, outputMessage.length);
    printf("Sending message of type %#04x with length %u\r\n", outputMessage.type, outputMessage.length);
    return send(comSocket, outBuffer, outputMessage.length + 6, 0);
}

void sendCardDeparted(){
    outputMessage.length = 0;
    outputMessage.type = CARD_GONE;
    sendMessage();
}

void sendCardArrival(){
    outputMessage.type = TAG_INFO_REPLY;
    outputMessage.length = sharedBufferLen;
    memcpy(outputMessage.message, sharedBuffer, sharedBufferLen);
    sendMessage();
}

void messageHandler() {
    switch (inputMessage.type) {
        case ECHO:
            outputMessage.length = inputMessage.length;
            memcpy(outputMessage.message, inputMessage.message, inputMessage.length);
            outputMessage.type = ECHO_REPLY;
            enableReader();
            setOnCardDepartCallback(sendCardDeparted);
            setOnCardArrivalCallback(sendCardArrival);
            break;
        case TAG_CMD:
            sharedBufferLen = inputMessage.length;
            memcpy(sharedBuffer, inputMessage.message, sharedBufferLen);
            outputMessage.length = sendTagCommand(outputMessage.message, sizeof (outputMessage.message), 500);
            outputMessage.type = TAG_CMD_REPLY;
            break;
    }
    sendMessage();
}


int readSocket() {
    long bytesRead = recv(comSocket, inBuffer, 1024, 0);
    while (bytesRead < 5) {
        long messageLen = recv(comSocket, inBuffer + bytesRead, 1024 - bytesRead, 0);
        if (messageLen < 1) {
            return -1;
        }
        bytesRead = messageLen + bytesRead;
    }
    if (checkHeader(inBuffer) == 0) {
        inputMessage.type = inBuffer[3];
        inputMessage.length = *(uint16_t *) &inBuffer[4];
        if (inputMessage.length == 0) {
            memset(inputMessage.message, 0, sizeof(inputMessage.message));
        }
        long remainingBytes = inputMessage.length - (bytesRead - 6);
        while (remainingBytes > 0) {
            long messageLen = recv(comSocket, inBuffer + bytesRead, remainingBytes, 0);
            if (messageLen < 1) {
                return -1;
            }
            bytesRead = messageLen + bytesRead;
            remainingBytes = inputMessage.length - (bytesRead - 6);
        }
        memcpy(inputMessage.message, &inBuffer[6], inputMessage.length);
        printf("Got message of type %#04x with length %u\r\n", inputMessage.type, inputMessage.length);
        messageHandler();
    } else {
        printf("Invalid header.\n");
        return -1;
    }
    return 0;
}

void setSocket(int in_sock) {
    comSocket = in_sock;
}

