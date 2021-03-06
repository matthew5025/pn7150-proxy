//
// Created by natsa on 23/06/2021.
//

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include "NfcHandler.h"
#include "linux_nfc_api.h"
#include "FileLogger.h"

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

nfcTagCallback_t g_TagCB;
nfc_tag_info_t g_tagInfos;

void (*onCardDepart)();

void (*onCardArrive)();

void onTagArrival(nfc_tag_info_t *pTagInfo) {
    g_tagInfos = *pTagInfo;
    unsigned int additionalInfoLen = 0;
    if (g_tagInfos.technology == 2) {
        additionalInfoLen = 11;

    } else if (g_tagInfos.technology == 1) {
        additionalInfoLen= 3;
    }



    memcpy(&sharedBuffer[0], &g_tagInfos.technology, 4);
    memcpy(&sharedBuffer[4], &g_tagInfos.uid_length, 4);
    memcpy(&sharedBuffer[8], g_tagInfos.uid, g_tagInfos.uid_length);
    memcpy(&sharedBuffer[8 + g_tagInfos.uid_length], &additionalInfoLen, 4);
    memcpy(&sharedBuffer[12 + g_tagInfos.uid_length], g_tagInfos.add_data, additionalInfoLen);

    sharedBufferLen = 12 + g_tagInfos.uid_length + additionalInfoLen;
    (*onCardArrive)();
}

void onReaderArrival(){
}

void setOnCardDepartCallback(void (*ptr)()) {
    onCardDepart = ptr;
}


void onTagDeparture(void) {
    (*onCardDepart)();
}

void setOnCardArrivalCallback(void (*ptr)()) {
    onCardArrive = ptr;
}

unsigned int sendTagCommand(unsigned char *result, unsigned int maxLen, unsigned int timeout) {
    unsigned int resultLen = 0;
    resultLen = nfcTag_transceive(g_tagInfos.handle, sharedBuffer, sharedBufferLen, result, maxLen, timeout);
    return resultLen;
}


void enableReader() {
    g_TagCB.onTagArrival = onTagArrival;
    g_TagCB.onTagDeparture = onTagDeparture;
    nfcManager_doInitialize();
    nfcManager_registerTagCallback(&g_TagCB);
    nfcManager_enableDiscovery(DEFAULT_NFA_TECH_MASK, 0x01, 0, 0);
}

void disableReader(){
    nfcManager_deregisterTagCallback();
    nfcManager_disableDiscovery();
    nfcManager_doDeinitialize();
    printf("Reader Stopped.\n");

}