//
// Created by natsa on 20/07/2021.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "LoggingHandler.h"
#include "FileLogger.h"

static pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t t1;

typedef struct MessageStruct MessageStruct;
static MessageStruct* endBuffer = 0;
static MessageStruct* currentBuffer = 0;
unsigned int bufferSize = 5;

struct MessageStruct{
    enum LogMsgType type;
    struct MessagePacket data;
    MessageStruct* next;
};

_Noreturn void* processData(void* unused);

void initBuffer(){
    endBuffer = calloc(1, sizeof (MessageStruct));
    currentBuffer = endBuffer;
    for (unsigned int i = 1; i < bufferSize; i++){
        currentBuffer->next = calloc(1, sizeof (MessageStruct));
        currentBuffer = currentBuffer->next;
    }
    currentBuffer->next = endBuffer;
    pthread_create(&t1, NULL, processData, NULL);
}

_Noreturn void* processData(void* unused){
    while (1){
        pthread_cond_wait(&condition, &mutex);
        while (currentBuffer->next != endBuffer){
            currentBuffer = currentBuffer->next;
            if(currentBuffer->type == RECV_MSG){
                printf("Received message type %#04x with length %u\r\n",
                       currentBuffer->data.type, currentBuffer->data.length);
            }else if(currentBuffer->type == SEND_MSG){
                printf("Sent message type %#04x with length %u\r\n",
                       currentBuffer->data.type, currentBuffer->data.length);
            }
            fileLoggerProcessMsg(currentBuffer->type, &currentBuffer->data);
        }
    }
}

void addData(enum LogMsgType type, struct MessagePacket* inPacket){
    endBuffer->type = type;
    endBuffer->data.type = inPacket->type;
    endBuffer->data.length = inPacket->length;
    memcpy(&endBuffer->data.message, inPacket->message, inPacket->length);
    endBuffer = endBuffer->next;
    pthread_cond_signal(&condition);
}