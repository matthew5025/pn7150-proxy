cmake_minimum_required(VERSION 3.16)
project(card_emulator C)

set(CMAKE_C_STANDARD 99)

add_executable(card_emulator main.c EmulationCommsHandler.c EmulationCommsHandler.h NfcEmulHandler.c NfcEmulHandler.h)
target_link_libraries(card_emulator libnfc_nci_linux.so -lpthread)