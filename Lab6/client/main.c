#include <stdio.h>
#include "lib/include/client.h"
int main(int argc, char* argv[]){
    /*All clients initialy recv msgs 
    Server (when client is registered) sends to them respective msg and then he will be in state send*/
    StartClientContext("127.0.0.1","10102","bye");
    return 0;
}