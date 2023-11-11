#ifndef CLIENT_H
#define CLIENT_H
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
void PrintLastError();
void PrintLastWSAError();
WINBOOL StartClientContext(const char *ip,const char* port, const char* bye_signal);
void MessageHandler(SOCKET listen_socket,const char* signal_bye);
int SendMessageToServer(LPVOID);
int GetMessageFromServer(LPVOID);
void ProcessFilePartFromServer(const char* buffer);
WINBOOL ReadFilePartForServer(char* destBuffer,int destBufferSize ,int destId, const char* fileName, LPDWORD writtenBytes,DWORD skipBytes);
void FinishClientContext(ADDRINFOA* full_server,SOCKET listen_socket);
#endif