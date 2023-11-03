#ifndef SERVER_H
#define SERVER_H
#include <Winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
void PrintLastError();
void PrintLastWSAError();
WINBOOL StartServerContext(const char* ip,const char* port, const char* bye_signal);
void MessageHandler(SOCKET listen_socket,const char* signal_bye);
void FinishServerContext(ADDRINFOA* full_server,SOCKET listen_socket);
#endif