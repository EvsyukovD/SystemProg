#include "../include/common.h"
#include <stdio.h>
#include <windows.h>
void PrintLastError(){
    printf("Util error code: %d", GetLastError());
}