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
WINBOOL UtilFileMappingArrangeBySymbols(const wchar_t *path)
{
   FileMapping *mapping = UtilGetFileMapping(path);
   if (!mapping)
   {
      PrintLastError();
      return 0;
   }
   qsort(mapping->dataPtr, mapping->fsize, sizeof(char), alphabet_comparator);
   UtilReleaseFileMapping(mapping);
   return 1;
}
WINBOOL UtilFileMappingCalculateSymbols(const wchar_t *path, int* num_of_uppers, int* num_of_lows)
{
   FileMapping *mapping = UtilGetFileMapping(path);
   if (!mapping)
   {
      PrintLastError();
      return 0;
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
   return 1;
}
WINBOOL UtilFileMappingDeleteStringFromFile(const wchar_t *path, const char *str)
{
   FileMapping *mapping = UtilGetFileMapping(path);
   if (!mapping)
   {
      PrintLastError();
      return 0;
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
   return 1;
}
WINBOOL UtilFileMappingSortNumsDecrease(const wchar_t *path) {
   FileMapping *mapping = UtilGetFileMapping(path);
   if (!mapping)
   {
      PrintLastError();
      return 0;
   }
   int len = 0;
   char buffer[20] = {0};
   int *data = convert_bytes_to_i(mapping->dataPtr, &len, ' ');
   if(!data){
      printf("Can't convert bytes to integers\n");
      UtilReleaseFileMapping(mapping);
      return 0;
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
   return 1;
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