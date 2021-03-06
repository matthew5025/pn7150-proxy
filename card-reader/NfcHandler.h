//
// Created by natsa on 23/06/2021.
//

#ifndef CARD_READER_NFCHANDLER_H
#define CARD_READER_NFCHANDLER_H

#endif //CARD_READER_NFCHANDLER_H
unsigned char sharedBuffer[512];
unsigned int sharedBufferLen;
void enableReader();
unsigned int sendTagCommand(unsigned char* result, unsigned int maxLen, unsigned int timeout);
void setOnCardDepartCallback(void (*ptr)());
void setOnCardArrivalCallback(void (*ptr)());
void onReaderArrival();
void disableReader();