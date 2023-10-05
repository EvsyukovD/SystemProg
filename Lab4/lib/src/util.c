#include "../include/util.h"
#include <stdio.h>
#include <psapi.h>
typedef NTSTATUS (NTAPI *pfnNtQueryInformationProcess)(
    IN  HANDLE ProcessHandle,
    IN  PROCESSINFOCLASS ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN  ULONG ProcessInformationLength,
    OUT PULONG ReturnLength    OPTIONAL
    );
void UtilCreateProcess(TCHAR *path){
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    int proc_ret_code = 0, thread_ret_code = 0;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    printf("Executable name: %s\n",path);
    // Start the child process. 
    if( !CreateProcess(path,   // No module name (use command line)
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
    PrintProcessInfo(&pi);
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
WINBOOL UtilGetCurrentProcesses(DWORD* aProcesses, DWORD *num_of_procs, int size){
    DWORD cbNeeded, cProcesses;
    if ( !EnumProcesses( aProcesses, size * sizeof(DWORD), &cbNeeded ) )
    {
        PrintLastError();
        return 0;
    }
    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);
    *num_of_procs = cProcesses;
    return 1;
}

void UtilCreateRandomThreads(){
        const int N = 5; 
        DWORD millis = 500;
        HANDLE threads[N];
        for (int i = 0; i < N; i++)
        {
            if(i % 2 == 0){
               threads[i] = CreateThread(NULL, 0, PrintPhraseFromThread, "meow",0, 0);
            } else {
                threads[i] = CreateThread(NULL, 0, SleepThread, &millis ,0, 0);
            } 
        }
        WaitForMultipleObjects(N, threads,TRUE, INFINITE);
        for (int i = 0; i < N; i++)
        {
             CloseHandle(threads[i]);
        }
}
void UtilPrintPEBInformation(PROCESS_BASIC_INFORMATION* info){
    HINSTANCE hinstLib = NULL; 
    pfnNtQueryInformationProcess NtQueryInformationProcess = NULL; 
    ULONG ulSize = 0;
    // Get a handle to the DLL module.
 
    hinstLib = LoadLibrary(TEXT("ntdll.dll")); 
 
    // If the handle is valid, try to get the function address.
 
    if (hinstLib) 
    { 
        NtQueryInformationProcess = (pfnNtQueryInformationProcess) GetProcAddress(hinstLib, "NtQueryInformationProcess"); 
 
        // If the function address is valid, call the function.
 
        if (NtQueryInformationProcess) 
        {
            HANDLE hProcess = GetCurrentProcess();
            if(hProcess){
               sm_EnableTokenPrivilege(SE_DEBUG_NAME, hProcess);
               NTSTATUS st = NtQueryInformationProcess(hProcess,ProcessBasicInformation,info,sizeof(PROCESS_BASIC_INFORMATION),&ulSize);
               PPEB ppeb = info->PebBaseAddress;
               PEB peb;
               ReadProcessMemory(hProcess,ppeb,&peb,sizeof(PEB),NULL);
               PrintLDRInformation(peb.Ldr, hProcess);
               CloseHandle(hProcess);
            } else {
                PrintLastError();
            }
        } else{
            PrintLastError();
        }
        // Free the DLL module.
        FreeLibrary(hinstLib); 
    } else {
        PrintLastError();
    }
}
void create_process_args(int argc, TCHAR *argv[]){
    if(argc > 2){
        UtilCreateProcess(argv[2]);
    } else {
        printf("Write path to executable file\n");
    }
}
void print_processes_args(int argc, TCHAR *argv[]){
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    if ( !UtilGetCurrentProcesses(aProcesses,&cProcesses, 1024))
    {
        PrintLastError();
        return;
    }
    
    for (DWORD i = 0; i < cProcesses; i++ )
    {
        if(aProcesses[i])
        {
            PrintUtilProcessInfo(aProcesses[i]);
        }
    }
}
void create_random_args(int argc, TCHAR *argv[]){
    UtilCreateRandomThreads();
}
void print_peb_info_args(int argc, TCHAR *argv[]){
     printf("Current process pid = %ld\n", GetCurrentProcessId());
     PROCESS_BASIC_INFORMATION info;
     UtilPrintPEBInformation(&info);
}
void print_extended_process_info_args(int argc, TCHAR* argv[]){
    if(argc > 2) {
       DWORD pid = atoll(argv[2]);
       PrintExtendedProcessInfo(pid);
    }
}
void print_options()
{
   char *options[] = {"0.Quit", 
                      "1.Create process [path_to_exe]", 
                      "2.Print processes", 
                      "3.Create random threads", 
                      "4.Print PEB info about current process",
                      "5.Print extended info about process [pid]"
                       };
   int options_num = sizeof(options) / sizeof(options[0]);
   for (int i = 0; i < options_num; i++)
   {
      printf("%s\n", options[i]);
   }
}