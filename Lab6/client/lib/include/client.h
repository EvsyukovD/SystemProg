#ifndef CLIENT_H
#define CLIENT_H
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
void PrintLastError();
void PrintLastWSAError();
WINBOOL StartClientContext(const char *ip,const char* port, const char* bye_signal);
void MessageHandler(SOCKET listen_socket,const char* signal_bye);
void FinishClientContext(ADDRINFOA* full_server,SOCKET listen_socket);
#endif