#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
FileMapping *UtilGetFileMapping(const wchar_t *path)
{
   HANDLE hFile = CreateFileW(path, FILE_GENERIC_READ | FILE_GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile == INVALID_HANDLE_VALUE)
   {
      return NULL;
   }
   DWORD dwFileSize = GetFileSize(hFile, 0);
   if (dwFileSize == INVALID_FILE_SIZE)
   {
      CloseHandle(hFile);
      return NULL;
   }
   HANDLE hMapping = CreateFileMappingW(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
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
   }
   FileMapping *mapping = (FileMapping *)malloc(sizeof(FileMapping));
   if (!mapping)
   {
      UnmapViewOfFile(dataPtr);
      CloseHandle(hMapping);
      CloseHandle(hFile);
      return NULL;
   }
   mapping->hFile = hFile;
   mapping->hMapping = hMapping;
   mapping->dataPtr = dataPtr;
   mapping->fsize = (size_t)dwFileSize;
   return mapping;
}
void UtilReleaseFileMapping(FileMapping *mapping)
{
   if (mapping)
   {
      UnmapViewOfFile(mapping->dataPtr);
      CloseHandle(mapping->hMapping);
      CloseHandle(mapping->hFile);
      free(mapping);
   }
}
int alphabet_comparator(const void *a, const void *b)
{
   return (*((char *)a) - *((char *)b));
}
void UtilFileMappingArrangeBySymbols(const wchar_t *path)
{
   FileMapping *mapping = UtilGetFileMapping(path);
   if (!mapping)
   {
      PrintLastError();
      return;
   }
   qsort(mapping->dataPtr, mapping->fsize, sizeof(char), alphabet_comparator);
   UtilReleaseFileMapping(mapping);
}
void UtilFileMappingCalculateSymbols(const wchar_t *path, int* num_of_uppers, int* num_of_lows)
{
   FileMapping *mapping = UtilGetFileMapping(path);
   if (!mapping)
   {
      PrintLastError();
      return;
   }
   long uppers = 0, lows = 0;
   for (int i = 0; i < mapping->fsize; i++)
   {
      if (isupper(mapping->dataPtr[i]))
      {
         uppers++;
      } else if(islower(mapping->dataPtr[i])){
         lows++;
      }
   }
   UtilReleaseFileMapping(mapping);
   *num_of_lows = lows;
   *num_of_uppers = uppers;
   return;
}
void remove_by_index(char *src, int idx)
{
   for (int i = idx; src[i]; i++)
   {
      src[i] = src[i + 1];
   }
}
void UtilFileMappingDeleteStringFromFile(const wchar_t *path, const char *str)
{
   FileMapping *mapping = UtilGetFileMapping(path);
   if (!mapping)
   {
      PrintLastError();
      return;
   }
   int n = strlen(str);
   char *ptr = strstr(mapping->dataPtr, str);
   while (ptr)
   {
      for (int i = 0; i < n; i++)
      {
           remove_by_index(mapping->dataPtr, ptr - mapping->dataPtr);
      }
      ptr = strstr(mapping->dataPtr, str);
   }
   UtilReleaseFileMapping(mapping);
}
int atoi_before_sep(const char* src, char sep){
   int res = 0;
   int negFlag = src[0] == '-' ? 1 : 0;
   for (int i = negFlag; src[i] && src[i] != sep; i++)
   {
        res = 10 * res + src[i] - '0';
   }
   if(negFlag){
      res = -res;
   }
   return res;
}
int int_comparator_decrease(const void *a, const void* b){
    return (*((int *)b) - *((int *)a));
    
}

int* convert_bytes_to_i(const char* bytes, int* size, char sep){
    const int QUOTA = 4;
    int N = 10;
    int* res = (int*) calloc(N, sizeof(int)), *ptr = NULL;
    int j = 0; //actual size of res
    char* prev = bytes;
    int len = strlen(bytes) + 1;
    for (int i = 0; i < len; i++)
    {
         if(bytes[i] == sep || !bytes[i] ){
            if(j == N){
               ptr = (int*)realloc(res, (N + QUOTA) * sizeof(int));
               if(!ptr){
                  free(res);
                  return NULL;
               }
               N += QUOTA;
            }
            res[j] = atoi_before_sep(prev, sep);
            j++;
            prev = bytes + i + 1;
         }
    }
    *size = j;
    return res;
}
void UtilFileMappingSortNumsDecrease(const wchar_t *path) {
   FileMapping *mapping = UtilGetFileMapping(path);
   if (!mapping)
   {
      PrintLastError();
      return;
   }
   int len = 0;
   char buffer[20] = {0};
   int *data = convert_bytes_to_i(mapping->dataPtr, &len, ' ');
   if(!data){
      printf("Can't convert bytes to integers\n");
      UtilReleaseFileMapping(mapping);
      return;
   }
   char* ptr = mapping->dataPtr;
   qsort(data, len, sizeof(int), int_comparator_decrease);
   for (int i = 0; i < len; i++)
   {
      itoa(data[i], buffer, 10);
      strcpy(ptr, buffer);
      ptr += strlen(buffer);
      *ptr = ' ';
      ptr += 1;
   }
   free(data);
   UtilReleaseFileMapping(mapping);
}
void arrange_symbols(){
   const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   puts("Enter file path:");
   //getline(&name, &M, stdin);
   scanf("%S", name);
   UtilFileMappingArrangeBySymbols(name);

}
void count_symbols(){
   const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   puts("Enter file path:");
   scanf("%S", name);
   int up = 0, low = 0;
   UtilFileMappingCalculateSymbols(name, &up, &low);
   printf("Num of uppers = %d\n Num of lowers = %d\n", up, low);
}
void delete_symbols(){
   const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   puts("Enter file path:");
   //getline(&name, &M, stdin);
   scanf("%S", name);
   UtilFileMappingDeleteStringFromFile(name, "a");
}
void sort_nums(){
       const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   puts("Enter file path:");
   //getline(&name, &M, stdin);
   scanf("%S", name);
   UtilFileMappingSortNumsDecrease(name);
}