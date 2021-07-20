//
// Created by natsa on 20/06/2021.
//

#include <stdint.h>

#ifndef CARD_READER_COMMSHANDLER_H
#define CARD_READER_COMMSHANDLER_H

struct MessagePacket {
    unsigned char type;
    uint16_t length;
    unsigned char message [512];
};

enum MessageType{
    ECHO = 0x01,
    ECHO_REPLY = 0x02,
    READER_ARRIVAL = 0x03,
    TAG_INFO_REPLY = 0x04,
    TAG_CMD = 0x05,
    TAG_CMD_REPLY = 0x06,
    CARD_GONE = 0x07,
    READER_GONE = 0x08
};


void setSocket(int in_sock);
int readSocket();

#endif //CARD_READER_COMMSHANDLER_H


