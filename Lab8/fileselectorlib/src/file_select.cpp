#include "../include/file_select.h"
#define BUFSIZE_1_FILE 1024
#define BUFSIZE_FILES 2048
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <commdlg.h>
char *select_one_file(const char* filter)
{
    char *buf = (char *)calloc(BUFSIZE_1_FILE, 1);
    if (!buf)
    {
        fprintf(stderr, "Cannot allocate memory for buf\n");
        return NULL;
    }
    OPENFILENAMEA ofn = {0};
    ofn.lpstrFilter = filter;
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = buf;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER;
    if (!GetOpenFileNameA(&ofn))
    {
        fprintf(stderr, "Error during selecting file %d", GetLastError());
        free(buf);
        return NULL;
    }
    return buf;
}
char **select_multiple_files(int *paths_number, const char* filter)
{
    char **res = NULL;
    OPENFILENAME ofn = {0};
    char szFile[MAX_PATH + 1] = {0};
    char szDirect[MAX_PATH + 1] = {0};
    int fCount = 0;
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szDirect;
    *(ofn.lpstrFile) = 0;
    ofn.nMaxFile = MAX_PATH;

    // file type filter, \0 - null character (end of string)
    //ofn.lpstrFilter = "All Files\0*.*\0";
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = szFile;
    *(ofn.lpstrFile) = 0;
    ofn.nMaxFileTitle = sizeof(szFile);
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;

    // show dialog Open
    if (GetOpenFileNameA(&ofn))
    {
        printf("files selected\n");
        char *ptr = ofn.lpstrFile;
        ptr[ofn.nFileOffset - 1] = 0;

        // Directory path:
        char *dir = ptr;

        ptr += ofn.nFileOffset;
        char *files = ptr;
        while (*ptr)
        {
            fCount++;
            // printf("File: %i %s\n", fCount, ptr);
            ptr += (lstrlen(ptr) + 1);
        }
        res = (char **)calloc(fCount, sizeof(char *));
        ptr = files;
        for (int i = 0; i < fCount; i++)
        {
            res[i] = (char *)calloc(strlen(dir) + 1 + strlen(ptr) + 1, 1);
            strcat(res[i], dir);
            strcat(res[i], "\\");
            strcat(res[i], ptr);
            ptr += (lstrlen(ptr) + 1);
        }

        printf("selected %d files\n", fCount);
    }
    else
    {
        printf("no files selected\n");
    }
    *paths_number = fCount;
    return res;
}