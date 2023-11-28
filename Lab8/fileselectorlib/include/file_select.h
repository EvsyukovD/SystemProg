#ifndef _FILE_SELECT_H
#define _FILE_SELECT_H
char* select_one_file(const char* filter);
char **select_multiple_files(int *paths_number, const char* filter);
#endif