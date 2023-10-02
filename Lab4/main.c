#include "lib/include/util.h"
#include <tchar.h>
#include <psapi.h>
int _tmain(int argc, TCHAR* argv[]){
   DWORD aProcesses[1024], cbNeeded, cProcesses;
   if(!UtilGetCurrentProcesses(aProcesses, &cProcesses)){
    return 1;
   }
    // Print the name and process identifier for each process.
    for (DWORD i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            PrintUtilProcessInfo(aProcesses[i]);
        }
    }

    return 0;
}