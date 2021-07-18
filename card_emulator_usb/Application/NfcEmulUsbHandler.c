//
// Created by natsa on 15/07/2021.
//

#include <Nfc.h>
#include <tool.h>
#include <time.h>
#include <pthread.h>
#include <asm-generic/errno.h>
#include <tml_hid.h>
#include "NfcEmulUsbHandler.h"

struct cardEmulationState ceState;
NxpNci_RfIntf_t RfInterface;
pthread_mutex_t waitForResponse;
struct timespec timeoutTime;

void waitForReader();
void stopNfc();
void startNfc();

void gotData(){
    pthread_mutex_unlock(&waitForResponse);
}

void sendReceiveData(){
    unsigned char OK[] = {0x90, 0x00};
    unsigned char NOK[] = {0x6A, 0x82};
    unsigned char CmdSize;

    while(NxpNci_CardModeReceive(ceState.buffer, &CmdSize) == NFC_SUCCESS){
        PRINTF("APDU: ");
        for(int i = 0; i < CmdSize; i++){
            PRINTF("%02x ", ceState.buffer[i]);
        }
        PRINTF("\r\n");

        if(CmdSize < 4){
            NxpNci_CardModeSend(NOK, sizeof(NOK));
            continue;
        }

            //ceState.bufferLen = (unsigned int*)CmdSize;
            memcpy(ceState.bufferLen, &CmdSize, sizeof (CmdSize));
            clock_gettime(CLOCK_REALTIME, &timeoutTime);
            timeoutTime.tv_sec += 5;
            (*ceState.sendData)();
            int retVal = pthread_mutex_timedlock(&waitForResponse, &timeoutTime);
            if(retVal == ETIMEDOUT){
                PRINTF("End of transaction (timeout from server!)\n");
                (*ceState.onReaderLeave)();
                //timeoutCb(NULL);
                tml_hid_Cancel();
                return;
            }else{
                CmdSize = *(unsigned char*)ceState.bufferLen;
                NxpNci_CardModeSend(ceState.buffer, CmdSize);
            }
        }

    PRINTF("End of transaction\n");
    (*ceState.onReaderLeave)();
}

void waitForReader(){
    while(ceState.stopNfc == false){
        while(NxpNci_DiscoveryCallback(&RfInterface) != NFC_SUCCESS);
        (*ceState.onReaderArrive)();
        pthread_mutex_trylock(&waitForResponse);
        sendReceiveData();
    }
}

void startNfc(){
    unsigned char DiscoveryTechnologies[] = { MODE_LISTEN | TECH_PASSIVE_NFCB};

    /* Mode configuration according to the targeted modes of operation */
    unsigned mode = 0 | NXPNCI_MODE_CARDEMU;

    /* Open connection to NXPNCI device */
    if (NxpNci_Connect() == NFC_ERROR) {
        PRINTF("Error: cannot connect to NXPNCI device\n");
        return;
    }

    if (NxpNci_ConfigureSettings() == NFC_ERROR) {
        PRINTF("Error: cannot configure NXPNCI settings\n");
        return;
    }

    if (NxpNci_ConfigureMode(mode) == NFC_ERROR)
    {
        PRINTF("Error: cannot configure NXPNCI\n");
        return;
    }

    /* Start Discovery */
    if (NxpNci_StartDiscovery(DiscoveryTechnologies,sizeof(DiscoveryTechnologies)) != NFC_SUCCESS)
    {
        PRINTF("Error: cannot start discovery\n");
        return;
    }
    waitForReader();
}

void stopNfc(){
    ceState.stopNfc = true;
    //timeoutCb(NULL);
    tml_hid_Cancel();
    NxpNci_StopDiscovery();
    NxpNci_Disconnect();
}


void* startUsbEmulation(void* args){
    struct cardEmulationState* inputStruct = args;
    ceState.onReaderArrive = inputStruct->onReaderArrive;
    ceState.onReaderLeave = inputStruct->onReaderLeave;
    ceState.sendData = inputStruct->sendData;
    ceState.buffer = inputStruct->buffer;
    ceState.bufferLen = inputStruct->bufferLen;
    ceState.stopNfc = false;
    startNfc();
    return 0;
}
