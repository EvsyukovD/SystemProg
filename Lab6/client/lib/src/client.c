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
void ProcessFilePartFromServer(const char* buf){
    // buf: fileName:data
    HANDLE file = INVALID_HANDLE_VALUE;
    const char sep = ':';
    char *ptr = strchr(buf, sep);
    if(!ptr){//invalid file partition
        return;
    }
    int size = ptr - buf; // before ':'
    char name[20] = {0};
    name[0] = '.';
    name[1] = '/';
    strncpy(&name[2], buf, size);
    file = CreateFileA(name,
                       FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                       FILE_SHARE_WRITE | FILE_SHARE_READ,
                       NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        file = CreateFileA(name,
                           FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                           FILE_SHARE_WRITE | FILE_SHARE_READ,
                           NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    }
    if (file == INVALID_HANDLE_VALUE)
    {
        PrintLastError();
        return;
    }
    char* data = &ptr[1];
    long unsigned int written_bytes = 0;
    int len = strlen(data);
    WINBOOL res = WriteFile(file, data, len, &written_bytes, NULL);
    if (!res)
    {
        CloseHandle(file);
        PrintLastError();
        return;
    }
    CloseHandle(file);
}
WINBOOL ReadFilePartForServer(char *destBuffer, 
                              int destBufferSize, 
                              int destId, 
                              const char *fileName, 
                              LPDWORD writtenBytes, 
                              DWORD skipBytes)
{
    int radix = 10;
    const char* sep = ":"; 
    HANDLE file = CreateFileA(fileName,
                              FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                              FILE_SHARE_WRITE | FILE_SHARE_READ,
                              NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        *writtenBytes = 0;
        return FALSE;
    }
    memset(destBuffer, 0, destBufferSize);
    itoa(destId, destBuffer, radix);
    strcat(destBuffer, sep);
    strcat(destBuffer, fileName);
    strcat(destBuffer, sep);
    int i = strlen(destBuffer);
    SetFilePointer(file,skipBytes, NULL, FILE_BEGIN);
    if(!ReadFile(file, &destBuffer[i], destBufferSize - i,writtenBytes,NULL)){
        PrintLastError();
        CloseHandle(file);
        return FALSE;
    }
    CloseHandle(file);
    return TRUE; // we have read full file
}
WINBOOL StartClientContext(const char * ip,const char* port, const char* signal_bye)
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
        PrintLastError();
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
        fflush(stdin);
        fgets(buffer, len, stdin);
        cur_len = strlen(buffer);
        if(cur_len > 0){
           buffer[cur_len - 1] = 0;
        }
        if (!strcmp(buffer,signal_bye))
        {
            isRunning = FALSE;
            GetExitCodeThread(receiverThread,&exitCode);
            if(exitCode == STILL_ACTIVE){
               TerminateThread(receiverThread, code);
            }
        }
        result = send(server_socket, buffer, len, 0);
        if (result == SOCKET_ERROR)
        {
            printf("Failed to send data with code %d\n", WSAGetLastError());
            isRunning = FALSE;
            GetExitCodeThread(receiverThread,&exitCode);
            if(exitCode == STILL_ACTIVE){
               TerminateThread(receiverThread, code);
            }
        }
        memset(buffer, 0, len);
    }
    return 0;
}
int GetMessageFromServer(LPVOID pserv){
    WINBOOL isRunning = TRUE;
    char buffer[1024] = {0};
    int len = 1024;
    int result = 0;
    char* ptr = NULL;
    char file_id = 'f';
    char* fileName = NULL;
    char sep = ':';
    SOCKET server_socket = *((SOCKET*) pserv);
    while(isRunning){
        result = recv(server_socket,buffer,len,0);
        if(result > 0){
           // ptr = strchr(buffer, sep);
           // if(ptr && ptr[1] == file_id && ptr[2] == sep){//buffer: id:data or id:file_id:...
           //    ProcessFilePartFromServer(&ptr[3]);
           // }else{
               printf("From server %s\n",buffer);
          //  }

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