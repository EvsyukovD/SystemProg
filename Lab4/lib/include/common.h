#ifndef COMMON_H
#define COMMON_H
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <winternl.h>
typedef struct _LDR_MODULE
    {
        LIST_ENTRY InLoadOrderModuleList;
        LIST_ENTRY InMemoryOrderModuleList;
        LIST_ENTRY InInitializationOrderModuleList;
        PVOID BaseAddress;
        PVOID EntryPoint;
        ULONG SizeOfImage;
        UNICODE_STRING FullDllName;
        UNICODE_STRING BaseDllName;
        ULONG Flags;
        USHORT LoadCount;
        USHORT TlsIndex;
        LIST_ENTRY HashTableEntry;
        ULONG TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;
typedef struct _PEB_LDR_DATA1 {
    BYTE Reserved1[8];
    PVOID Reserved2[3];
    LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA1, *PPEB_LDR_DATA1;
typedef struct _PEB_LDR_DATA2
    {
        ULONG Length;
        UCHAR Initialized;
        PVOID SsHandle;
        LIST_ENTRY InLoadOrderModuleList;
        LIST_ENTRY InMemoryOrderModuleList;
        LIST_ENTRY InInitializationOrderModuleList;
        PVOID EntryInProgress;

    } PEB_LDR_DATA2, *PPEB_LDR_DATA2;
void PrintLastError();
void PrintProcessInfo(const PROCESS_INFORMATION* pi);
void PrintLDRInformation(const PPEB_LDR_DATA Ldr, HANDLE hProcess);
void PrintExtendedProcessInfo(DWORD processID);
void PrintUtilProcessInfo(DWORD processID);
BOOL sm_EnableTokenPrivilege(LPCTSTR pszPrivilege,HANDLE hProcess);
int __stdcall PrintPhraseFromThread(const char*);
int __stdcall SleepThread();
#endif