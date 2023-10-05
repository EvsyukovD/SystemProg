#include "lib/include/util.h"
#include "lib/include/common.h"
#include <tchar.h>
#include <psapi.h>
#include <locale.h>
#include <stdio.h>
int _tmain(int argc, TCHAR* argv[]){
   /*DWORD aProcesses[1024], cbNeeded, cProcesses;
    if ( !UtilGetCurrentProcesses(aProcesses,&cProcesses, 1024))
    {
        PrintLastError();
        return 0;
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
    printf("cProc = %d\n",cProcesses);
    UtilCreateRandomThreads();*/
    setlocale(LC_ALL,"");
    void (*fptrs_args[])(int, TCHAR*) = {NULL, create_process_args, print_processes_args, create_random_args,
                         print_peb_info_args};
    int res = 0, n = 0;
    int size = sizeof(fptrs_args) / sizeof(fptrs_args[1]);
    if(argc > 1){

       if(!strcmp("-h",argv[1]) || !strcmp("--help",argv[1])){
         print_options();
         return 0;
       }
       
       res = atoi(argv[1]);
       if(res > 0 && res < size){
          fptrs_args[res](argc, argv);
          return 0;
       } else {
        printf("Quit\n");
        return 0;
       }
    } else {
        const TCHAR* args[] = {"","","8620"};
        print_peb_info_args(3, args);
    }
   /* print_options();
    puts("Enter option:");
    n = scanf("%d", &res);
    while (n >= 0)
    {
        while (n > 0 && res > 0 && res < size)
        {
            fptrs_dialog[res]();
            print_options();
            puts("Enter option:");
            n = scanf("%d", &res);
        }
        if (!res)
        {
            puts("Quit");
            return 0;
        }*/
    return 0;
}