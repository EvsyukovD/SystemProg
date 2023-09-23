#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "util.h"
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