#ifndef CLIENTMSGSERVICE_H
#define CLIENTMSGSERVICE_H
#include <Windows.h>
void ProcessFilePartFromServer(const char* buffer);
WINBOOL ReadFilePartForServer(char* destBuffer, 
                              int destBufferSize, 
                              int destId, 
                              const char* fileName, 
                              LPDWORD writtenBytes,
                              DWORD skipBytes,
                              WINBOOL* isEof);
WINBOOL Dialog(char* buffer, int bufferSize);
DWORD GetNumberFromBufferBeforeSep(const char* s, char sep);
void ExtractFileName(char* destBuffer, const char* buffer);
#endif