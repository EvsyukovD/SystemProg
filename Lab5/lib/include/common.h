#ifndef COMMON_H
#define COMMON_H
#include "list.h"
#include <windows.h>
void PrintLastError();
HANDLE UtilGetFileHandle(const char *path);
/**
 * Thread secure print. Arguments the same as for printf
*/
void SecurePrint(const char* format,...);
#endif