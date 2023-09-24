#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
ssize_t getline(char **restrict buffer, size_t *restrict size,
                FILE *restrict fp) {
    register int c;
    register char *cs = NULL;

    if (cs == NULL) {
        register int length = 0;
        while ((c = getc(fp)) != EOF) {
            cs = (char *)realloc(cs, ++length+1);
            if ((*(cs + length - 1) = c) == '\n') {
                *(cs + length) = '\0';
                *buffer = cs;
                break;
            }
        }
        return (ssize_t)(*size = length);
    } else {
        while (--(*size) > 0 && (c = getc(fp)) != EOF) {
            if ((*cs++ = c) == '\n')
                break;
        }
        *cs = '\0';
    }
    return (ssize_t)(*size=strlen(*buffer));
}
void PrintLastError()
{
   char *desc = NULL;
   printf("UtilError: %d\n", GetLastError());
}
wchar_t *Char2Wchar(const char *str)
{
   wchar_t *wstr = NULL;
   int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
   wstr = (wchar_t *)calloc(size, sizeof(wchar_t));
   if (!wstr)
   {
      return NULL;
   }
   MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, size * sizeof(wchar_t));
   return wstr;
}
char *Wchar2Char(const wchar_t *wstr)
{
   char *str = NULL;
   int size = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
   str = (char *)calloc(size, sizeof(char));
   if (!str)
   {
      return NULL;
   }
   WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, size * sizeof(char), NULL, NULL);
   return str;
}

int alphabet_comparator(const void *a, const void *b)
{
   return (*((char *)a) - *((char *)b));
}
void remove_by_index(char *src, int idx)
{
   for (int i = idx; src[i]; i++)
   {
      src[i] = src[i + 1];
   }
}
int atoi_before_sep(const char* src, char sep){
   int res = 0;
   int negFlag = src[0] == '-' ? 1 : 0;
   for (int i = negFlag; src[i] && src[i] != sep; i++)
   {
        res = 10 * res + src[i] - '0';
   }
   if(negFlag){
      res = -res;
   }
   return res;
}
int int_comparator_decrease(const void *a, const void* b){
    return (*((int *)b) - *((int *)a));
    
}
int* convert_bytes_to_i(const char* bytes, int* size, char sep){
    const int QUOTA = 4;
    int N = 10;
    int* res = (int*) calloc(N, sizeof(int)), *ptr = NULL;
    int j = 0; //actual size of res
    char* prev = bytes;
    int len = strlen(bytes) + 1;
    for (int i = 0; i < len; i++)
    {
         if(bytes[i] == sep || !bytes[i] ){
            if(j == N){
               ptr = (int*)realloc(res, (N + QUOTA) * sizeof(int));
               if(!ptr){
                  free(res);
                  return NULL;
               }
               N += QUOTA;
            }
            res[j] = atoi_before_sep(prev, sep);
            j++;
            prev = bytes + i + 1;
         }
    }
    *size = j;
    return res;
}
