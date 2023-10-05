#include "../include/util.h"
#include "../include/common.h"
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
        }
    }
    // Print the process name and identifier.
    printf("%s  (PID: %u)\n", szProcessName, processID );
    // Release the handle to the process.
    CloseHandle( hProcess );
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
        /*PLIST_ENTRY pstart = NULL;
        void* readAddr = NULL;
        PLIST_ENTRY pmodule = NULL;
        PEB_LDR_DATA2 peb_ldr_data;
        LDR_MODULE peb_ldr_module;
        if(!ReadProcessMemory(hProcess,ldr,&peb_ldr_data,sizeof(peb_ldr_data),NULL)){
            PrintLastError();
            return;  
        }
        pmodule = peb_ldr_data.InMemoryOrderModuleList.Flink;
        pstart = pmodule;
        readAddr = (void*)pmodule;
        PLIST_ENTRY prev = NULL;
        TCHAR name[1024] = {0};
        while(ReadProcessMemory(hProcess,readAddr,&peb_ldr_module,sizeof(peb_ldr_module),NULL)){
              ReadProcessMemory(hProcess,peb_ldr_module.BaseDllName.Buffer,name,1024 * sizeof(TCHAR),NULL);
              printf("[+] %s\n", name);
              readAddr = (void *) peb_ldr_module.InLoadOrderModuleList.Flink;
              if(pstart == readAddr){
                 break;
              }
        }
        return;*/

        while (entry != &ldr->InMemoryOrderModuleList)
        {
             printf("[+] %S\n", data->FullDllName.Buffer);
             entry = entry->Flink;
             data = (PLDR_DATA_TABLE_ENTRY)entry;//entry->Blink;
        }
}
void UtilPrintPEBInformation(PROCESS_BASIC_INFORMATION* info){
    HINSTANCE hinstLib = NULL; 
    pfnNtQueryInformationProcess NtQueryInformationProcess = NULL; 
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE; 
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
            fRunTimeLinkSuccess = TRUE;
            HANDLE hProcess = GetCurrentProcess();/*OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   //PROCESS_ALL_ACCESS,
                                   FALSE, Get);*/
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
        fFreeResult = FreeLibrary(hinstLib); 
    } else {
        PrintLastError();
    }

    // If unable to call the DLL function, use an alternative.
    if (!fRunTimeLinkSuccess) 
        printf("Message printed from executable\n"); 
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
    
    // Calculate how many process identifiers were returned.
    // Print the name and process identifier for each process.
    for (DWORD i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
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
void print_options()
{
   char *options[] = {"0.Quit", 
                      "1.Create process [path_to_exe]", 
                      "2.Print processes", 
                      "3.Create random threads", 
                      "4.Print PEB info about process"
                       };
   int options_num = sizeof(options) / sizeof(options[0]);
   for (int i = 0; i < options_num; i++)
   {
      printf("%s\n", options[i]);
   }
}