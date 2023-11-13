#include <stdio.h>
#include "lib/include/server.h"
int main(int argc, char* argv[]){
    //StartServerContext("localhost","10102");
    void (*fptrs_args[])(int, char* []) = {NULL, StartServerContextArgs, StartServerPipeContextArgs};
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