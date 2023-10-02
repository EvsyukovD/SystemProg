#ifndef UTIL_H
#define UTIL_H
#include <tchar.h>
#include <windows.h>
typedef struct {
  DWORD processID;
  TCHAR* processName;
  HMODULE hmod;
} UtilProcessInfo;
void UtilReleaseProcessInfo(UtilProcessInfo*);
void UtilCreateProcess(TCHAR *path);
WINBOOL UtilGetCurrentProcesses(DWORD *aProcesses, DWORD *num_of_procs, int size);
void PrintUtilProcessInfo(DWORD processID);
void UtilCreateRandomThreads();
void UtilPEBInformation(DWORD Pid);

void create_process_args(int argc, TCHAR *argv[]);
void print_processes_args(int argc, TCHAR *argv[]);
void create_random_args(int argc, TCHAR *argv[]);
void print_peb_info_args(int argc, TCHAR *argv[]);
void print_options();
#endif