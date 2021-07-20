//
// Created by natsa on 20/07/2021.
//


#ifndef CARD_READER_LOGGINGHANDLER_H
#define CARD_READER_LOGGINGHANDLER_H
#include "CommsHandler.h"

enum LogMsgType{SEND_MSG, RECV_MSG, CTRL_MSG};

void initBuffer();
void addData(enum LogMsgType type, struct MessagePacket* inPacket);

#endif //CARD_READER_LOGGINGHANDLER_H


