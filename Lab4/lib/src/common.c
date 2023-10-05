#include "../include/common.h"

#include <stdio.h>
#include <process.h>
#include <psapi.h>
void PrintLastError()
{
    int error = GetLastError();
    if (error)
    {
        printf("UtilCodeError: %d\n", error);
    }
}
void PrintProcessInfo(const PROCESS_INFORMATION* pi){
    printf("ProcessID = %ld\n", pi->dwProcessId);
    printf("ThreadID = %ld\n", pi->dwThreadId);
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
BOOL sm_EnableTokenPrivilege(LPCTSTR pszPrivilege,HANDLE hProcess)
{
    HANDLE hToken        = 0;
    TOKEN_PRIVILEGES tkp = {0}; 

    // Get a token for this process.

    if (!OpenProcessToken(hProcess,
                          TOKEN_ADJUST_PRIVILEGES |
                          TOKEN_QUERY, &hToken))
    {
        return FALSE;
    }

    // Get the LUID for the privilege. 

    if(LookupPrivilegeValue(NULL, pszPrivilege,
                            &tkp.Privileges[0].Luid)) 
    {
        tkp.PrivilegeCount = 1;  // one privilege to set    

        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        // Set the privilege for this process. 

        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                              (PTOKEN_PRIVILEGES)NULL, 0); 

        if (GetLastError() != ERROR_SUCCESS)
           return FALSE;
        
        return TRUE;
    }

    return FALSE;
}
void PrintLDRInformation(const PPEB_LDR_DATA ldr, HANDLE hProcess){
        PLDR_DATA_TABLE_ENTRY data = NULL;
        PLIST_ENTRY entry = ldr->InMemoryOrderModuleList.Flink;
        
        data = (PLDR_DATA_TABLE_ENTRY) entry;//(ldr->InMemoryOrderModuleList.Blink);
        while (entry != &ldr->InMemoryOrderModuleList)
        {
             printf("[+] %S\n", data->FullDllName.Buffer);
             entry = entry->Flink;
             data = (PLDR_DATA_TABLE_ENTRY)entry;//entry->Blink;
        }
}
void PrintUtilProcessInfo(DWORD processID){
    TCHAR szProcessName[1024] = TEXT("<unknown>");
    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );
    // Get the process name.
    if (hProcess)
    {
        HMODULE hMod;
        DWORD cModules;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cModules) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName, 
                               sizeof(szProcessName)/sizeof(TCHAR) );
        } else {
            PrintLastError();
            CloseHandle( hProcess );
            return;
        }
    } else{
        PrintLastError();
    }
    // Print the process name and identifier.
    printf("%s  (PID: %u)\n", szProcessName, processID );
    // Release the handle to the process.
    CloseHandle( hProcess );
}
void PrintExtendedProcessInfo(DWORD processID){
    TCHAR szProcessName[1024] = TEXT("<unknown>");
    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );
    // Get the process name.
    if (hProcess)
    {
        HMODULE hMods[1024];
        DWORD cModules;
        if ( EnumProcessModules( hProcess, &hMods, sizeof(hMods), 
             &cModules) )
        {
            for (int i = 0; i < (cModules / sizeof(HMODULE)); i++)
            {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.
            if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
                                      sizeof(szModName) / sizeof(TCHAR)))
            {
                // Print the module name and handle value.
                _tprintf( TEXT("Module: %s\n"), szModName);
            }
            }
        } {
            PrintLastError();
            CloseHandle( hProcess );
            return;
        }
    } else {
        PrintLastError();
    }
    // Print the process name and identifier.
    printf("%s  (PID: %u)\n", szProcessName, processID );
    // Release the handle to the process.
    CloseHandle( hProcess );
}