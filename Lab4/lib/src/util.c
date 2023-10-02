#include "../include/util.h"
#include "../include/common.h"
//#include <windows.h>
#include <stdio.h>
#include <psapi.h>
//#define PSAPI_VERSION 1
//#pragma comment(util_lib, "psapi")
int UtilCreateProcess(int argc, TCHAR *argv[]){
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    int proc_ret_code = 0, thread_ret_code = 0;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    if( argc != 2 )
    {
        printf("Usage: %s [cmdline]\n", argv[0]);
        return;
    }
    printf("Executable name: %s\n",argv[1]);
    // Start the child process. 
    if( !CreateProcess(argv[1],   // No module name (use command line)
        NULL,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        PrintLastError();
        return;
    }
    PrintProcessInfo(&pi,&si);
    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );
    GetExitCodeProcess(pi.hProcess, &proc_ret_code);
    GetExitCodeThread(pi.hThread, &thread_ret_code);
    printf("Process return code: %d\n",proc_ret_code);
    printf("Thread return code: %d\n",thread_ret_code);
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}
WINBOOL UtilGetCurrentProcesses(DWORD* aProcesses, DWORD *num_of_procs){
    /*const int N = 1024;
    DWORD aProcesses = (DWORD*)calloc(N, sizeof(DWORD));
    if(!aProcesses){
        printf("Can't allocate memory\n");
        PrintLastError();
        return NULL;
    }*/
    DWORD cbNeeded, cProcesses;
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        PrintLastError();
        return 0;
    }
    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);
    *num_of_procs = cProcesses;
    //*num_of_processes = cProcesses;
    return 1;
}
void UtilReleaseProcessInfo(UtilProcessInfo* info){
    if(info){
        free(info->processName);
        free(info);
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
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName, 
                               sizeof(szProcessName)/sizeof(TCHAR) );
        }
    }
    // Print the process name and identifier.
    printf("%s  (PID: %u)\n", szProcessName, processID );
    // Release the handle to the process.
    CloseHandle( hProcess );
}