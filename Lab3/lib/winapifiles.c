#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
void PrintLastError()
{
   char *desc = NULL;
   printf("UtilError: %d\n", GetLastError());
}
wchar_t *Char2Wchar(const char *str)
{
   wchar_t *wstr = NULL;
   int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
   wstr = (wchar_t *)calloc(size, sizeof(wchar_t));
   if (!wstr)
   {
      return NULL;
   }
   MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, size * sizeof(wchar_t));
   return wstr;
}
char *Wchar2Char(const wchar_t *wstr)
{
   char *str = NULL;
   int size = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
   str = (char *)calloc(size, sizeof(char));
   if (!str)
   {
      return NULL;
   }
   WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, size * sizeof(char), NULL, NULL);
   return str;
}
WINBOOL UtilCreateFile(const wchar_t *name, const wchar_t *data)
{
   int error;
   char *desc = NULL;
   HANDLE file = INVALID_HANDLE_VALUE;
   file = CreateFileW(name,
                      FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                      FILE_SHARE_WRITE | FILE_SHARE_READ,
                      NULL, CREATE_ALWAYS,
                      FILE_ATTRIBUTE_NORMAL, NULL);
   if (file == INVALID_HANDLE_VALUE)
   {
      PrintLastError();
      return 0;
   }
   long unsigned int written_bytes = 0;
   int len = (wcslen(data)) * sizeof(wchar_t);
   WINBOOL res = WriteFile(file, data, len, &written_bytes, NULL);
   if (!res)
   {
      PrintLastError();
      return res;
   }
   CloseHandle(file);
   return res;
}
DWORD UtilGetSizeOfFile(const wchar_t *path)
{
   HANDLE file = INVALID_HANDLE_VALUE;
   file = CreateFileW(path,
                      FILE_GENERIC_READ,
                      FILE_SHARE_READ,
                      NULL, OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL, NULL);
   if (file == INVALID_HANDLE_VALUE)
   {
      PrintLastError();
      return -1;
   }
   DWORD size = GetFileSize(file, 0);
   CloseHandle(file);
   return size;
}
WINBOOL UtilDeleteFile(const wchar_t *path)
{
   WINBOOL res = DeleteFileW(path);
   if (!res)
   {
      PrintLastError();
   }
   return res;
}
WINBOOL UtilRenameFile(const wchar_t *path, const wchar_t *new_name)
{
   WINBOOL res = MoveFileW(path, new_name);
   if (!res)
   {
      PrintLastError();
   }
   return res;
}
WINBOOL UtilReadFile(const wchar_t *path, wchar_t *buffer, int size)
{
   HANDLE file = INVALID_HANDLE_VALUE;
   file = CreateFileW(path,
                      FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                      FILE_SHARE_WRITE | FILE_SHARE_READ,
                      NULL, OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL, NULL);
   if (file == INVALID_HANDLE_VALUE)
   {
      PrintLastError();
      return 0;
   }
   LPDWORD written_bytes = 0;
   if (!ReadFile(file, buffer, size, written_bytes, NULL))
   {
      PrintLastError();
      CloseHandle(file);
      return 0;
   }
   CloseHandle(file);
   return 1;
}

