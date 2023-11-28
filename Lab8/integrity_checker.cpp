#include <stdio.h>
#include <stdlib.h>
#include "integritylistlib/include/integritylist.h"
#include "cryptolib/include/integrity.h"
#include "fileselectorlib/include/file_select.h"
#include <string.h>
#include <stdint.h>
int main(int argc, char *argv[])
{
        printf("Select json:\n");
        char* json_path = select_one_file("*.json");
        int paths_number = argc - 1;
        char** paths = (char**) calloc(paths_number, sizeof(char*));
        for (int i = 1; i < argc; i++)
        {
             paths[i - 1] = (char*)calloc(strlen(argv[i]) + 1, 1);
             strcpy(paths[i - 1], argv[i]);
        }
        LinkedList* list = create_integrity_list(paths, paths_number);
        for (int i = 0; i < argc; i++)
        {
            free(paths[i]);
        }
        free(paths);
        save_list_to_json(json_path, list);
        DeepErase(list, release_integrity_object);
    return 0;
}