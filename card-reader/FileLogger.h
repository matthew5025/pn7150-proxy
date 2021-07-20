//
// Created by natsa on 11/07/2021.
//

#ifndef CARD_READER_FILELOGGER_H
#define CARD_READER_FILELOGGER_H
#include "CommsHandler.h"
#include "LoggingHandler.h"

void fileLoggerProcessMsg(enum LogMsgType type, struct MessagePacket* inPacket);
#endif //CARD_READER_FILELOGGER_H