void PrintFileAttributes(DWORD attributes)
{
   if (attributes & FILE_ATTRIBUTE_READONLY)
   {
      printf("ReadOnly ");
   }
   if (attributes & FILE_ATTRIBUTE_HIDDEN)
   {
      printf("Hidden ");
   }
   if (attributes & FILE_ATTRIBUTE_SYSTEM)
   {
      printf("System file ");
   }
   if (attributes & FILE_ATTRIBUTE_DIRECTORY)
   {
      printf("Directory ");
   }
   if (attributes & FILE_ATTRIBUTE_ARCHIVE)
   {
      printf("Archive ");
   }
   if (attributes & FILE_ATTRIBUTE_DEVICE)
   {
      printf("Reserved for system ");
   }
   if (attributes & FILE_ATTRIBUTE_NORMAL)
   {
      printf("Doesn't have other attributes ");
   }
   if (attributes & FILE_ATTRIBUTE_TEMPORARY)
   {
      printf("Temporary ");
   }
   if (attributes & FILE_ATTRIBUTE_SPARSE_FILE)
   {
      printf("Sparse ");
   }
   if (attributes & FILE_ATTRIBUTE_REPARSE_POINT)
   {
      printf("Symbolic link ");
   }
   if (attributes & FILE_ATTRIBUTE_COMPRESSED)
   {
      printf("Compressed ");
   }
   if (attributes & FILE_ATTRIBUTE_OFFLINE)
   {
      printf("Don't available immediately ");
   }
   if (attributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
   {
      printf("Content not indexed ");
   }
   if (attributes & FILE_ATTRIBUTE_ENCRYPTED)
   {
      printf("Is encrypted ");
   }
   if (attributes & FILE_ATTRIBUTE_VIRTUAL)
   {
      printf("Value reserved ");
   }
   printf("\n");
}
DWORD UtilGetFileAttributes(const wchar_t *path)
{
   DWORD attributes = GetFileAttributesW(path);
   if (attributes == INVALID_FILE_ATTRIBUTES)
   {
      PrintLastError();
      return INVALID_FILE_ATTRIBUTES;
   }
   // wprintf(L"File attributes for file %S:\n", path);
   // PrintFileAttributes(attributes);
   return attributes;
}
WINBOOL UtilCopyFile(const wchar_t *src_path, const wchar_t *dest_path)
{
   WINBOOL res = CopyFileW(src_path, dest_path, 0);
   if (!res)
   {
      PrintLastError();
   }
   return res;
}
WINBOOL UtilSetFileAttributes(const wchar_t *path, DWORD attribute)
{
   WINBOOL res = SetFileAttributesW(path, attribute);
   if (!res)
   {
      PrintLastError();
   }
   return res;
}
void UtilPrintFolderContentRec(const wchar_t *root, int tabs)
{
   WIN32_FIND_DATA fd;
   int len = wcslen(root) + 4;
   const int N = 1024;
   wchar_t buf[N];
   wcscpy(buf, root);
   buf[len - 1] = '\0';
   buf[len - 2] = '*';
   buf[len - 3] = 92;
   buf[len - 4] = 92;
   HANDLE hFind = FindFirstFileW(buf, &fd);
   if (hFind != INVALID_HANDLE_VALUE)
   {
      do
      {
         if (fd.dwFileAttributes != FILE_ATTRIBUTE_REPARSE_POINT)
         {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
               for (int i = 0; i < tabs; i++)
               {
                  printf("\t");
               }
               printf("Folder: %S\n", fd.cFileName);
               if (wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L".."))
               {
                  buf[len - 1] = 0;
                  buf[len - 2] = 0;
                  buf[len - 3] = 0;
                  buf[len - 4] = 92;
                  wcscat(buf, fd.cFileName);
                  UtilPrintFolderContentRec(buf, tabs + 1);
               }
            }
            else
            {
               for (int i = 0; i < tabs; i++)
               {
                  printf("\t");
               }
               printf("File: %S\n", fd.cFileName);
            }
         }
      } while (FindNextFileW(hFind, &fd));
      FindClose(hFind);
   }
   else
   {
      PrintLastError();
   }
}
void UtilPrintFolderContent(const wchar_t *path)
{
   UtilPrintFolderContentRec(path, 0);
}
/*
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
*/
void create_file()
{
   const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   wchar_t data[N];
   puts("Enter file path:");
   scanf("%S", name);
   //getline(&name, &M, stdin);
   puts("Enter data for this file:");
   // note: use getline
   scanf("%S", data);
   //getline(&data, &M, stdin);

   UtilCreateFile(name, data);
}
void read_file()
{
   const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   puts("Enter file path:");
   scanf("%S", name);
   //getline(&name, &M, stdin);
   wchar_t content[N];
   UtilReadFile(name, content, N);
   printf("Data from file:\n");
   printf("%S", content);
   puts("");
}
void delete_file()
{
   const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   puts("Enter file path:");
  scanf("%S", name);
  // getline(&name, &M, stdin);
   if (UtilDeleteFile(name))
   {
      puts("File is deleted");
   }
}
void rename_file()
{
   const int N = 1024;
   const int M = N - 1;
   wchar_t old_name[N];
   wchar_t new_name[N];
   puts("Enter old file name:");
   scanf("%S", old_name);
   //getline(&old_name, &M, stdin);
   puts("Enter new file name:");
   //getline(&new_name, &M, stdin);
   scanf("%S", new_name);
   if (UtilRenameFile(old_name, new_name))
   {
      printf("File was renamed to %S", new_name);
   }
}
void copy_file()
{
   const int N = 1024;
   const int M = N - 1;
   wchar_t src[N];
   wchar_t dest[N];
   puts("Enter src file name:");
   //getline(&src, &M, stdin);
   scanf("%S", src);
   puts("Enter dest file name:");
   //getline(&dest, &M, stdin);
   scanf("%S", dest);
   UtilCopyFile(src, dest);
}
void get_size()
{
   const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   puts("Enter file path:");
   //getline(&name, &M, stdin);
   scanf("%S", name);
   DWORD res = UtilGetSizeOfFile(name);
   printf("Size of this file: %ld bytes", res);
}
void get_attributes()
{
   const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   puts("Enter file path:");
   //getline(&name, &M, stdin);
   scanf("%S", name);
   DWORD attributes = UtilGetFileAttributes(name);
   printf("File attributes for file %S:\n", name);
   PrintFileAttributes(attributes);
}
void set_attributes()
{
   const int N = 1024;
   const int M = N - 1;
   wchar_t name[N];
   puts("Enter file path:");
   //getline(&name, &M, stdin);
   scanf("%S", name);
   puts("Make readonly (type 1) file or hidden (type 0):");
   int type = 0;
   scanf("%d", &type);
   DWORD attributes = UtilGetFileAttributes(name);
   switch (type)
   {
   case 1:
      UtilSetFileAttributes(name, attributes | FILE_ATTRIBUTE_READONLY);
      break;
   case 0:
      UtilSetFileAttributes(name, attributes | FILE_ATTRIBUTE_HIDDEN);
      break;
   default:
      break;
   }
}
void print_folder_content()
{
   const int N = 1024;
   const int M = N - 1;
   wchar_t root[N];
   puts("Enter root path:");
   //getline(&root, &M, stdin);
   scanf("%S", root);
   UtilPrintFolderContent(root);
}
void print_options()
{
   char *options[] = {"0.Quit", "1.Create file", "2.Read file", "3.Delete file", "4.Rename file", "5.Copy file",
                      "6.Get size of file", "7.Get file attributes", "8.Set attributes", "9.Print folder content recursively",
                      "10.Arrange symbols","11.Count symbols","12.Delete symbols","13.Sort nums"};
   int options_num = sizeof(options) / sizeof(options[0]);
   for (int i = 0; i < options_num; i++)
   {
      printf("%s\n", options[i]);
   }
}
/*
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
*/