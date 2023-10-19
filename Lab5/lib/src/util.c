#include "../include/util.h"
#include "../include/common.h"
#include <stdlib.h>
#include <stdio.h>
#define MAX_LIST_SIZE 5
#define MAX_THREADS 20

CRITICAL_SECTION bufferLock = {0};
CONDITION_VARIABLE bufferNotEmpty;
CONDITION_VARIABLE bufferNotFull;
WINBOOL finishProduceAndConsume = 0;
LinkedList* globalList = NULL;

HANDLE hFile = NULL;
HANDLE semOutput = NULL;
HANDLE semRCounter = NULL;
HANDLE semFile = NULL;
int readersCount = 0;//for readers function realization

int readersNumber = 0;// number of threads - readers
int writersNumber = 0;
WINBOOL finishRW = FALSE;

HANDLE semCounter = NULL;
int counter = 0;

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
int InitRWContext(int argc,char* argv[]){
        if(argc <= 3){
           printf("Not enough args\n");
           return FALSE;
        }
        writersNumber = atoi(argv[1]); 
        readersNumber = atoi(argv[2]);
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
        // init readers events
        /*globalReadEvent = CreateEvent(NULL,  // default security
                                      FALSE, // auto reset
                                      FALSE,  // default state signaled
                                      NULL);
        if (!globalReadEvent)
        {
              PrintLastError();
              return FALSE;
        }
        globalWriteEvent = CreateEvent(NULL,  // default security
                                      FALSE, // auto reset
                                      FALSE,  // default state signaled
                                      NULL);*/
        hFile = UtilGetFileHandle(argv[3]);
        if(!hFile || hFile == INVALID_HANDLE_VALUE){
           PrintLastError();
           return FALSE;
        }
        return TRUE;
}
void DeInitializeRWContext()
{
     CloseHandle(semFile);
     CloseHandle(semRCounter);
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

void ThreadIncrementCounter_1(LPVOID){

}
void ThreadIncrementCounter_2(LPVOID){}
void ThreadIncrementCounter_3(LPVOID){}

void ProducerAndConsumer(int argc, char* argv[]){
     if(argc > 2){
        int prodNumber = atoi(argv[1]), consNumber = atoi(argv[2]);
        if(prodNumber + consNumber > MAX_THREADS){
           printf("Too many threads. The number of threads should be not more than %d\n",MAX_THREADS);
           return;
        }
        InitGlobalList();
        InitializeConditionVariable(&bufferNotEmpty);
        InitializeConditionVariable(&bufferNotFull);
        InitializeCriticalSection(&bufferLock);
        int millis = 5000;
        if(argc > 3){
            millis = atoi(argv[3]);
        }
        DWORD dwNewThreadID;
        HANDLE threads[MAX_THREADS];
        int NumThreads = prodNumber + consNumber;
        int (*fptr)(int) = NULL;
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
              threads[i] = CreateThread(NULL,0,WriteToFile,argc > 4 ? argv[4] : data,0,&threadId);
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