#ifndef _INTEGRITY_LIST_H
#define _INTEGRITY_LIST_H
#include <stdint.h>
#include <windows.h>
#include "list.h"
#include "../../jsonlib/json.hpp"
/**
 * @brief Structure for checking integrity of an object.
*/
typedef struct integrity_object_struct{
    char* path;//path to object
    uint8_t* hash;//hash value
    uint16_t hash_len;//hash length
    char* hash_alg_name;//hash algortihm name
    int type;//object type - file, folder or reg
}integrity_object_struct;
/**
 * @brief Create empty object struct.
*/
integrity_object_struct* create_empty_integrity_object();
/**
 * @brief Erase integrity object.
*/
void release_integrity_object(void* ob);
/**
 * @brief Init integrity object.
 * @param ob integrity struct
 * @param path object path
 * @param hash hash value
 * @param hash_len hash length
 * @param hash_alg_name hash algorithm name
 * @param type object type (from cryptolib)
*/
void init_integrity_object(integrity_object_struct* ob,const char* path, const uint8_t* hash, uint16_t hash_len, const char* hash_alg_name, int type);
/**
 * @brief Save integrity list to json.
 * @param path_to_json json file path
 * @param list integrity list
*/
BOOL save_list_to_json(const char* path_to_json, const LinkedList* list);
/**
 * @brief Read list from json path.
 * @param path_to_json json file path
 * @returns list
*/
LinkedList* read_list_from_json(const char* path_to_json);
/**
 * @deprecated Bad func
 * @brief Convert integrity struct to string.
*/
char* struct_to_string(integrity_object_struct st);
/**
 * @brief Convert integrity struct to json.
 * @param st integrity struct
 * @param js target
*/
void struct_to_json(const integrity_object_struct& st, nlohmann::json& js);
/**
 * @brief Create integrity via paths array.
 * @param paths paths to objects
 * @param path_number number of paths
*/
LinkedList* create_integrity_list(char* paths[], int path_number);
/**
 * @brief Create integrity struct for object with given path.
 * @param path path to object
*/
integrity_object_struct* create_integrity_struct_for_path(const char* path);
#endif