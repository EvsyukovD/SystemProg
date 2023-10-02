#include "../include/common.h"
//#include <windows.h>
#include <stdio.h>
void PrintLastError()
{
    int error = GetLastError();
    if (error)
    {
        printf("UtilCodeError: %d\n", error);
    }
}
void PrintProcessInfo(const PROCESS_INFORMATION* pi, const STARTUPINFO* si){
    printf("ProcessID = %ld\n", pi->dwProcessId);
    printf("ThreadID = %ld\n", pi->dwThreadId);
    //wprintf(L"Title: %s\n",si->lpDesktop);
}