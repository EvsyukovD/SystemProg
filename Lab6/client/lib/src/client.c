#include "client.h"
#include "clientmsgservice.h"
#include <stdio.h>
const char* signal_bye = "bye";
WINBOOL endSessionFlag = FALSE;
CRITICAL_SECTION section = {0};
void PrintLastError(){
    printf("Client error: %d\n",GetLastError());
}
void PrintLastWSAError(){
    printf("Client wsa error: %d\n", WSAGetLastError());
}
WINBOOL StartClientContext(const char * ip,const char* port)
{
    int result = E_FAIL;
    WSADATA wsa = {0};
    SOCKET connect_socket = INVALID_SOCKET;
    ADDRINFOA client = {0};
    ADDRINFOA *full_client = NULL;

    result = WSAStartup(0x0202, &wsa);
    printf("WSAStartup returned %d\n", result);
    if (result != ERROR_SUCCESS)
    {
        PrintLastWSAError();
        FinishClientContext(full_client, connect_socket);
        return FALSE;
    }

    client.ai_family = AF_INET;
    client.ai_socktype = SOCK_STREAM;
    client.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo(ip, port, &client, &full_client);
    printf("GetAddrInfo returned %d\n", result);
    if (result != ERROR_SUCCESS)
    {
        PrintLastWSAError();
        FinishClientContext(full_client, connect_socket);
        return FALSE;
    }

    connect_socket = socket(full_client->ai_family, full_client->ai_socktype, full_client->ai_protocol);
    if (connect_socket == INVALID_SOCKET)
    {
        PrintLastWSAError();
        FinishClientContext(full_client, connect_socket);
        return FALSE;
    }

    printf("Create socket successfully \n");
    result = connect(connect_socket, full_client->ai_addr, full_client->ai_addrlen);
    if (result == SOCKET_ERROR)
    {
        PrintLastWSAError();
        FinishClientContext(full_client, connect_socket);
        return FALSE;
    }
    MessageHandler(connect_socket, signal_bye);
    FinishClientContext(full_client, connect_socket);
    return TRUE;
}
int SendMessageToServer(LPVOID args){
    WINBOOL isRunning = TRUE;
    char buffer[1024] = {0};
    int len = 1024;
    int cur_len = 0;
    int result = 0;
    LONG64* params = (LONG64*) args;
    HANDLE* h = params[1];
    SOCKET* s = params[0];
    SOCKET server_socket = *s;
    HANDLE receiverThread = *h;
    DWORD code = 0;
    DWORD exitCode = 0;
    while(isRunning){
        if (Dialog(buffer, len))
        {
            if (!strcmp(buffer, signal_bye))
            {
                isRunning = FALSE;
                GetExitCodeThread(receiverThread, &exitCode);
                if (exitCode == STILL_ACTIVE)
                {
                    TerminateThread(receiverThread, code);
                }
            }
            result = send(server_socket, buffer, len, 0);
            if (result == SOCKET_ERROR)
            {
                printf("Failed to send data with code %d\n", WSAGetLastError());
                isRunning = FALSE;
                GetExitCodeThread(receiverThread, &exitCode);
                if (exitCode == STILL_ACTIVE)
                {
                    TerminateThread(receiverThread, code);
                }
            }
            memset(buffer, 0, len);
        }
    }
    return 0;
}
int GetMessageFromServer(LPVOID pserv){
    WINBOOL isRunning = TRUE;
    char buffer[1024] = {0};
    int len = 1024;
    int result = 0;
    char* ptr = NULL;
    char* fileName = NULL;
    char sep = ':';
    char file_id = 'f';
    SOCKET server_socket = *((SOCKET*) pserv);
    while(isRunning){
        result = recv(server_socket, buffer, len, 0);
        if(result > 0){
            ptr = strchr(buffer, sep);
            if(ptr &&  ptr[1] == file_id && ptr[2] == sep){//buffer: id:data or id:file_id:...
               ProcessFilePartFromServer(&ptr[3]);
            }else{
               printf("From server: client %s\n",buffer);
            }

        } else if(result == 0){
            printf("Server close connection\n");
            isRunning = FALSE;
        } else {
            printf("Failed to recv with code %d\n", WSAGetLastError());
            isRunning = FALSE;
        }
        memset(buffer,0, len);
    }
}
void MessageHandler(SOCKET connect_socket, const char *signal_bye)
{
    HANDLE threads[2];
    int threadsNumber = 2;
    DWORD threadId = 0;
    threads[0] = CreateThread(NULL,0,GetMessageFromServer,&connect_socket,0,&threadId);
    LONG64* args[2] = {&connect_socket, &threads[0]};
    threads[1] = CreateThread(NULL,0,SendMessageToServer,args,0,&threadId);
    WaitForMultipleObjects(threadsNumber,threads,TRUE,INFINITE);
    for (int i = 0; i < threadsNumber; i++)
    {
        CloseHandle(threads[i]);
    }
    printf("Finish client api\n");
}
void FinishClientContext(ADDRINFOA *full_server, SOCKET listen_socket)
{
    if (full_server)
    {
        freeaddrinfo(full_server);
    }
    if (listen_socket != INVALID_SOCKET)
    {
        closesocket(listen_socket);
    }
}
void StartClientContextArgs(int argc, char* argv[]){
     StartClientContext(argv[2], argv[3]);
}
void print_options()
{
   char *options[] = {"1.Start client [ip] [port]", 
                      "2.Start client pipe [pipename_to_connect]" 
                       };
   int options_num = sizeof(options) / sizeof(options[0]);
   for (int i = 0; i < options_num; i++)
   {
      printf("%s\n", options[i]);
   }
}