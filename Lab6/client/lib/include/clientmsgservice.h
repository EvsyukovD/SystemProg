#ifndef CLIENTMSGSERVICE_H
#define CLIENTMSGSERVICE_H
#include <Windows.h>
char* answer(const char* msg);
void ProcessFilePartFromServer(const char* buffer);
WINBOOL ReadFilePartForServer(char* destBuffer, 
                              int destBufferSize, 
                              int destId, 
                              const char* fileName, 
                              LPDWORD writtenBytes,
                              DWORD skipBytes,
                              WINBOOL* isEof);
WINBOOL Dialog(char* buffer, int bufferSize);
void ExtractFileName(char* destBuffer, const char* buffer);
#endif