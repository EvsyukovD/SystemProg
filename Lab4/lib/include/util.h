#ifndef UTIL_H
#define UTIL_H
#include <tchar.h>
#include <windows.h>
#include <winternl.h>
#include "common.h"
void UtilCreateProcess(TCHAR *path);
WINBOOL UtilGetCurrentProcesses(DWORD *aProcesses, DWORD *num_of_procs, int size);

void UtilCreateRandomThreads();
void UtilPrintPEBInformation(PROCESS_BASIC_INFORMATION* info);


void create_process_args(int argc, TCHAR *argv[]);
void print_processes_args(int argc, TCHAR *argv[]);
void create_random_args(int argc, TCHAR *argv[]);
void print_peb_info_args(int argc, TCHAR *argv[]);
void print_extended_process_info_args(int argc, TCHAR* argv[]);
void print_options();
#endif