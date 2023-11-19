#include "clientmsgservice.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wincrypt.h>
#include <math.h>
#pragma comment (lib, "Crypt32.lib")
DWORD GetNumberFromBufferBeforeSep(const char* s, char sep){
   DWORD result = 0;
   for (int i = 0; s[i] && s[i] != sep; i++)
   {
        result = 10 * result + s[i] - '0';
   }
   return result;
}
void ExtractFileName(char* destBuffer, const char* buffer){
   char delim = '\\';
   char* ptr = strrchr(buffer, delim);
   ptr = ptr + 1;
   strcpy(destBuffer, ptr);
}
void fill_written_bytes_str(char* dest_str, int dest_str_len, char filler){
     for (int i = 0; i < dest_str_len; i++)
     {
        if(!isdigit(dest_str[i])){
           dest_str[i] = filler;
        }
     }
     
}
WINBOOL encode_to_base64(char* bytes, DWORD len, DWORD* enc_size){
     WINBOOL res = FALSE;
     DWORD needed_size = 0;
     res = CryptBinaryToStringA((const BYTE*)bytes, len, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &needed_size);
     if(!res){
        return FALSE;
     }
     char* tmp_s = (char*)calloc(needed_size, sizeof(char));
     if(!tmp_s){
        return FALSE;
     }
     res = CryptBinaryToStringA((const BYTE*)bytes, len, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, tmp_s, &needed_size);
     if(!res){
        printf("Error crypt: %d\n", GetLastError());
        free(tmp_s);
        return FALSE;
     }
     unsigned char b = 0;
     for (int i = 0; i < needed_size; i++)
     {
          bytes[i] = tmp_s[i];
          /*b = bytes[i];
          if(b <= 0xfd){
             b++;
             bytes[i] = b;
          }*/
     }
     *enc_size = needed_size;
     free(tmp_s);
     return TRUE;
}
WINBOOL decode_base64(char* bytes, DWORD len, DWORD* dec_len){
     WINBOOL res = FALSE;
     DWORD dwBinaryLen = 0, dwSkip = 0, dwFlags = 0;
     res = CryptStringToBinaryA(bytes, len, CRYPT_STRING_BASE64, NULL,&dwBinaryLen, &dwSkip, &dwFlags);
     if(!res){
       return FALSE;
     }
     char* tmp_s = (char*)calloc(dwBinaryLen, sizeof(char));
     if(!tmp_s){
       return FALSE;
     }
     res = CryptStringToBinaryA(bytes, len, CRYPT_STRING_BASE64, tmp_s, &dwBinaryLen, &dwSkip, &dwFlags);
     if(!res){
        free(tmp_s);
        return FALSE;
     }
     unsigned char b = 0;
     memset(bytes, 0, len);
     for (int i = 0; i < dwBinaryLen; i++)
     {
          bytes[i] = tmp_s[i];
          /*b = bytes[i];
          if(b <= 0xfe){
             b--;
             bytes[i] = b;
          }*/
     }
     *dec_len = dwBinaryLen;
     free(tmp_s);
     return TRUE;
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
        printf("Your answer: %c\n", ans[0]);
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
           puts("Enter file path:");
           scanf("%s", fileName);
           puts("Enter dest id:");
           scanf("%d", &destId);
           if (ReadFilePartForServer(buffer, bufferSize, destId, fileName, &writtenBytes, skipBytes, &isEof, TRUE))
           {
              if (isEof)
              {
                 //end of reading file
                 filePartSendContinue = FALSE;
                 skipBytes = 0;
                 memset(fileName, 0, sizeof(fileName) / sizeof(fileName[0]));
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
        printf("You should choose type of msg\n");
        return FALSE;
     }
     else
     {
        if (ReadFilePartForServer(buffer, bufferSize, destId, fileName, &writtenBytes, skipBytes, &isEof, FALSE))
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
DWORD GetPartSizeFromBuffer(const char*s){
   DWORD result = 0;
   for (int i = 0; s[i] && isdigit(s[i]); i++)
   {
        result = 10 * result + s[i] - '0';
   }
   return result;
}
void ProcessFilePartFromServer(const char* buf, int buf_len){
     // buf: fileName:size:data
     HANDLE file = INVALID_HANDLE_VALUE;
     const char *sep = ":";
     char ans[2] = {0}; 
     char *ptr = strstr(buf, sep);
     if (!ptr)
     { // invalid file partition
        return;
     }
     int size = ptr - buf; // before ':'
     char name[1024] = {0};
     strcpy(name, SOURCE_ROOT);
     strcat(name, "//instl//");
     strncat(name, buf, size);
     ptr = ptr + 1;//ptr={size:data}
     static DWORD originFileSize = 0;
     WINBOOL isFirstFileMsg = FALSE;
     DWORD part_size = 0;
     file = CreateFileA(name,
                        FILE_GENERIC_READ | FILE_APPEND_DATA,
                        FILE_SHARE_WRITE | FILE_SHARE_READ,
                        NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL, NULL);
     if(file == INVALID_HANDLE_VALUE){
        isFirstFileMsg = TRUE;
        originFileSize = GetNumberFromBufferBeforeSep(ptr, sep[0]); // read file size
        file = CreateFileA(name,
                    FILE_GENERIC_READ | FILE_APPEND_DATA,
                    FILE_SHARE_WRITE | FILE_SHARE_READ,
                    NULL, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, NULL);
     } else {
        isFirstFileMsg = FALSE;
        DWORD size = GetFileSize(file, NULL);
        if(size == originFileSize){
           printf("File %s already exists\n", name);
           return;
        }
        part_size = GetPartSizeFromBuffer(ptr); // read part size
     }
     if (file == INVALID_HANDLE_VALUE)
     {
        printf("Util error: %d\n", GetLastError());
        return;
     }
     if(isFirstFileMsg){
        return;
     }
     // ptr
     // filesize:data
     ptr = strstr(ptr, sep) + 1;//ptr={data}
     char *data = ptr;
     long unsigned int written_bytes = 0;
     int len = part_size;
     DWORD dec_len = 0;
     decode_base64(data, len, &dec_len);
     WINBOOL res = WriteFile(file, data, dec_len, &written_bytes, NULL);
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
                              DWORD skipBytes, WINBOOL *isEof, WINBOOL isFirstPart)
{
    int radix = 10;
    const char* sep = ":";
    const char* file_id = "f";
    char filler = '_';
    char fileName[30] = {0};
    char written_bytes_str[5] = {0};
    int gap_size_bytes = 4;
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
    DWORD fileSize = GetFileSize(file, NULL);
    //destBuffer format: destId:file_id:fileName:filesize:data
    memset(destBuffer, 0, destBufferSize);
    itoa(destId, destBuffer, radix);
    strcat(destBuffer, sep);
    strcat(destBuffer, file_id);
    strcat(destBuffer, sep);
    strcat(destBuffer, fileName);
    strcat(destBuffer, sep);
    int i = strlen(destBuffer);
    if(isFirstPart){
       _ultoa_s(fileSize, &destBuffer[i], destBufferSize - i, radix);
       strcat(destBuffer, sep);
       CloseHandle(file);
       return TRUE;
    }
    char* gap_ptr = &destBuffer[i];
    strcat(&gap_ptr[gap_size_bytes], sep);
    int headerSize = &gap_ptr[gap_size_bytes] - destBuffer + 1;
    SetFilePointer(file, skipBytes, NULL, FILE_BEGIN);
    double v = destBufferSize - headerSize - 1;
    DWORD read_len = floor(v / 2);
    WINBOOL result = ReadFile(file, &destBuffer[headerSize], read_len ,writtenBytes,NULL);
    if(!result){
        printf("Util error: %d\n", GetLastError());
        CloseHandle(file);
        return FALSE;
    }
    DWORD enc_len = 0;
    encode_to_base64(&destBuffer[headerSize], *writtenBytes, &enc_len);
    _ultoa_s(enc_len, written_bytes_str,sizeof(written_bytes_str)/ sizeof(written_bytes_str[0]), radix);
    fill_written_bytes_str(written_bytes_str, gap_size_bytes, filler);
    strncpy(gap_ptr, written_bytes_str, gap_size_bytes);
    memset(&destBuffer[headerSize + enc_len], filler, destBufferSize - headerSize - enc_len);
    //gap_ptr[gap_size_bytes - 1] = ' ';
    //DWORD size = GetFileSize(file, NULL);
    *isEof = *writtenBytes + skipBytes >= fileSize;
    CloseHandle(file);
    return TRUE;
}