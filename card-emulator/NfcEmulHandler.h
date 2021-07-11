//
// Created by natsa on 27/06/2021.
//

#ifndef CARD_EMULATOR_NFCEMULHANDLER_H
#define CARD_EMULATOR_NFCEMULHANDLER_H

#endif //CARD_EMULATOR_NFCEMULHANDLER_H
unsigned char sharedBuffer[512];
unsigned int sharedBufferLen;

void startEmulation();
void setOnDataCallback(void (*ptr)());
void endEmulation();
void setOnReaderGoneCallback(void (*ptr)());