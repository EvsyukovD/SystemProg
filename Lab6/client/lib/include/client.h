#ifndef CLIENT_H
#define CLIENT_H
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
WINBOOL StartClientContext(const char *ip,const char* port);
void MessageHandler(SOCKET listen_socket,const char* signal_bye);
int SendMessageToServer(LPVOID);
int GetMessageFromServer(LPVOID);
void StartClientContextArgs(int argc, char* argv[]);
void StartClientPipeContextArgs(int argc, char* argv[]);
int StartClientPipeContext(int argc, char* argv[]);
void PrintLastError();
int recv_until_sep(char sep,SOCKET, char* buf, int len, int flags);
void PrintLastWSAError();
void print_options();
void FinishClientContext(ADDRINFOA* full_server,SOCKET listen_socket);
#endif