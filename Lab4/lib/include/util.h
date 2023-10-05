#ifndef UTIL_H
#define UTIL_H
#include <tchar.h>
#include <windows.h>
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
void UtilCreateProcess(TCHAR *path);
WINBOOL UtilGetCurrentProcesses(DWORD *aProcesses, DWORD *num_of_procs, int size);
void PrintUtilProcessInfo(DWORD processID);
void UtilCreateRandomThreads();
BOOL sm_EnableTokenPrivilege(LPCTSTR pszPrivilege,HANDLE hProcess);
void UtilPrintPEBInformation(PROCESS_BASIC_INFORMATION* info);
void PrintLDRInformation(const PPEB_LDR_DATA Ldr, HANDLE hProcess);

void create_process_args(int argc, TCHAR *argv[]);
void print_processes_args(int argc, TCHAR *argv[]);
void create_random_args(int argc, TCHAR *argv[]);
void print_peb_info_args(int argc, TCHAR *argv[]);
void print_options();
#endif