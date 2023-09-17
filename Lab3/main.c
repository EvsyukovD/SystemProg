#include "lib/util.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
    void (*fptrs[])() = {NULL, create_file, read_file, delete_file,
                         rename_file, copy_file, get_size, get_attributes, set_attributes, print_folder_content};
    int res = 0, n = 0;
    int size = sizeof(fptrs) / sizeof(fptrs[1]); 
    puts("Enter option:");
    n = scanf("%d",&res);
    while(n > 0 && res > 0 && res < size){
        fptrs[res]();
        puts("Enter option:");
        n = scanf("%d",&res);
    }
    if(!res){
        puts("Quit");
    }
    //const char *name = "C:\\Users\\devsy\\Desktop\\SysProg\\Lab3\\test\\file.txt";
   // const char *new_name = "C:\\Users\\devsy\\Desktop\\SysProg\\Lab3\\test\\file3.txt";
   // const char *folder = "C:\\Users\\devsy\\Desktop\\SysProg\\Lab3";
    return 0;
}