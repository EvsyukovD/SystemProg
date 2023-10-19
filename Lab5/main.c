#include "lib/include/util.h"
#include <time.h>
int main(int argc, char* argv[]){
    
    void (*fptrs_args[])(int, char* []) = {NULL, ProducerAndConsumer, ReadAndWrite, ThreadsRace};
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