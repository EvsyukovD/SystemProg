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
int UtilCreateProcess(int argc, TCHAR *argv[]);
WINBOOL UtilGetCurrentProcesses(DWORD *aProcesses, DWORD *num_of_procs);
void PrintUtilProcessInfo(DWORD processID);
#endif