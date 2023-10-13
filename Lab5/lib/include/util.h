#ifndef UTIL_H
#define UTIL_H
#include "../include/list.h"
typedef struct
{
    int value;
} Data;
Data* CreateData();
void ReleaseData(void*);
LinkedList* globalList = NULL;
LinkedList* GetGlobalList();
void AddDataToGlobalList();
Data* TakeDataFromGlobalList();
#endif