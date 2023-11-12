#include "clientmsgservice.h"
#include <stdlib.h>
#include <stdio.h>
char* answer(const char* msg){
     const int ANSWER_SIZE = 20;
     char *ans = calloc(ANSWER_SIZE,sizeof(char));
     if(!ans){
        return NULL;
     }
     fflush(stdin);
     printf("Write answer for message: %s",msg);
     fgets(ans,ANSWER_SIZE, stdin);
     return ans;
}
void ExtractFileName(char* destBuffer, const char* buffer){
   char delim = '\\';
   char* ptr = strrchr(buffer, delim);
   ptr = ptr + 1;
   strcpy(destBuffer, ptr);
}
WINBOOL Dialog(char* buffer, int bufferSize){
     fflush(stdin);
     int c = 0, n = 0;
     char ans[2];
     static char fileName[1024] = {0};
     static int destId = -1;
     static WINBOOL filePartSendContinue = FALSE;
     DWORD writtenBytes = 0;
     static long skipBytes = 0;
     WINBOOL isEof = FALSE;
     if (!filePartSendContinue)// do we need to send next file partition?
     {
        puts("Do you want to send msg or file? Press Y or N respectively");
        scanf("%s", ans);
        if (ans[0] == 'Y')
        {
           n = scanf("%s", buffer);
           if (n <= 0)
           {
              puts("Invalid msg format");
              return FALSE;
           }
           return TRUE;
        }
        if (ans[0] == 'N')
        {
           puts("Enter file name:");
           scanf("%s", fileName);
           puts("Enter dest id:");
           scanf("%d", &destId);
           if (ReadFilePartForServer(buffer, bufferSize, destId, fileName, &writtenBytes, skipBytes, &isEof))
           {
              if (isEof)
              {
                 //end of reading file
                 filePartSendContinue = FALSE;
                 skipBytes = 0;
                 memset(fileName, 0, sizeof(fileName));
                 destId = -1;
                 return TRUE;
              }
              else
              {
                 skipBytes += writtenBytes;
                 filePartSendContinue = TRUE;
                 return TRUE;
              }
           }
           else
           {
              puts("Error during reading file");
              filePartSendContinue = FALSE;
              skipBytes = 0;
              return FALSE;
           }
        }
        return FALSE;
     }
     else
     {
        if (ReadFilePartForServer(buffer, bufferSize, destId, fileName, &writtenBytes, skipBytes, &isEof))
        {
           if (isEof)
           {
              filePartSendContinue = FALSE;
              skipBytes = 0;
              memset(fileName, 0, sizeof(fileName));
              destId = -1;
              return TRUE;
           }
           else
           {
              skipBytes += writtenBytes;
              filePartSendContinue = TRUE;
              return TRUE;
           }
        }
        else
        {
           puts("Error during reading file");
           filePartSendContinue = FALSE;
           skipBytes = 0;
           return FALSE;
        }
     }
}
void ProcessFilePartFromServer(const char* buf){
    // buf: fileName:data
    HANDLE file = INVALID_HANDLE_VALUE;
    const char* sep = ":";
    char *ptr = strstr(buf, sep);
    if(!ptr){//invalid file partition
        return;
    }
    int size = ptr - buf; // before ':'
    char name[1024] = {0};
    strcpy(name, "C:\\Users\\devsy\\Desktop\\SysProg\\Lab6\\client\\instl\\");
    strncat(name, buf, size);
    file = CreateFileA(name,
                       FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                       FILE_SHARE_WRITE | FILE_SHARE_READ,
                       NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        file = CreateFileA(name,
                           FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                           FILE_SHARE_WRITE | FILE_SHARE_READ,
                           NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    }
    if (file == INVALID_HANDLE_VALUE)
    {
        printf("Util error: %d\n", GetLastError());
        return;
    }
    char* data = &ptr[1];
    long unsigned int written_bytes = 0;
    int len = strlen(data);
    WINBOOL res = WriteFile(file, data, len, &written_bytes, NULL);
    if (!res)
    {
        CloseHandle(file);
        printf("Util error: %d\n", GetLastError());
        return;
    }
    CloseHandle(file);
}
WINBOOL ReadFilePartForServer(char *destBuffer, 
                              int destBufferSize, 
                              int destId, 
                              const char *path, 
                              LPDWORD writtenBytes, 
                              DWORD skipBytes, WINBOOL *isEof)
{
    int radix = 10;
    const char* sep = ":";
    const char* file_id = "f";
    char fileName[30] = {0};
    HANDLE file = CreateFileA(path,
                              FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                              FILE_SHARE_WRITE | FILE_SHARE_READ,
                              NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        printf("Util error: %d\n", GetLastError());
        *writtenBytes = 0;
        return FALSE;
    }
    ExtractFileName(fileName, path);
    //destBuffer format: destId:file_id:fileName:data
    memset(destBuffer, 0, destBufferSize);
    itoa(destId, destBuffer, radix);
    strcat(destBuffer, sep);
    strcat(destBuffer, file_id);
    strcat(destBuffer, sep);
    strcat(destBuffer, fileName);
    strcat(destBuffer, sep);
    int headerSize = strlen(destBuffer);
    SetFilePointer(file, skipBytes, NULL, FILE_BEGIN);
    WINBOOL result = ReadFile(file, &destBuffer[headerSize], destBufferSize - headerSize,writtenBytes,NULL);
    if(!result){
        printf("Util error: %d\n", GetLastError());
        CloseHandle(file);
        return FALSE;
    }
    DWORD size;
    //SetFilePointerEx(file, 0, &pos, FILE_CURRENT);
    size = GetFileSize(file, NULL);
    *isEof = *writtenBytes + skipBytes >= size;
    CloseHandle(file);
    return TRUE;
}