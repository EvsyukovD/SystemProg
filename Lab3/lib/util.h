#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>
#include <stdio.h>
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
void print_options();
void arrange_symbols();
void count_symbols();
void delete_symbols();
void sort_nums();

ssize_t getline(char **restrict buffer, size_t *restrict size, FILE *restrict fp);
int alphabet_comparator(const void* a, const void* b);
void remove_by_index(char *src, int idx);
int atoi_before_sep(const char* src, char sep);
int int_comparator_decrease(const void *a, const void* b);

typedef struct {
  HANDLE hFile;
  HANDLE hMapping;
  size_t fsize;
  char* dataPtr;
} FileMapping;
int* convert_bytes_to_i(const char* bytes, int* size, char sep);
wchar_t* Char2Wchar(const char *);
char* Wchar2Char(const wchar_t*);
void PrintLastError();
void PrintFileAttributes(DWORD attributes);

WINBOOL UtilCreateFile(const wchar_t* path, const wchar_t* data);
WINBOOL UtilReadFile(const wchar_t* path, wchar_t* buffer,int size);
WINBOOL UtilDeleteFile(const wchar_t* path);
WINBOOL UtilRenameFile(const wchar_t* path, const wchar_t* new_name);
WINBOOL UtilCopyFile(const wchar_t* src_path, const wchar_t* dest_path);
DWORD UtilGetSizeOfFile(const wchar_t* path);
DWORD UtilGetFileAttributes(const wchar_t* path);
WINBOOL UtilSetFileAttributes(const wchar_t* path, DWORD attribute);
void UtilPrintFolderContentRec(const wchar_t* path, int tabs);
void UtilPrintFolderContent(const wchar_t* path);
FileMapping* UtilGetFileMapping(const wchar_t* path);
void UtilReleaseFileMapping(FileMapping* mapping);
void UtilFileMappingArrangeBySymbols(const wchar_t* path);
void UtilFileMappingCalculateSymbols(const wchar_t* path,int* num_of_uppers, int * num_of_lows);
void UtilFileMappingDeleteStringFromFile(const wchar_t* path, const char* str);
void UtilFileMappingSortNumsDecrease(const wchar_t* path);
#endif