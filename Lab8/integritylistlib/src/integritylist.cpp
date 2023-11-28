#include "../include/integritylist.h"
#include "../../cryptolib/include/integrity.h"
#include <fstream>
#include <string.h>
#include <iostream>
#include <cstring>
using json = nlohmann::json;
integrity_object_struct *create_empty_integrity_object()
{
     integrity_object_struct *res = (integrity_object_struct *)calloc(1, sizeof(integrity_object_struct));
     if (!res)
     {
          fprintf(stderr, "Cannot allocate memory for creating integrity object\n");
     }
     return res;
}
void release_integrity_object(void *o)
{
     if (o)
     {
          integrity_object_struct *ob = (integrity_object_struct *)o;
          free(ob->hash);
          free(ob->hash_alg_name);
          free(ob->path);
     }
}
void init_integrity_object(integrity_object_struct *ob,
 const char *path,
 const uint8_t *hash,
 uint16_t hash_len,
 const char *hash_alg_name,
 int type)
{
     ob->hash = (uint8_t *)calloc(hash_len, 1);
     memcpy(ob->hash, hash, hash_len);
     ob->hash_alg_name = (char *)calloc(strlen(hash_alg_name) + 1, 1);
     strcpy(ob->hash_alg_name, hash_alg_name);
     ob->hash_len = hash_len;
     ob->path = (char *)calloc(strlen(path) + 1, 1);
     strcpy(ob->path, path);
     ob->type = type;
}
char *struct_to_string(integrity_object_struct st)
{
     int hash_len = st.hash_len;
     int path_len = strlen(st.path);
     int alg_name_len = strlen(st.hash_alg_name);
     const char *temp = "{\"hash\":,\"alg\":,\"hash_length\":,\"path\":}";
     int temp_len = strlen(temp);
     char *res = (char *)calloc(temp_len + hash_len + path_len + alg_name_len + 1, 1); // 1 for '\0'
     if (!res)
     {
          return NULL;
     }
     sprintf(res, "[%d,%s,%d,%s]", st.hash, st.hash_alg_name, st.hash_len, st.path);
     return res;
}
void struct_to_json(const integrity_object_struct &st, json &struct_json)
{
     char *hex_hash_str = hash_to_hex_str(st.hash, st.hash_len);
     std::string hash = std::string(hex_hash_str),
                 alg_name = std::string(st.hash_alg_name),
                 path = std::string(st.path);
     int type = st.type;
     struct_json = {{"hash", hash}, {"algorithm_name", alg_name}, {"hash_len", st.hash_len}, {"path", path}, {"type", type}};
     free(hex_hash_str);
     /*struct_json["hash"] = hash;
      struct_json["algorithm_name"] = std::string(st.hash_alg_name);
      struct_json["hash_len"] = st.hash_len;
      struct_json["path"] = std::string(st.path);*/
}
BOOL save_list_to_json(const char *path_to_json, const LinkedList *list)
{
     json j, struct_json;
     std::ofstream output(path_to_json);
     int size = list->size;
     j["hashes"] = json::array();
     integrity_object_struct *st = NULL;
     char *str_struct = NULL;
     for (int i = 0; i < size; i++)
     {
          st = (integrity_object_struct *)FindByIndex(list, i)->data;
          if (st)
          {
               struct_to_json(*st, struct_json);
               std::cout << struct_json.dump() << std::endl;
               j["hashes"].push_back(struct_json);
          }
     }
     output << j;
     output.close();
     return TRUE;
}
LinkedList *read_list_from_json(const char *path_to_json)
{
     json j;
     std::ifstream input(path_to_json);
     LinkedList *res = CreateList();
     if (!res)
     {

          return NULL;
     }
     integrity_object_struct *st = NULL;
     input >> j;
     input.close();
     uint8_t* hash = NULL;
     int hash_len = 0, type;
     std::string path, hash_str, alg_name;
     for (auto it = j["hashes"].begin(); it != j["hashes"].end(); it++)
     {
          auto json_object = *it;
          st = create_empty_integrity_object();
          path = json_object["path"];
          hash_str = json_object["hash"];
          alg_name = json_object["algorithm_name"];
          hash_len = json_object["hash_len"];
          type = json_object["type"];
          hash = get_hash_value_from_str(hash_str.c_str(), hash_len);
          init_integrity_object(st, path.c_str(),
                                hash,
                                hash_len,
                                alg_name.c_str(),
                                type);
          
          Add(res, st);
     }
     return res;
}
integrity_object_struct *create_integrity_struct_for_path(const char *path)
{
     integrity_object_struct *st = create_empty_integrity_object();
     if (!st)
     {
          return NULL;
     }
     HKEY hkey;
     int type = get_type_for_path(path, &hkey);
     uint8_t *hash = md5_hash_object(path, type, &hkey);
     if (!hash)
     {
          free(st);
          return NULL;
     }
     init_integrity_object(st, path, hash, (uint16_t)MD5LEN, "MD5", type);
     return st;
}
LinkedList *create_integrity_list(char *paths[], int path_number)
{
     if (path_number <= 0)
     {
          return NULL;
     }
     LinkedList *res = CreateList();
     if (!res)
     {
          fprintf(stderr, "Cannot allocate memory for creating integrity list\n");
          return NULL;
     }
     integrity_object_struct *st = NULL;
     for (int i = 0; i < path_number; i++)
     {
          fprintf(stdout, "Processing path:%s\n", paths[i]);
          st = create_integrity_struct_for_path(paths[i]);
          Add(res, st);
     }
     return res;
}