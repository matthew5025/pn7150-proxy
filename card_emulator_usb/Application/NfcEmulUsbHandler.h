//
// Created by natsa on 15/07/2021.
//


#ifndef CARD_EMULATOR_USB_NFCEMULUSBHANDLER_H
#define CARD_EMULATOR_USB_NFCEMULUSBHANDLER_H

#endif //CARD_EMULATOR_USB_NFCEMULUSBHANDLER_H
#include "stdbool.h"

struct cardEmulationState {
    void (*onReaderArrive)(void);
    void (*sendData)(void );
    void (*onReaderLeave)(void);
    unsigned char* buffer;
    unsigned int* bufferLen;
    bool stopNfc;
};

void* startUsbEmulation(void* args);
void gotData();