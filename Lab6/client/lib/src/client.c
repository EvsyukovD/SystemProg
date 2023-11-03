#include "client.h"
#include "clientmsgservice.h"
#include <stdio.h>
void PrintLastError(){
    printf("Client error: %d\n",GetLastError());
}
void PrintLastWSAError(){
    printf("Client wsa error: %d\n", WSAGetLastError());
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
void MessageHandler(SOCKET connect_socket, const char *signal_bye)
{
    const int len = 1024;
    char buffer[1024] = {0};
    char* ans = NULL;
    int result = 0;
    char c = 0;
    char name[100];
    WINBOOL is_running = TRUE;
    printf("Write initial msg\n");
    fgets(buffer, len, stdin);
    while (is_running)
    {
        result = send(connect_socket, buffer, strlen(buffer), 0);
        if (result == SOCKET_ERROR)
        {
            printf("Failed to send data with code %d",WSAGetLastError());
            is_running = FALSE;
            continue;
        }
        memset(buffer, 0, len);
        result = recv(connect_socket, buffer, len, 0);
        if(result > 0){
            ans = answer(buffer);
            if(ans){
                if(send(connect_socket,ans,strlen(ans),0) == SOCKET_ERROR){
                   result = 0;
                }
                free(ans);
                ans = NULL;
            }
            if(strncmp(buffer, signal_bye, strlen(signal_bye)) == 0){
                printf("Server close connection\n");
                is_running = FALSE;
                result == ERROR_SUCCESS;
            }
        } else if(result == 0){
                  printf("Connection closing...\n");
        } else {
                  printf("Failed to recv with code %d\n",WSAGetLastError());
        }
        printf("Finish work? Press Y to finish...\n");
        fflush(stdin);
        scanf("%c",&c);
        if(c == 'Y'){
            is_running = FALSE;
        }
        fflush(stdin);
    }
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