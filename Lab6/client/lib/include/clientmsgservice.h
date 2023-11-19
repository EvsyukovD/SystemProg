#ifndef CLIENTMSGSERVICE_H
#define CLIENTMSGSERVICE_H
#include <Windows.h>
void ProcessFilePartFromServer(const char* buffer, int buf_len);
WINBOOL ReadFilePartForServer(char* destBuffer, 
                              int destBufferSize, 
                              int destId, 
                              const char* fileName, 
                              LPDWORD writtenBytes,
                              DWORD skipBytes,
                              WINBOOL* isEof, WINBOOL isFirstPart);
WINBOOL Dialog(char* buffer, int bufferSize);
DWORD GetNumberFromBufferBeforeSep(const char* s, char sep);
DWORD GetPartSizeFromBuffer(const char*s);
void fill_written_bytes_str(char* dest_str, int dest_str_len, char filler);
WINBOOL encode_to_base64(char* bytes, DWORD len, DWORD* enc_size);
WINBOOL decode_base64(char* bytes, DWORD len, DWORD* dec_len);
void ExtractFileName(char* destBuffer, const char* buffer);
#endif