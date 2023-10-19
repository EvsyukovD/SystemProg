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
int AddDataToGlobalList(int millis);
int TakeDataFromGlobalList(int millis);

int InitRWContext(int argc,char* argv[]);
void DeInitializeRWContext();
int WriteToFile(LPVOID data);
int ReadFromFileAndPrint();

void ThreadIncrementCounter_1(LPVOID);
void ThreadIncrementCounter_2(LPVOID);
void ThreadIncrementCounter_3(LPVOID);

extern void PrintIntList(LinkedList*);
void ProducerAndConsumer(int argc, char* argv[]);
void ReadAndWrite(int argc, char* argv[]);
void ThreadsRace();
void print_options();
#endif