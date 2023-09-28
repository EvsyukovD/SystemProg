#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>
#include <stdio.h>
#include <windows.h>
void create_file_dialog();
void read_file_dialog();
void delete_file_dialog();
void rename_file_dialog();
void copy_file_dialog();
void get_size_dialog();
void get_attributes_dialog();
void set_attributes_dialog();
void print_folder_content_dialog();
void print_options();
void arrange_symbols_dialog();
void count_symbols_dialog();
void delete_symbols_dialog();
void sort_nums_dialog();


void create_file_args(int argc, char *argv[]);
void read_file_args(int argc, char *argv[]);
void delete_file_args(int argc, char *argv[]);
void rename_file_args(int argc, char *argv[]);
void copy_file_args(int argc, char *argv[]);
void get_size_args(int argc, char *argv[]);
void get_attributes_args(int argc, char *argv[]);
void set_attributes_args(int argc, char *argv[]);
void print_folder_content_args(int argc, char *argv[]);
void arrange_symbols_args(int argc, char *argv[]);
void count_symbols_args(int argc, char *argv[]);
void delete_symbols_args(int argc, char *argv[]);
void sort_nums_args(int argc, char *argv[]);

ssize_t getline(char **restrict buffer, size_t *restrict size, FILE *restrict fp);
/**
 * @brief Simple compare of 2 bytes
 * @param a  1-st byte
 * @param b  2-nd byte
 * @return a - b
*/
int alphabet_comparator(const void* a, const void* b);
/**
 * @brief Remove bytes by left shift of string. Empty positions - space
 * @param src source string
 * @param idx  index of removed bytes
*/
void remove_by_index(char *src, int idx);
/**
 * @brief Convert string before first separator to integer
 * @param src source string
 * @param sep separator char
 * @returns integer
*/
int atoi_before_sep(const char* src, char sep);
/**
 * @brief Comparator of integers for decrease for qsort function
 * @param a first int pointer
 * @param b second int pointer
 * @return b - a
*/
int int_comparator_decrease(const void *a, const void* b);

typedef struct {
  HANDLE hFile; //handle of file
  HANDLE hMapping; //handle for file mapping
  size_t fsize; //size of file
  char* dataPtr; //content of file 
} FileMapping;
/**
 * @brief Convert bytes with separators to integers
 * @param bytes source string
 * @param size number of bytes
 * @param sep separator char
 * @returns int array pointer
*/
int* convert_bytes_to_i(const char* bytes, int* size, char sep);
/**
 * @brief Convert char to wchar
 * @param src source char string
 * @returns wchar array
*/
wchar_t* Char2Wchar(const char *);
/**
 * @brief Convert wchar to char
 * @param src source wchar string
 * @returns char array
*/
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
WINBOOL UtilPrintFolderContentRec(const wchar_t* path, int tabs);
WINBOOL UtilPrintFolderContent(const wchar_t* path);
FileMapping* UtilGetFileMapping(const wchar_t* path);
void UtilReleaseFileMapping(FileMapping* mapping);
WINBOOL UtilFileMappingArrangeBySymbols(const wchar_t* path);
WINBOOL UtilFileMappingCalculateSymbols(const wchar_t* path,int* num_of_uppers, int * num_of_lows);
WINBOOL UtilFileMappingDeleteStringFromFile(const wchar_t* path, const char* str);
WINBOOL UtilFileMappingSortNumsDecrease(const wchar_t* path);
#endif