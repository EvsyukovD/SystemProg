#ifndef UTIL_H
#define UTIL_H
#include "list.h"
#include <windows.h>
typedef struct
{
    int value;
} Data;
Data* CreateData();
void ReleaseData(void*);
extern LinkedList* globalList;
LinkedList* GetGlobalList();
LinkedList* InitGlobalList();
void EraseGlobalList(void (*ReleaseData)(void *));
int AddDataToGlobalList(LPVOID);
int TakeDataFromGlobalList(LPVOID);

int InitRWContext(int argc,char* argv[]);
void DeInitializeRWContext();
int WriteToFile(LPVOID data);
int ReadFromFileAndPrint();

int ThreadIncrementCounter_1(LPVOID);
int ThreadIncrementCounter_2(LPVOID);
int ThreadIncrementCounter_3(LPVOID);

extern void PrintIntList(LinkedList*);
void ProducerAndConsumer(int argc, char* argv[]);
void ReadAndWrite(int argc, char* argv[]);
void ThreadsRace(int argc,char* argv[]);
void print_options();
#endif