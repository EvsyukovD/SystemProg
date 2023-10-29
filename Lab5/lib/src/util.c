#include "../include/util.h"
#include "../include/common.h"
#include <stdlib.h>
#include <stdio.h>
#define MAX_LIST_SIZE 5
#define MAX_THREADS 20
#define COUNTER_MAX_VALUE 300
CRITICAL_SECTION bufferLock = {0};
CONDITION_VARIABLE bufferNotEmpty;
CONDITION_VARIABLE bufferNotFull;
WINBOOL finishProduceAndConsume = 0;
LinkedList* globalList = NULL;

HANDLE hFile = NULL;
HANDLE semOutput = NULL;//semaphore for stdout
HANDLE semRCounter = NULL;//semaphore for readersCount 
HANDLE semFile = NULL;//semaphore for file
int readersCount = 0;//for readers function realization

int readersNumber = 0;// number of threads - readers
int writersNumber = 0;// number of threads - writers
WINBOOL finishRW = FALSE;

HANDLE semCounter = NULL;//semaphore for globalCounter
int globalCounter = 0;

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
     if(!it){ //empty list
       printf("]\n");
       return;
     }
     while(it){
        printf("%d%s", *((int*)it->data), it->next ? ", ": "]\n");
        it = it->next;
     }
}
int AddDataToGlobalList(LPVOID){
        srand(time(NULL));
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
              if(data){
              data->value = rand() % 100;
              printf("Thread with id = %d add data = %d to list\n",GetCurrentThreadId(),data->value);
              Add(GetGlobalList(), data);
              printf("Current list: \n");
              PrintIntList(GetGlobalList());
              } else {
               printf("Not enough memory to add data\n");
              }
              LeaveCriticalSection(&bufferLock);
              WakeConditionVariable(&bufferNotEmpty);
        }
        return 0;
}
int TakeDataFromGlobalList(LPVOID){
        while(TRUE){
              EnterCriticalSection(&bufferLock);
              while(!finishProduceAndConsume && GetGlobalList()->size == 0){
                     SleepConditionVariableCS(&bufferNotEmpty,&bufferLock,INFINITE);
              }
              if(finishProduceAndConsume){
                 LeaveCriticalSection(&bufferLock);
                 break;
              }
              Data* data = RemoveByIndex(GetGlobalList(),GetGlobalList()->size - 1);
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
int InitRWContext(int argc,char* argv[]){
        if(argc <= 4){
           printf("Not enough args\n");
           return FALSE;
        }
        writersNumber = atoi(argv[2]); 
        readersNumber = atoi(argv[3]);
        if (writersNumber + readersNumber > MAX_THREADS)
        {
              printf("Too many threads. The number of threads should be not more than %d\n", MAX_THREADS);
              writersNumber = 0;
              readersNumber = 0;
              return FALSE;
        }
        int threadsNumber = writersNumber + readersNumber;
        semRCounter = CreateSemaphoreA(NULL,1,1,"RCounter");
        semFile = CreateSemaphoreA(NULL,1,1,"file");
        semOutput = CreateSemaphoreA(NULL,1,1,"output");
        hFile = UtilGetFileHandle(argv[4]);
        if(!hFile || hFile == INVALID_HANDLE_VALUE){
           CloseHandle(semCounter);
           CloseHandle(semFile);
           CloseHandle(semOutput);
           PrintLastError();
           return FALSE;
        }
        return TRUE;
}
void DeInitializeRWContext()
{
     CloseHandle(semFile);
     CloseHandle(semRCounter);
     CloseHandle(semOutput);
     CloseHandle(hFile);
}
void SecurePrint(const char* format,...){
   WaitForSingleObject(semOutput,INFINITE);
   va_list arglist;
   va_start( arglist, format );
   vprintf( format, arglist );
   va_end( arglist );
   ReleaseSemaphore(semOutput,1,NULL);
}
int WriteToFile(LPVOID data){
    while(TRUE){
          Sleep(5000);
          WaitForSingleObject(semFile,INFINITE);
          SecurePrint("Writer %d is writing data %s\n",GetCurrentThreadId(),data);
          if(!WriteFile(hFile,(char*)data,strlen((char*)data),NULL,NULL)){
             SecurePrint("Thread %d can't write to file\n",GetCurrentThreadId());
             PrintLastError();
             break;
          }
          ReleaseSemaphore(semFile,1,NULL);
          if(finishRW){
            break;
          }
    }
    return 0;
}
int ReadFromFileAndPrint(){
    char* buffer[1024] = {0};
    WINBOOL isFileCaptured = FALSE; //file captured
    while(TRUE){
          WaitForSingleObject(semRCounter,INFINITE);
          readersCount++;
          if(readersCount == 1){
             WaitForSingleObject(semFile,INFINITE);
             SetFilePointer(hFile,0,0,FILE_BEGIN);
             isFileCaptured = TRUE;
          }
          ReleaseSemaphore(semRCounter,1,NULL);
          if(isFileCaptured){
             if (!ReadFile(hFile, buffer, 1023, NULL, NULL))
             {
                SecurePrint("Thread %d can't read from file\n", GetCurrentThreadId());
                SecurePrint("Util error: %d\n", GetLastError());
                break;
             }
             else
             {
                SetFilePointer(hFile, 0, 0, FILE_END);
                SecurePrint("Data from file (thread %d)[\n%s\n]\n", GetCurrentThreadId(), buffer);
             }
          }
          WaitForSingleObject(semRCounter,INFINITE);
          readersCount--;
          if(!readersCount){
             ReleaseSemaphore(semFile,1,NULL);
             isFileCaptured = FALSE;
          }
          ReleaseSemaphore(semRCounter,1,NULL);
          if(finishRW){
            break;
          }
          Sleep(5000);
    }
    return 0;
}

int ThreadIncrementCounter_1(LPVOID){
     int delta = 1;
     while(TRUE){
          Sleep(1000);
          WaitForSingleObject(semCounter,INFINITE);
          if(globalCounter <= COUNTER_MAX_VALUE - delta){
             globalCounter = globalCounter + delta;
             SecurePrint("Thread 1 increment counter on %d. New counter value = %d\n",delta,globalCounter);
          }else{
            ReleaseSemaphore(semCounter,1,NULL);
            SecurePrint("Counter has big value. Thread %d exit\n",GetCurrentThreadId());
            break;
          }
          ReleaseSemaphore(semCounter,1,NULL);
          
     }
     return 0;
}
int ThreadIncrementCounter_2(LPVOID ptr){
     HANDLE *hThread = (HANDLE *)ptr;
     int delta = 20;
     int stopVal = 50;
     WINBOOL isSuspended = FALSE;
     while (TRUE)
     {
          Sleep(1000);
          WaitForSingleObject(semCounter, INFINITE);
          if (globalCounter <= COUNTER_MAX_VALUE - delta)
          {
            globalCounter = globalCounter + delta;
            SecurePrint("Thread 2 increment counter on %d. New counter value = %d\n", delta, globalCounter);
            if (globalCounter >= stopVal && !isSuspended)
            {
                SuspendThread(*hThread);
                isSuspended = TRUE;
            }
          }
          else
          {
            ReleaseSemaphore(semCounter, 1, NULL);
            SecurePrint("Counter has big value. Thread %d exit\n", GetCurrentThreadId());
            break;
          }
          ReleaseSemaphore(semCounter, 1, NULL);
     }
     return 0;
}
int ThreadIncrementCounter_3(LPVOID ptr){
     HANDLE *hThread = (HANDLE *)ptr;
     int delta = 30;
     int continueVal = 250;
     WINBOOL isResumed = FALSE;
     while (TRUE)
     {
          Sleep(1000);
          WaitForSingleObject(semCounter, INFINITE);
          if (globalCounter <= COUNTER_MAX_VALUE - delta)
          {
            globalCounter = globalCounter + delta;
            SecurePrint("Thread 3 increment counter on %d. New counter value = %d\n", delta, globalCounter);
            if (globalCounter >= continueVal && !isResumed)
            {
                ResumeThread(*hThread);
                isResumed = TRUE;
            }
          }
          else
          {
            ReleaseSemaphore(semCounter, 1, NULL);
            SecurePrint("Counter has big value. Thread %d exit\n", GetCurrentThreadId());
            break;
          }
          ReleaseSemaphore(semCounter, 1, NULL);
     }
     return 0;
}

void ProducerAndConsumer(int argc, char* argv[]){
     if(argc > 3){
        int prodNumber = atoi(argv[2]), consNumber = atoi(argv[3]);
        if(prodNumber + consNumber > MAX_THREADS){
           printf("Too many threads. The number of threads should be not more than %d\n",MAX_THREADS);
           return;
        }
        InitGlobalList();
        InitializeConditionVariable(&bufferNotEmpty);
        InitializeConditionVariable(&bufferNotFull);
        InitializeCriticalSection(&bufferLock);
        DWORD dwNewThreadID;
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
        DeleteCriticalSection(&bufferLock);
     }
}

void ReadAndWrite(int argc, char* argv[]){
     if(!InitRWContext(argc,argv)){
        return;
     }
     HANDLE threads[MAX_THREADS];
     DWORD threadId;
     int threadsNumber = writersNumber + readersNumber;
     const char* data = "a";
     for (int i = 0; i < threadsNumber; i++)
     {    
          if(i < readersNumber){
              threads[i] = CreateThread(NULL,0,ReadFromFileAndPrint,0,0,&threadId);
              SecurePrint("Reader %d created\n",threadId);
          } else {
              threads[i] = CreateThread(NULL,0,WriteToFile,argc > 5 ? argv[5] : data,0,&threadId);
              SecurePrint("Writer %d created\n",threadId);
          }

     }
     char c = 0;
     SecurePrint("For ending program press enter\n");
     scanf("%c",&c);
     finishRW = TRUE;
     for (int i = 0; i < threadsNumber; i++)
     {
       WaitForSingleObject(threads[i],INFINITE);
       CloseHandle(threads[i]);
     }
     DeInitializeRWContext();
}

void ThreadsRace(int argc,char* argv[]){
     semCounter = CreateSemaphoreA(NULL,1,1,"globalCounter");
     semOutput = CreateSemaphoreA(NULL,1,1,"output");
     DWORD threadID = 0;
     HANDLE thread1 = CreateThread(0,0,ThreadIncrementCounter_1,0,0,&threadID);
     SecurePrint("Thread 1 (%d) created\n",threadID);
     HANDLE thread2 = CreateThread(0,0,ThreadIncrementCounter_2,(LPVOID)&thread1,0,&threadID);
     SecurePrint("Thread 2 (%d) created\n",threadID);
     HANDLE thread3 = CreateThread(0,0,ThreadIncrementCounter_3,(LPVOID)&thread1,0,&threadID);
     SecurePrint("Thread 3 (%d) created\n",threadID);
     HANDLE threads[] = {thread1,thread2,thread3};
     WaitForMultipleObjects(3,threads,TRUE,INFINITE);
     CloseHandle(semCounter);
     CloseHandle(semOutput);
}
void print_options()
{
   char *options[] = {"1.Create producers and consumers [number of producers] [number of consumers]", 
                      "2.Create readers and writers [number of writers] [number of readers] [path to file] [[opt] data for write in file]", 
                      "3.Create threads"
                       };
   int options_num = sizeof(options) / sizeof(options[0]);
   for (int i = 0; i < options_num; i++)
   {
      printf("%s\n", options[i]);
   }
}