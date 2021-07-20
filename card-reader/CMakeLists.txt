cmake_minimum_required(VERSION 3.16)
project(card_reader C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


add_executable(card_reader main.c CommsHandler.c CommsHandler.h NfcHandler.c NfcHandler.h FileLogger.c FileLogger.h
        LoggingHandler.c LoggingHandler.h CompanionController.c CompanionController.h)
target_link_libraries(card_reader libnfc_nci_linux.so -lpthread)