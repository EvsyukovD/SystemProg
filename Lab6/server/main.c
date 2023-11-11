#include <stdio.h>
#include "lib/include/server.h"
int main(int argc, char* argv[]){
    StartServerContext("localhost","10102");
    return 0;
}