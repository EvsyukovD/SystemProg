#include "msgservice.h"
#include <stdlib.h>
#include <stdio.h>
char* answer(const char* msg){
     const int ANSWER_SIZE = 20;
     char *ans = calloc(ANSWER_SIZE,sizeof(char));
     if(!ans){
        return NULL;
     }
     fflush(stdin);
     printf("Write answer for message: %s",msg);
     fgets(ans,ANSWER_SIZE, stdin);
     return ans;
}