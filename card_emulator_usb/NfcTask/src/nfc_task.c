/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <tool.h>
#include <Nfc.h>
#include <ndef_helper.h>

#define PRINT_BUF(x,y,z)  {unsigned int loop; PRINTF(x); for(loop=0;loop<z;loop++) PRINTF("%.2x ", y[loop]); PRINTF("\n");}
/* Discovery loop configuration according to the targeted modes of operation */
unsigned char DiscoveryTechnologies[] = { MODE_LISTEN | TECH_PASSIVE_NFCB};

/* Mode configuration according to the targeted modes of operation */
unsigned mode = 0 | NXPNCI_MODE_CARDEMU;


void PICC_ISO14443_4_scenario (void)
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

            if(CmdSize < 4){
                NxpNci_CardModeSend(NOK, sizeof(NOK));
                continue;
            }

            printf("\n");

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

                NxpNci_CardModeSend(OK, sizeof(OK));
            }
        }
        else
        {
            PRINTF("End of transaction\n");
            return;
        }
    }
}

void task_nfc(void)
{
    NxpNci_RfIntf_t RfInterface;

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

    while(1)
    {
        PRINTF("\nWAITING FOR DEVICE DISCOVERY\n");

		/* Wait until a peer is discovered */
        while(NxpNci_WaitForDiscoveryNotification(&RfInterface) != NFC_SUCCESS);

        /* Is activated from remote T4T ? */
        if ((RfInterface.Interface == INTF_ISODEP) && ((RfInterface.ModeTech & MODE_MASK) == MODE_LISTEN))
        {
            PRINTF(" - LISTEN MODE: Activated from remote Reader\n");
            NxpNci_ProcessCardMode(RfInterface);
            //PICC_ISO14443_4_scenario();
            PRINTF("READER DISCONNECTED\n");
        }
    }
}
