#include "../include/util.h"
#include "../include/common.h"
#include <stdlib.h>
#include <stdio.h>
#define MAX_LIST_SIZE 5
CRITICAL_SECTION bufferLock = {0};
CONDITION_VARIABLE bufferNotEmpty;
CONDITION_VARIABLE bufferNotFull;
WINBOOL finishProduceAndConsume = 0;

LinkedList* globalList = NULL;
LinkedList* GetGlobalList(){
    return globalList;
}
LinkedList* InitGlobalList(){
    if(!globalList){
        globalList = CreateList();
    }
    return globalList;
}
void EraseGlobalList(void (*ReleaseData)(void *)){
     DeepErase(globalList, ReleaseData);
     globalList = NULL;
}

Data* CreateData(){
    return (Data*)calloc(1, sizeof(Data));
}
void ReleaseData(void* pData){
   if(pData){
      free(pData);
   }
}
void PrintIntList(LinkedList* list){
     Item* it = FindByIndex(list, 0);
     printf("[");
     while(it){
        printf("%d%s", *((int*)it->data), it->next ? ", ": "]\n");
        it = it->next;
     }
}
int AddDataToGlobalList(int millis){
        srand(time(NULL));
 /*  // convert parameter to mutex handle
   HANDLE hMutex = (HANDLE)lpMutex;
   // save the thread ID
   DWORD dwThreadID = GetCurrentThreadId();
   DWORD dwResult = WaitForSingleObject(hMutex, INFINITE);
   if (dwResult == WAIT_OBJECT_0)
   {
      
      LinkedList* list = GetGlobalList();
      int* p = (int*)calloc(1, sizeof(int));
      *p = rand();
      printf("Thread %p acquired mutex.Add data %d\n", dwThreadID, *p);
      Add(list, p);
      printf("Current list:\n");
      PrintIntList(list);
      // hold the mutex
      Sleep(1000);
      printf("Thread %p releasing the mutex.\n", dwThreadID);
      ReleaseMutex(hMutex);
   }
   else
   {
      return 1;
   }
   return 0;*/
        while(TRUE){
              Sleep(5000);
              EnterCriticalSection(&bufferLock);
              while(!finishProduceAndConsume && GetGlobalList()->size == MAX_LIST_SIZE){
                     SleepConditionVariableCS(&bufferNotFull,&bufferLock,INFINITE);
              }
              if(finishProduceAndConsume){
                 LeaveCriticalSection(&bufferLock);
                 break;
              }
              Data* data = (Data*) calloc(1, sizeof(Data));
              data->value = rand() % 100;
              printf("Thread with id = %d add data = %d to list\n",GetCurrentThreadId(),data->value);
              Add(GetGlobalList(), data);
              printf("Current list: \n");
              PrintIntList(GetGlobalList());
              LeaveCriticalSection(&bufferLock);
              WakeConditionVariable(&bufferNotEmpty);
        }
        return 0;
}
int TakeDataFromGlobalList(int millis){
  /*  // convert parameter to mutex handle
   HANDLE hMutex = (HANDLE)lpMutex;
   // save the thread ID
   DWORD dwThreadID = GetCurrentThreadId();
   DWORD dwResult = WaitForSingleObject(hMutex, INFINITE);
   if (dwResult == WAIT_OBJECT_0)
   {
      printf("Thread %p acquired mutex.Take data\n", dwThreadID);
      // hold the mutex
      LinkedList* list = GetGlobalList();
      RemoveByIndex(list, 0);
      printf("Current list:\n");
      PrintIntList(list);
      Sleep(1000);
      printf("Thread %p releasing the mutex.\n", dwThreadID);
      ReleaseMutex(hMutex);
   }
   else
   {
      return 1;
   }
   return 0;*/
        while(TRUE){
              EnterCriticalSection(&bufferLock);
              while(!finishProduceAndConsume && GetGlobalList()->size == 0){
                     SleepConditionVariableCS(&bufferNotEmpty,&bufferLock,INFINITE);
              }
              if(finishProduceAndConsume){
                 LeaveCriticalSection(&bufferLock);
                 break;
              }
              Data* data = RemoveByIndex(GetGlobalList(),0);
              printf("Thread with id = %d take data = %d from list\n",GetCurrentThreadId(),data->value);
              ReleaseData(data);
              printf("Current list: \n");
              PrintIntList(GetGlobalList());
              LeaveCriticalSection(&bufferLock);
              WakeConditionVariable(&bufferNotFull);
              Sleep(5000);
        }
        return 0;
}

void ProducerAndConsumer(int argc, char* argv[]){
     if(argc > 2){
        InitGlobalList();
        InitializeConditionVariable(&bufferNotEmpty);
        InitializeConditionVariable(&bufferNotFull);
        InitializeCriticalSection(&bufferLock);
        int prodNumber = atoi(argv[1]), consNumber = atoi(argv[2]);
        int millis = 5000;
        if(argc > 3){
            millis = atoi(argv[3]);
        }
        DWORD dwNewThreadID;
        const int MAX_THREADS = 20;
        HANDLE threads[MAX_THREADS];
        int NumThreads = prodNumber + consNumber;
        int (*fptr)(LPVOID) = NULL;
        for (int i = 0; i < NumThreads;i++)
        {
            fptr = i < prodNumber ? AddDataToGlobalList : TakeDataFromGlobalList;
            threads[i] = CreateThread(NULL, 0, fptr, 0, 0, &dwNewThreadID);
            printf("%s thread with id = %p created.\n", i < prodNumber ? "Producer" : "Consumer",dwNewThreadID);
        }
        int c = 1;
        printf("To stop producers and consumers press enter\n");
        scanf("%c", &c);
        EnterCriticalSection (&bufferLock);
        finishProduceAndConsume = TRUE;
        LeaveCriticalSection (&bufferLock);
        WakeAllConditionVariable (&bufferNotFull);
        WakeAllConditionVariable (&bufferNotEmpty);
        for (int i = 0; i < NumThreads; i++)
        {
            WaitForSingleObject(threads[i],INFINITE);
        }
        printf("Current global list:\n");
        PrintIntList(GetGlobalList());
        EraseGlobalList(ReleaseData);
     }
}
void print_options()
{
   char *options[] = {"1.Create producers and consumers [number of producers] [number of consumers] [[opt] delay in millis]", 
                      "2.Create readers and writers [number of producers] [number of consumers] [path to file]", 
                      "3.Create threads"
                       };
   int options_num = sizeof(options) / sizeof(options[0]);
   for (int i = 0; i < options_num; i++)
   {
      printf("%s\n", options[i]);
   }
}