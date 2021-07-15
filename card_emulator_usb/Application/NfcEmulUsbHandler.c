//
// Created by natsa on 15/07/2021.
//

#include <Nfc.h>
#include <tool.h>
#include "NfcEmulUsbHandler.h"

cardEmulationCallbacks* callback;
void emulDataTransfer (void)
{
    unsigned char OK[] = {0x90, 0x00};
    unsigned char NOK[] = {0x6A, 0x82};
    unsigned char Cmd[256];
    unsigned char CmdSize;

    while (1)
    {
        int state = NxpNci_CardModeReceive(Cmd, &CmdSize);
        PRINTF("State: %d\r\n", state);
        if(state == NFC_SUCCESS)
        {
            PRINTF("APDU: ");
            for(int i = 0; i < CmdSize; i++){
                PRINTF("%02x ", Cmd[i]);
            }

            printf("\n");

            if(CmdSize < 4){
                NxpNci_CardModeSend(NOK, sizeof(NOK));
                continue;
            }


            memcpy(callback->sharedBuffer, Cmd, CmdSize);
            unsigned int bufferSize = CmdSize;
            memcpy(callback->sharedBufferLen, &bufferSize, sizeof (bufferSize));
            (*callback->onDataReceived)();

            if ((CmdSize >= 2) && (Cmd[0] == 0x00))
            {
                switch (Cmd[1])
                {
                    case 0xA4:
                        PRINTF("Select File received\n");
                        break;

                    case 0xB0:
                        PRINTF("Read Binary received\n");
                        break;

                    case 0xD0:
                        PRINTF("Write Binary received\n");
                        break;

                    default:
                        break;
                }
            }
            sem_wait(&callback->mutex);
            unsigned char dataLen = (unsigned char) *(callback->sharedBufferLen);
            NxpNci_CardModeSend(callback->sharedBuffer, dataLen);

        }
        else
        {
            PRINTF("End of transaction\n");
            return;
        }
    }

}


int usbEmulThread(void* threadData){
    NxpNci_RfIntf_t RfInterface;
    callback = threadData;
    unsigned mode = 0 | NXPNCI_MODE_CARDEMU;

    unsigned char DiscoveryTechnologies[] = { MODE_LISTEN | TECH_PASSIVE_NFCB};

    /* Open connection to NXPNCI device */
    if (NxpNci_Connect() == NFC_ERROR) {
        PRINTF("Error: cannot connect to NXPNCI device\n");
    }

    if (NxpNci_ConfigureSettings() == NFC_ERROR) {
        PRINTF("Error: cannot configure NXPNCI settings\n");
    }

    if (NxpNci_ConfigureMode(mode) == NFC_ERROR)
    {
        PRINTF("Error: cannot configure NXPNCI\n");
    }

    /* Start Discovery */
    if (NxpNci_StartDiscovery(DiscoveryTechnologies,sizeof(DiscoveryTechnologies)) != NFC_SUCCESS)
    {
        PRINTF("Error: cannot start discovery\n");
    }


    while(1)
    {
        PRINTF("\nWAITING FOR DEVICE DISCOVERY\n");

        /* Wait until a peer is discovered */
        while(NxpNci_WaitForDiscoveryNotification(&RfInterface) != NFC_SUCCESS);

        /* Is activated from remote T4T ? */
        if ((RfInterface.Interface == INTF_ISODEP) && ((RfInterface.ModeTech & MODE_MASK) == MODE_LISTEN))
        {
            (*callback->onEmulationActivated)();
            PRINTF(" - LISTEN MODE: Activated from remote Reader\n");
            emulDataTransfer();
            PRINTF("READER DISCONNECTED\n");
            (*callback-> onEmulationDeactivated)();
        }
    }

}
