//
// Created by natsa on 11/07/2021.
//

#include "FileLogger.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

FILE *fp = NULL;

void createFile(){
    if(fp != NULL){
        //printf("Attempting to create a new file when one already exists...\n");
        return;
    }
    char text[21];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    unsigned long length = strftime(text, sizeof(text)-1, "%Y-%m-%dT%H-%M-%S", t);
    text[length] = 0;
    fp = fopen(text, "w+");
}

void writeToFile(char * format, ...){
    if(fp == NULL){
        //printf("Attempting to write to nullptr...\n");
        return;
    }
    va_list args;
    va_start(args, format);
    vfprintf(fp, format, args);
    va_end(args);
}

void closeFile(){
    if(fp == NULL){
        //printf("Attempting to close nullptr...\n");
        return;
    }
    fclose(fp);
    fp = NULL;
}

void fileLoggerProcessMsg(enum LogMsgType type, struct MessagePacket* inPacket){
    if(inPacket->type == READER_ARRIVAL){
        createFile();
    } else if (inPacket->type == READER_GONE){
        closeFile();
    } else if (inPacket->type == TAG_INFO_REPLY){
        unsigned int tagTech;
        unsigned int uidLength;
        memcpy(&tagTech, inPacket->message, 4);
        memcpy(&uidLength, &inPacket->message[4], 4);
        if(tagTech == 1){
            writeToFile("Card Type: ISO14443A ");
        } else if(tagTech == 2){
            writeToFile("Card Type: ISO14443B ");
        }
        writeToFile("Card UID: ");
        for(int i = 0; i < uidLength; i++){
            writeToFile("%02X", inPacket->message[8 + i]);
        }
        writeToFile("\r\n");
    }else if(inPacket->type == TAG_CMD){
        writeToFile("Received data from reader: \r\n");
        for (int i = 0x00; i < inPacket->length; i++) {
            writeToFile("%02X", inPacket->message[i]);
        }
        writeToFile("\r\n\r\n");
    } else if(inPacket->type == TAG_CMD_REPLY){
        writeToFile("Response sent: \r\n");
        for (int i = 0x00; i < inPacket->length; i++) {
            writeToFile("%02X", inPacket->message[i]);
        }
        writeToFile("\r\n\r\n");
    }
}