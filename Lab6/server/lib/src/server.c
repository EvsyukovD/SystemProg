#include "server.h"
#include "msgservice.h"
#include <stdio.h>
void PrintLastError(){
    printf("Server error: %d\n",GetLastError());
}
void PrintLastWSAError(){
    printf("Server wsa error: %d\n", WSAGetLastError());
}
WINBOOL StartServerContext(const char* ip,const char* port, const char* signal_bye)
{
    int result = E_FAIL;
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
    MessageHandler(listen_socket, signal_bye);
    FinishServerContext(full_server,listen_socket);
    return TRUE;
}
void MessageHandler(SOCKET listen_socket, const char *signal_bye)
{
    const int len = 1024;
    char buffer[1024] = {0};
    char* ans = NULL;
    int result = 0;
    int c = 0;
    WINBOOL is_running = TRUE;
    while (is_running)
    {
        SOCKET client = INVALID_SOCKET;
        client = accept(listen_socket, NULL, NULL);
        if (client == INVALID_SOCKET)
        {
            printf("Failed to accept with code %d",WSAGetLastError());
            continue;
        }
        printf("New client connected\n");
        do
        {
            result = recv(client, buffer, len, 0);
            if(result > 0){
                if (strncmp(buffer, signal_bye, strlen(signal_bye)) == 0)
                {
                    result == 0;
                }
                else
                {
                    ans = answer(buffer);
                    if (ans)
                    {
                        if (send(client, ans, strlen(ans), 0) == SOCKET_ERROR)
                        {
                            result = 0;
                        }
                        free(ans);
                        ans = NULL;
                    }
                }
            } else if(result == 0){
                printf("Connection closing...\n");
            } else {
                printf("Failed to recv with code %d\n",WSAGetLastError());
            }
            memset(buffer, 0, sizeof(buffer));
        } while (result > 0);
        
        result = shutdown(client,SD_BOTH);
        if(result == SOCKET_ERROR){
            printf("Failed to shutdown with code %d\n",WSAGetLastError());
            is_running = FALSE;
        }
        closesocket(client);
        printf("Client is disconnected\n");
        printf("Finish work? Press Y to finish...\n");
        fflush(stdin);
        scanf("%c",&c);
        if(c == 'Y'){
            is_running = FALSE;
        }
        fflush(stdin);
    }
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