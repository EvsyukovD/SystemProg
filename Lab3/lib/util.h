#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>
#include <windows.h>
void create_file();
void read_file();
void delete_file();
void rename_file();
void copy_file();
void get_size();
void get_attributes();
void set_attributes();
void print_folder_content();

wchar_t* Char2Wchar(const char *);
char* Wchar2Char(const wchar_t*);
WINBOOL UtilCreateFile(const wchar_t* path, const wchar_t* data);
wchar_t* UtilReadFile(const wchar_t* path);
WINBOOL UtilDeleteFile(const wchar_t* path);
WINBOOL UtilRenameFile(const wchar_t* path, const wchar_t* new_name);
WINBOOL UtilCopyFile(const wchar_t* src_path, const wchar_t* dest_path);
DWORD UtilGetSizeOfFile(const wchar_t* path);
DWORD UtilGetFileAttributes(const wchar_t* path);
WINBOOL UtilSetFileAttributes(const wchar_t* path, DWORD attribute);
void UtilPrintFolderContentRec(const wchar_t* path);
void PrintLastError();
void PrintFileAttributes(DWORD attributes);
#endif