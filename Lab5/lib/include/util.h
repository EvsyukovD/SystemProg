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
extern void PrintIntList(LinkedList*);
void ProducerAndConsumer(int argc, char* argv[]);
void print_options();
#endif