//
// Created by natsa on 15/07/2021.
//

#include <semaphore.h>

#ifndef CARD_EMULATOR_USB_NFCEMULUSBHANDLER_H
#define CARD_EMULATOR_USB_NFCEMULUSBHANDLER_H

#endif //CARD_EMULATOR_USB_NFCEMULUSBHANDLER_H

typedef struct {
    void (*onEmulationActivated) ();
    void (*onEmulationDeactivated) ();
    void (*onDataReceived)();
    unsigned char* sharedBuffer;
    unsigned int* sharedBufferLen;
    unsigned char cardTech;
    sem_t mutex;
}cardEmulationCallbacks;

int usbEmulThread(void* threadData);