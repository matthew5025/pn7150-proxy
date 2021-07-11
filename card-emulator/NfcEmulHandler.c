//
// Created by natsa on 27/06/2021.
//

#include "NfcEmulHandler.h"
#include "linux_nfc_api.h"
#include <pthread.h>
#include <malloc.h>
#include <string.h>

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

nfcHostCardEmulationCallback_t g_HceCB;
unsigned char g_readerMode = 0x00;
unsigned char HCE_response[255];
unsigned char HCE_response_len;
void (*onDataCallback)();
void (*onReaderGoneCallback)();

void setOnDataCallback(void (*ptr)()){
    onDataCallback = ptr;
}

void setOnReaderGoneCallback(void (*ptr)()){
    onReaderGoneCallback = ptr;
}

void onHostCardEmulationActivated(unsigned char mode)
{
    printf("Card activated\n");
    g_readerMode = mode;
    switch(mode)
    {
        case MODE_LISTEN_A: printf("Remote reader is type A\n");
            break;
        case MODE_LISTEN_B: printf("Remote reader is type B\n");
            break;
        case MODE_LISTEN_F: printf("Remote reader is type F\n");
            break;
        default: printf(" Remote reader type is unknown\n");
            break;
    }
}

void onHostCardEmulationDeactivated()
{
    printf("Card deactivated\n");
    g_readerMode = 0x00;
    (*onReaderGoneCallback)();
}



void onDataReceived(unsigned char *data, unsigned int data_length)
{
    memcpy(sharedBuffer, data, data_length);
    sharedBufferLen = data_length;
    (*onDataCallback)();
}

void hceResponse(){
    nfcHce_sendCommand(sharedBuffer, sharedBufferLen);

}


void startEmulation(){
    g_HceCB.onDataReceived = onDataReceived;
    g_HceCB.onHostCardEmulationActivated = onHostCardEmulationActivated;
    g_HceCB.onHostCardEmulationDeactivated = onHostCardEmulationDeactivated;
    unsigned int cardTech = *(unsigned int *)&sharedBuffer;
    unsigned long techMask = 0x00;
    unsigned int uidLen = *(unsigned int *)&sharedBuffer[4];
    unsigned int addInfoLen = *(unsigned int *)&sharedBuffer[8 + uidLen];
    if(cardTech == 2){
        techMask = 0x02;
        setConfigValue("HOST_LISTEN_TECH_MASK", &techMask, sizeof (techMask));
        nfcManager_doInitialize();
        nfcHce_registerHceCallback(&g_HceCB);
        nfcManager_setConfig(0x39, uidLen, &sharedBuffer[8]);
        nfcManager_setConfig(0x3A, 4, &sharedBuffer[12 + uidLen + 4]);

    }else if(cardTech == 1){
        techMask = 0x01;
        setConfigValue("HOST_LISTEN_TECH_MASK", &techMask, sizeof (techMask));
        nfcManager_doInitialize();
        nfcHce_registerHceCallback(&g_HceCB);
    }

    nfcManager_enableDiscovery(0x00, 0, 1, 0);

}

void endEmulation(){
    nfcHce_deregisterHceCallback();
    nfcManager_disableDiscovery();
    nfcManager_doDeinitialize();
}