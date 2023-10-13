#include "../include/util.h"
#include <stdlib.h>
LinkedList* GetGlobalList(){
    return globalList;
}
Data* CreateData(){
    return (Data*)calloc(1, sizeof(Data));
}
void ReleaseData(void* pData){
   if(pData){
      free(pData);
   }
}
void AddDataToGlobalList(){

}
Data* TakeDataFromGlobalList(){
    return NULL;
}