#include "lib/include/list.h"
int main(int argc, char* argv[]){
    int N = 4;
    int a[4] = {1,2,3,4};
    LinkedList* list = CreateList();
    for (int i = 0; i < N; i++)
    {
        Add(list,&a[i]);
    }
    int* ptr = NULL;
    for (int i = 0; i < N; i++)
    {
        ptr = (int*)FindByIndex(list,i)->data;
        printf("list[%d] = %d\n",i, *ptr);
    }
    Erase(list);
    return 0;
}