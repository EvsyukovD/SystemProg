#include "../include/common.h"
#include <stdio.h>
#include <windows.h>
void PrintLastError(){
    printf("Util error code: %d", GetLastError());
}
HANDLE UtilGetFileHandle(const char *path)
{
   HANDLE hFile = CreateFile(path, FILE_GENERIC_READ | FILE_GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile == INVALID_HANDLE_VALUE)
   {
      return NULL;
   }
   /*
   DWORD dwFileSize = GetFileSize(hFile, 0);
   if (dwFileSize == INVALID_FILE_SIZE)
   {
      CloseHandle(hFile);
      return NULL;
   }
   HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
   if (!hMapping)
   {
      CloseHandle(hFile);
      return NULL;
   }
   char *dataPtr = (char *)MapViewOfFile(hMapping,
                                         FILE_MAP_ALL_ACCESS,
                                         0,
                                         0,
                                         dwFileSize);
   if (!dataPtr)
   {
      CloseHandle(hMapping);
      CloseHandle(hFile);
      return NULL;
   }*/
   return hFile;
}