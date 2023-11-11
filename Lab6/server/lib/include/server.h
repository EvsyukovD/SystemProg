#ifndef SERVER_H
#define SERVER_H
#include <Winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include "list.h"
#define SERVER_DEFAULT_MSG_TYPE 0
#define SERVER_END_SESSION_MSG_TYPE 1
typedef struct Client{
    SOCKET client;
    int id;
    char ip[16];
    int port;
}Client;
typedef struct Message{
    int src_id;
    int dest_id;
    int msg_type;
    char buffer[1024];
}Message;
extern LinkedList* globalList;
LinkedList* GetGlobalList();
LinkedList* InitGlobalList();
void EraseGlobalList(void (*ReleaseData)(void *));
void PrintLastError();
void PrintLastWSAError();
void GetIPAndPort(unsigned int socketFD,char *ip, int* port);
void RegisterClients(SOCKET listen_socket, Client* clients, int clientsNumber);
void RemoveClientById(int id);
int GetNumberFromBufferBeforeSep(const char* s, const char* sep);
WINBOOL CheckClientId(int id);
int GetClientId(SOCKET client);
SOCKET GetSocketById(int id);
Message* InitByeMessage(SOCKET client);
Message* InitMessageFromClient(SOCKET client,const char* buffer);
int GetMessageFromClient(LPVOID client);//add data to global list
int SendMessageToClient(LPVOID client);//take data from global list
WINBOOL StartServerContext(const char* ip,const char* port1);
void MessageHandler(SOCKET listen_socket);
void FinishServerContext(ADDRINFOA* full_server,SOCKET listen_socket);
#endif