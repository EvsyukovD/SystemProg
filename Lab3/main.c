#include "lib/include/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    void (*fptrs_dialog[])() = {NULL, create_file_dialog, read_file_dialog, delete_file_dialog,
                         rename_file_dialog, copy_file_dialog, get_size_dialog, get_attributes_dialog, 
                         set_attributes_dialog, print_folder_content_dialog,arrange_symbols_dialog,count_symbols_dialog,
                         delete_symbols_dialog,sort_nums_dialog};
    void (*fptrs_args[])(int argc, char *argv[]) = {NULL, create_file_args, read_file_args, delete_file_args,
                         rename_file_args, copy_file_args, get_size_args, get_attributes_args, 
                         set_attributes_args, print_folder_content_args,arrange_symbols_args,count_symbols_args,
                         delete_symbols_args,sort_nums_args};
    int res = 0, n = 0;
    int size = sizeof(fptrs_dialog) / sizeof(fptrs_dialog[1]);
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
        return 0;
       }

    }
    print_options();
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
        }
    }
    return 0;
}