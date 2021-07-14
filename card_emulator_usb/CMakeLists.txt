cmake_minimum_required(VERSION 3.16)
project(card_emulator_usb C)

set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE NfcLibSrc NfcLibrary/*.c)
file(GLOB_RECURSE TMLSrc TML/*.c)
file(GLOB_RECURSE NfcTaskSrc NfcTask/*.c)

include_directories(TML/inc NfcLibrary/inc NfcLibrary/NxpNci/inc NfcLibrary/NdefLibrary/inc NfcTask/inc)
add_definitions(-DCARDEMU_SUPPORT -DCARDEMU_RAW_EXCHANGE)
add_executable(card_emulator_usb ${TMLSrc} ${NfcLibSrc} ${NfcTaskSrc} Application/main.c)

target_link_libraries(card_emulator_usb rt pthread udev)
