//
// Created by natsa on 23/06/2021.
//

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include "NfcHandler.h"
#include "linux_nfc_api.h"

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

nfcTagCallback_t g_TagCB;
nfc_tag_info_t g_tagInfos;

void onTagArrival(nfc_tag_info_t *pTagInfo){
    g_tagInfos = *pTagInfo;
    unsigned int additionalInfoLen = 0;
    if(g_tagInfos.technology == 2){
        additionalInfoLen = 11;
    }
    memcpy(&sharedBuffer[0], &g_tagInfos.technology, 4);
    memcpy(&sharedBuffer[4], &g_tagInfos.uid_length, 4);
    memcpy(&sharedBuffer[8], g_tagInfos.uid, g_tagInfos.uid_length);
    memcpy(&sharedBuffer[8 + g_tagInfos.uid_length], &additionalInfoLen, 4);
    memcpy(&sharedBuffer[12 + g_tagInfos.uid_length], g_tagInfos.add_data, additionalInfoLen);

    sharedBufferLen = 12 + g_tagInfos.uid_length + additionalInfoLen;
    pthread_cond_signal(&condition);
}

void onTagDeparture(void){
}

unsigned int sendTagCommand(unsigned char* result, unsigned int maxLen, unsigned int timeout){
    unsigned int resultLen = 0;
    printf("Received data from reader : \n");
    for(int i = 0x00; i < sharedBufferLen; i++)
    {
        printf("%02X ", sharedBuffer[i]);
    }
    printf("\n\n");

    resultLen = nfcTag_transceive(g_tagInfos.handle, sharedBuffer, sharedBufferLen, result, maxLen, timeout);

    printf("Response sent : \n");
    for(int i = 0x00; i < resultLen; i++)
    {
        printf("%02X ", result[i]);
    }

    printf("\n\n");
    fflush(stdout);


    return resultLen;
}


void getTagInfo(){
    g_TagCB.onTagArrival = onTagArrival;
    g_TagCB.onTagDeparture = onTagDeparture;
    nfcManager_doDeinitialize();
    nfcManager_doInitialize();
    nfcManager_registerTagCallback(&g_TagCB);
    nfcManager_enableDiscovery(DEFAULT_NFA_TECH_MASK, 0x01, 0, 0);
    pthread_cond_wait(&condition, &mutex);
}