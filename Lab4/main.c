#include "lib/include/util.h"
#include "lib/include/common.h"
#include <tchar.h>
#include <psapi.h>
#include <locale.h>
#include <stdio.h>
int _tmain(int argc, TCHAR* argv[]){
    setlocale(LC_ALL,"");
    void (*fptrs_args[])(int, TCHAR*) = {NULL, create_process_args, print_processes_args, create_random_args,
                         print_peb_info_args, print_extended_process_info_args};
    int res = 0;
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
    }
    return 0;
}