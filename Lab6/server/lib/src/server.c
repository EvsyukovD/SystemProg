#include "server.h"
#include "msgservice.h"
#include <stdio.h>
#include <stdlib.h>
const char* signal_bye = "bye";
Client clients[2];
int clientsNumber = 2;
CRITICAL_SECTION section = {0};
CONDITION_VARIABLE bufferNotEmpty;
SOCKET listen_socket = {0};
HANDLE threads[4];
int threadsNumber = 4;
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

Message* CreateMessage(){
    return (Message*)calloc(1, sizeof(Message));
}
void ReleaseMessage(void* pData){
   if(pData){
      free(pData);
   }
}
int GetNumberFromBufferBeforeSep(const char* s, const char* sep){
   int result = 0;
   for (int i = 0; s[i] && s[i] != sep[0]; i++)
   {
        result = 10 * result + s[i] - '0';
   }
   return result;
}
WINBOOL CheckClientId(int id){
    for (int i = 0; i < clientsNumber; i++)
    {
        if(clients[i].id == id){
           return TRUE;       
        }
    }
    return FALSE;
}
int GetClientId(SOCKET client){
    char ip[16] = {0};
    int port = 0;
    int id = -1;
    GetIPAndPort(client,ip, &port);
    for (int i = 0; i < clientsNumber; i++)
    {
        if(clients[i].port == port && !strcmp(clients[i].ip,ip)){
           id = clients[i].id;
           return id;
        }
    }
    return -1;
}
void RemoveClientById(int id){
    for (int i = 0; i < clientsNumber; i++)
    {
        if(clients[i].id == id){
           memset(clients[i].ip, 0, sizeof(clients[i].ip));
           clients[i].port = 0;
           clients[i].id = -1;
           clients[i].client = INVALID_SOCKET;
        }
    }
}
void PrintLastError(){
    printf("Server error: %d\n",GetLastError());
}
void PrintLastWSAError(){
    printf("Server wsa error: %d\n", WSAGetLastError());
}
void GetIPAndPort(unsigned int socketFD,char *ip, int* port){
     struct sockaddr address;
     socklen_t addressLength = sizeof(address);
     struct sockaddr_in* addressInternet = NULL;
     getpeername(socketFD,&address,&addressLength);
     addressInternet = (struct sockaddr_in*)&address;
     strcpy(ip,inet_ntoa(addressInternet->sin_addr));
     *port = ntohs(addressInternet->sin_port);
}
SOCKET GetSocketById(int id){
     for (int i = 0; i < clientsNumber; i++)
     {
        if (clients[i].id == id)
        {
           return clients[i].client;
        }
     }
     return INVALID_SOCKET;
}
Message* InitByeMessage(SOCKET client){
    Message* msg = CreateMessage();
    msg->src_id = GetClientId(client);
    msg->dest_id = msg->src_id;
    msg->msg_type = SERVER_END_SESSION_MSG_TYPE;
    strcpy(msg->buffer,signal_bye);
    return msg;
}
Message* InitMessageFromClient(SOCKET client,const char* buffer){
    Message *msg = NULL;
    const char* sep = ":";
    int radix = 10;
    int start = 0;
    if(!strcmp(buffer,signal_bye)){
       return InitByeMessage(client);
    } else {
        int dest_id = GetNumberFromBufferBeforeSep(buffer,sep);
        if(CheckClientId(dest_id)){
            int src_id = GetClientId(client);
            if(src_id == dest_id){
                return NULL;// do not send msgs back to client
            }
            msg = CreateMessage();
            msg->dest_id = dest_id;
            msg->src_id = src_id;
            itoa(msg->src_id, msg->buffer, radix);
            start = strlen(msg->buffer);
            strcpy(msg->buffer + start, strstr(buffer,sep));
            msg->msg_type = SERVER_DEFAULT_MSG_TYPE;
            return msg;
        } else {
            printf("Can't find user with dest id = %d\n", dest_id);
            msg = CreateMessage();
            msg->src_id = GetClientId(client);
            itoa(msg->src_id, msg->buffer, radix);
            sprintf(msg->buffer,"Can't find user with dest id = %d\n", dest_id);
            msg->dest_id = msg->src_id;
            return msg;
        }
    }
}
WINBOOL StartServerContext(const char* ip,const char* port)
{
    int result = E_FAIL, result2 = E_FAIL;
    WSADATA wsa = {0};
    SOCKET listen_socket = INVALID_SOCKET;
    ADDRINFOA server = {0};
    ADDRINFOA *full_server = NULL;

    result = WSAStartup(0x0202, &wsa);
    printf("WSAStartup returned %d\n", result);
    if (result != ERROR_SUCCESS)
    {
        PrintLastError();
        FinishServerContext(full_server, listen_socket);
        return FALSE;
    }

    server.ai_family = AF_INET;
    server.ai_socktype = SOCK_STREAM;
    server.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo(ip, port, &server, &full_server);
    printf("GetAddrInfo returned %d\n", result);
    if (result != ERROR_SUCCESS)
    {
        PrintLastError();
        FinishServerContext(full_server, listen_socket);
        return FALSE;
    }

    listen_socket = socket(full_server->ai_family, full_server->ai_socktype, full_server->ai_protocol);
    if (listen_socket == INVALID_SOCKET)
    {
        PrintLastWSAError();
        FinishServerContext(full_server, listen_socket);
        return FALSE;
    }

    printf("Create socket successfully \n");
    result = bind(listen_socket, full_server->ai_addr, full_server->ai_addrlen);
    if (result == SOCKET_ERROR)
    {
        PrintLastWSAError();
        FinishServerContext(full_server, listen_socket);
        return FALSE;
    }

    result = listen(listen_socket, SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
        PrintLastWSAError();
        FinishServerContext(full_server, listen_socket);
        return FALSE;
    }
    MessageHandler(listen_socket);// create 2 threads
    FinishServerContext(full_server,listen_socket);
    return TRUE;
}
void RegisterClients(SOCKET listen_socket, Client* clients, int clientsNumber){
    WINBOOL finishRegistration = FALSE;
    int i = 0;
    int result = 0;
    char buffer[50] = {0};
    int len = 50;
    while (!finishRegistration)
    {
        clients[i].client = accept(listen_socket, NULL, NULL);
        if (clients[i].client == INVALID_SOCKET)
        {
            printf("Failed to accept with code %d", WSAGetLastError());
            continue;
        }
        clients[i].id = i;
        GetIPAndPort(clients[i].client,clients[i].ip,&clients[i].port);
        if(clientsNumber - 1 == i){
            finishRegistration = TRUE;
        }
        sprintf(buffer,"Your id = %d, bye signal = '%s'",clients[i].id, signal_bye);
        result = send(clients[i].client, buffer, len, 0);
        if(result == SOCKET_ERROR){
           printf("Failed to send data with code %d\n", WSAGetLastError());
        }
        i++;
    }
}
int GetMessageFromClient(LPVOID cl){
    WINBOOL isRunning = TRUE;
    int result = 0;
    char buffer[1024] = {0};
    int len = 1024;
    int millis = 1000;
    SOCKET client = *((SOCKET*) cl);
    Message *msg = NULL;
    while (isRunning)
    {
        Sleep(millis);
        result = recv(client, buffer, len, 0);
        if (result > 0)
        {
            msg = InitMessageFromClient(client,buffer);
        }
        else if (result == 0)
        {
            printf("Client: close connection...\n");
            isRunning = FALSE;
        }
        else
        {
            printf("Failed to recv with code %d\n", WSAGetLastError());
            isRunning = FALSE;
        }
        if (msg)
        {
            isRunning = msg->msg_type == SERVER_END_SESSION_MSG_TYPE ? FALSE : TRUE;
            EnterCriticalSection(&section);
            //WaitForSingleObject(semGlobalList, INFINITE);
            Add(GetGlobalList(), msg);
            msg = NULL;
            LeaveCriticalSection(&section);
            //ReleaseSemaphore(semGlobalList, 1, NULL);
            WakeAllConditionVariable(&bufferNotEmpty);
        }
        memset(buffer, 0, len);
    }
    return 0;
}
int SendMessageToClient(LPVOID cl){
    WINBOOL isRunning = TRUE;
    int result = 0;
    char buffer[1024] = {0};
    int len = 1024;
    int millis = 1000;
    Message *msg = NULL;
    SOCKET client = *((SOCKET*)cl);
    SOCKET destClient = INVALID_SOCKET;
    while (isRunning)
    {
        EnterCriticalSection(&section);
        //WaitForSingleObject(semGlobalList, INFINITE);
        while (GetGlobalList()->size == 0)
        {
            SleepConditionVariableCS(&bufferNotEmpty, &section, INFINITE);
        }
        msg = GetDataByIndex(GetGlobalList(), 0);
        destClient = GetSocketById(msg->dest_id);
        if (destClient == client)
        {
            RemoveByIndex(GetGlobalList(), 0);
            if (msg->dest_id == msg->src_id && msg->msg_type == SERVER_END_SESSION_MSG_TYPE)
            {
                RemoveClientById(msg->src_id);
                isRunning = FALSE;
            }
            else
            {
                result = send(destClient, msg->buffer, strlen(msg->buffer), 0);
                if (result == SOCKET_ERROR)
                {
                    printf("Failed to send data with code %d\n", WSAGetLastError());
                }
            }
            ReleaseMessage(msg);
        }
        //ReleaseSemaphore(semGlobalList, 1, NULL);
        LeaveCriticalSection(&section);
        Sleep(millis);
    }
    result = shutdown(client, SD_BOTH);
    if (result == SOCKET_ERROR)
    {
       printf("Failed to shutdown with code %d\n",WSAGetLastError());
    }
    closesocket(client);
    return 0;
}
void MessageHandler(SOCKET listen_socket)
{
    DWORD threadId = 0;
    WINBOOL is_running = TRUE;
    RegisterClients(listen_socket,clients,clientsNumber);
    InitGlobalList();
    InitializeCriticalSection(&section);
    for (int i = 0; i < clientsNumber; i++)
    {
        threads[i] = CreateThread(NULL, 0, GetMessageFromClient, &clients[i], 0, &threadId);
    }
    for (int i = clientsNumber; i < threadsNumber; i++)
    {
        threads[i] = CreateThread(NULL, 0, SendMessageToClient, &clients[i - clientsNumber], 0, &threadId);
    }
    WaitForMultipleObjects(threadsNumber, threads, TRUE, INFINITE);
    for (int i = 0; i < threadsNumber; i++)
    {
        CloseHandle(threads[i]);
    }
    DeleteCriticalSection(&section);
    EraseGlobalList(ReleaseMessage);
}
void FinishServerContext(ADDRINFOA *full_server, SOCKET listen_socket)
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