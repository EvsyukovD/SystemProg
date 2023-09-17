#include "lib/util.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[]){
    const char *name = "C:\\Users\\devsy\\Desktop\\SysProg\\Lab3\\test\\file.txt";
    const char *new_name = "C:\\Users\\devsy\\Desktop\\SysProg\\Lab3\\test\\file3.txt";
    wchar_t* wname = Char2Wchar(name);
    wchar_t* wname2 = Char2Wchar(new_name);
    wchar_t* data = Char2Wchar("blaa-bla");
    UtilCreateFile(wname, data);
    printf("Size = %d\n",UtilGetSizeOfFile(wname));
    UtilGetFileAttributes(wname);
    // UtilRenameFile(wname, wname2);
    UtilCopyFile(wname,wname2);
    UtilDeleteFile(wname);
    free(wname);
    free(wname2);
    free(data);
    return 0;
}