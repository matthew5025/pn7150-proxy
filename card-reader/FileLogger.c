//
// Created by natsa on 11/07/2021.
//

#include "FileLogger.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

FILE *fp = NULL;

void createFile(){
    if(fp != NULL){
        printf("Attempting to create a new file when one already exists...\n");
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
        printf("Attempting to write to nullptr...\n");
        return;
    }
    va_list args;
    va_start(args, format);
    vfprintf(fp, format, args);
    va_end(args);
}

void closeFile(){
    if(fp == NULL){
        printf("Attempting to close nullptr...\n");
        return;
    }
    fclose(fp);
    fp = NULL;
}