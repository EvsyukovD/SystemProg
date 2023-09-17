#include "util.h"
#include <stdio.h>
#include <stdlib.h>
void PrintLastError(){
    char* desc = NULL;
    //FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, &desc , 0, 0);
    printf("UtilError: %d\n",GetLastError());
    //LocalFree(desc);
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
    char* desc = NULL;
    HANDLE file = INVALID_HANDLE_VALUE;
    file = CreateFileW(name,
                       FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                       FILE_SHARE_WRITE | FILE_SHARE_READ,
                       NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        PrintLastError();
        return;
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
DWORD UtilGetSizeOfFile(const wchar_t* path){
    HANDLE file = INVALID_HANDLE_VALUE;
    file = CreateFileW(path,
                       FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                       FILE_SHARE_WRITE | FILE_SHARE_READ,
                       NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if(file == INVALID_HANDLE_VALUE){
        PrintLastError();
        return -1;
    }
    DWORD size = GetFileSize(file, 0);
    CloseHandle(file);
    return size;
}
WINBOOL UtilDeleteFile(const wchar_t* path){
    WINBOOL res = DeleteFileW(path);
    if(!res){
       PrintLastError();
    }
    return res;
}
WINBOOL UtilRenameFile(const wchar_t* path, const wchar_t* new_name){
    WINBOOL res = MoveFileW(path, new_name);
    if(!res){
        PrintLastError();
    }
    return res;
}
void PrintFileAttributes(DWORD attributes){
     if(attributes & FILE_ATTRIBUTE_READONLY){
        printf("ReadOnly ");
     }
     if(attributes & FILE_ATTRIBUTE_HIDDEN){
        printf("Hidden ");
     }
     if(attributes & FILE_ATTRIBUTE_SYSTEM){
        printf("System file ");
     }
     if(attributes & FILE_ATTRIBUTE_DIRECTORY){
        printf("Directory ");
     }
     if(attributes & FILE_ATTRIBUTE_ARCHIVE){
        printf("Archive ");
     }
     if(attributes & FILE_ATTRIBUTE_DEVICE){
        printf("Reserved for system ");
     }
     if(attributes & FILE_ATTRIBUTE_NORMAL){
        printf("Doesn't have other attributes ");
     }
     if(attributes & FILE_ATTRIBUTE_TEMPORARY){
        printf("Temporary ");
     }
     if(attributes & FILE_ATTRIBUTE_SPARSE_FILE){
        printf("Sparse ");
     }
     if(attributes & FILE_ATTRIBUTE_REPARSE_POINT){
        printf("Symbolic link ");
     }
     if(attributes & FILE_ATTRIBUTE_COMPRESSED){
        printf("Compressed ");
     }
     if(attributes & FILE_ATTRIBUTE_OFFLINE){
        printf("Don't available immediately ");
     }
     if(attributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED){
        printf("Content not indexed ");
     }
     if(attributes & FILE_ATTRIBUTE_ENCRYPTED){
        printf("Is encrypted ");
     }
     if(attributes & FILE_ATTRIBUTE_VIRTUAL){
        printf("Value reserved ");
     }
     printf("\n");
}
DWORD UtilGetFileAttributes(const wchar_t* path){
    DWORD attributes = GetFileAttributesW(path);
    if(attributes == INVALID_FILE_ATTRIBUTES){
       PrintLastError();
       return INVALID_FILE_ATTRIBUTES; 
    }
    //wprintf(L"File attributes for file %S:\n", path);
    //PrintFileAttributes(attributes);
    return attributes;
}
WINBOOL UtilCopyFile(const wchar_t* src_path, const wchar_t* dest_path){
   WINBOOL res = CopyFileW(src_path, dest_path, 0);
   if(!res){
      PrintLastError();
   }
   return res;
}
WINBOOL UtilSetFileAttributes(const wchar_t* path, DWORD attribute){
    WINBOOL res = SetFileAttributesW(path, attribute);
    if(!res){
        PrintLastError();
    }
    return res;
}
void UtilPrintFolderContentRec(const wchar_t* root){
    WIN32_FIND_DATA fd;
    int len = 0;
    const int N = 1024;
    wchar_t buf[N];
    //int dir = 0, file = 0;
    HANDLE hFind = FindFirstFileW(root, &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
    do
        {
            if (fd.dwFileAttributes != FILE_ATTRIBUTE_REPARSE_POINT)
            {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    printf("Folder: %S\n", fd.cFileName);
                    //GetFullPathNameW(fd.cFileName,N ,buf, NULL);
                    //UtilPrintFolderContentRec(fd.cFileName);
                   // dir++;
                }
                else
                {
                    printf("File: %S\n", fd.cFileName);
                    //file++;
                }
            }
        }
        while(FindNextFileW(hFind, &fd));
       // printf("%d\n", file);
       // printf("%d\n", dir);
        FindClose(hFind);
    }
}

void create_file(){
     const int N = 1024;
     wchar_t name[N];
     wchar_t data[N];
     puts("Enter file path:");
     scanf(L"%S", name);
     puts("Enter data for this file:");
     scanf(L"%S", data);
     UtilCreateFile(name, data);
}
void read_file(){
     
}
void delete_file(){
     const int N = 1024;
     wchar_t name[N];
     puts("Enter file path:");
     scanf(L"%S", name);
     UtilDeleteFile(name);
}
void rename_file(){
     const int N = 1024;
     wchar_t old_name[N];
     wchar_t new_name[N];
     puts("Enter old file name:");
     scanf(L"%S", old_name);
     puts("Enter new file name:");
     scanf(L"%S", new_name);
     UtilRenameFile(old_name, new_name);
}
void copy_file(){
   const int N = 1024;
     wchar_t src[N];
     wchar_t dest[N];
     puts("Enter src file name:");
     scanf("%S", src);
     puts("Enter dest file name:");
     scanf(L"%S", dest);
     UtilCopyFile(src, dest);
}
void get_size(){
     const int N = 1024;
     wchar_t name[N];
     puts("Enter file path:");
     scanf(L"%S", name);
     DWORD res = UtilGetSizeOfFile(name);
     printf("Size of this file: %ld bytes", res);
}
void get_attributes(){
     const int N = 1024;
     wchar_t name[N];
     puts("Enter file path:");
     scanf(L"%S", name);
     DWORD attributes = UtilGetFileAttributes(name);
     wprintf(L"File attributes for file %S:\n", name);
     PrintFileAttributes(attributes);
}
void set_attributes(){
     const int N = 1024;
     wchar_t name[N];
     puts("Enter file path:");
     scanf(L"%S", name);
     puts("Make readonly (type 1) file or hidden (type 0):");
     int type = 0;
     scanf("%d",&type);
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
void print_folder_content(){
    
}