//
// Created by natsa on 20/06/2021.
//

#define _GNU_SOURCE

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "CommsHandler.h"
#include "NfcHandler.h"
#include "FileLogger.h"
#include <stdio.h>
#include <linux_nfc_api.h>
#include <signal.h>
#include "LoggingHandler.h"


unsigned char inBuffer[1024];
unsigned char outBuffer[1024];
int comSocket = -1;
unsigned char header[] = {0xAB, 0xBB, 0xCB};
unsigned char intercept[] = {0x02, 0x00, 0x00, 0x0A, 0x05};
unsigned char replace[] = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E};

struct MessagePacket inputMessage, outputMessage;


int checkHeader(unsigned char *input) {
    return memcmp(input, header, 3);
}

long sendMessage() {

    char* rplPtr = memmem(outputMessage.message, outputMessage.length, intercept, sizeof (intercept));
    if(rplPtr != NULL){
        memcpy(rplPtr, replace, sizeof (replace));
    }

    memcpy(outBuffer, header, 3);
    outBuffer[3] = outputMessage.type;
    memcpy(&outBuffer[4], &outputMessage.length, sizeof (uint16_t));
    memcpy(outBuffer + 6, outputMessage.message, outputMessage.length);
    //printf("Sending message of type %#04x with length %u\r\n", outputMessage.type, outputMessage.length);
    addData(SEND_MSG, &outputMessage);
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
            sendMessage();
            return;
        case TAG_CMD:
            sharedBufferLen = inputMessage.length;
            memcpy(sharedBuffer, inputMessage.message, sharedBufferLen);
            outputMessage.length = sendTagCommand(outputMessage.message, sizeof (outputMessage.message), 500);
            outputMessage.type = TAG_CMD_REPLY;
            sendMessage();
            return;
        case READER_ARRIVAL:
            onReaderArrival();
            return;
        case READER_GONE:
            return;
        default:
            return;
    }
}

//void sigpipe_handler(int unused)
//{
//    printf("Got SIGPIPE!\r\n");
//}


int readSocket() {
    long bytesRead = recv(comSocket, inBuffer, 1024, 0);
    while (bytesRead < 5) {
        long messageLen = recv(comSocket, inBuffer + bytesRead, 1024 - bytesRead, 0);
        if (messageLen < 1) {
            perror("Error retrieving message");
            disableReader();
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
                disableReader();
                return -1;
            }
            bytesRead = messageLen + bytesRead;
            remainingBytes = inputMessage.length - (bytesRead - 6);
        }
        memcpy(inputMessage.message, &inBuffer[6], inputMessage.length);
        addData(RECV_MSG, &inputMessage);
        //printf("Got message of type %#04x with length %u\r\n", inputMessage.type, inputMessage.length);
        messageHandler();
    } else {
        printf("Invalid header.\n");
        disableReader();
        return -1;
    }
    return 0;
}


void setSocket(int in_sock) {
    //sigaction(SIGPIPE, &(struct sigaction){sigpipe_handler}, NULL);
    comSocket = in_sock;
    disableReader();
}

