//
// Created by natsa on 12/07/2021.
//

#include <stdio.h>
#include "CompanionController.h"
#include "CommsHandler.h"

int threadStart(void* thr_data){
    printf("%s", (com *)thr_data);
    return 0;
}