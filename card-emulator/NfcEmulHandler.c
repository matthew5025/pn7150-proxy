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
void (*onReaderArriveCallback)();

void setOnDataCallback(void (*ptr)()){
    onDataCallback = ptr;
}

void setOnReaderGoneCallback(void (*ptr)()){
    onReaderGoneCallback = ptr;
}

void setOnReaderArriveCallback(void (*ptr)()){
    onReaderArriveCallback = ptr;
}

void onHostCardEmulationActivated(unsigned char mode)
{
    printf("Card activated\n");
    (*onReaderArriveCallback)();
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
        printf("Emulation Started. Card Type: ISO14443B ");
    }else if(cardTech == 1){
        techMask = 0x01;
        setConfigValue("HOST_LISTEN_TECH_MASK", &techMask, sizeof (techMask));
        nfcManager_doInitialize();
        nfcHce_registerHceCallback(&g_HceCB);
        printf("Emulation Started. Card Type: ISO14443A ");

    }

    printf("Card UID: ");
    for(int i = 0; i< uidLen; i++){
        printf("%02X", sharedBuffer[8 + i]);
    }
    printf("\n");

    nfcManager_enableDiscovery(0x00, 0, 1, 0);

}

void endEmulation(){
    nfcHce_deregisterHceCallback();
    nfcManager_disableDiscovery();
    nfcManager_doDeinitialize();
    printf("Emulation Stopped.\n");
}