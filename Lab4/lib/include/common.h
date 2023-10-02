#ifndef COMMON_H
#define COMMON_H
#include <stdlib.h>
#include <windows.h>
void PrintLastError();
void PrintProcessInfo(const PROCESS_INFORMATION* pi, const STARTUPINFO* si);
#endif