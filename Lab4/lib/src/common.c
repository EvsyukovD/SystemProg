#include "../include/common.h"
#include <stdio.h>
#include <process.h>
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
int __stdcall PrintPhraseFromThread(const char *str){
     int N = 5;
     for (int i = 0; i < N; i++)
     {
       printf("String from thread %ld:%s\n",GetCurrentThreadId(),str);
     }
     _endthreadex(0);
     return 0;
}
int __stdcall SleepThread(const DWORD* millis){
    printf("Thread with threadID = %ld start sleep for %ld milliseconds\n", GetCurrentThreadId(), *millis);
    Sleep(*millis);
    printf("Thread with threadID = %ld has slept %ld milliseconds\n", GetCurrentThreadId(), *millis);
    _endthreadex(0);
    return 0;
}
