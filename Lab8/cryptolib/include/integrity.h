#ifndef _INTEGRITY_H
#define _INTEGRITY_H
#include <stdint.h>
#include <windows.h>
#define MD5LEN  16
#define _FILE_TYPE 0 //file type
#define _FOLDER_TYPE 1 //folder type
#define _REG_TYPE 3 //reg type
/**
 * @brief MD5 function for hashing files.
*/
uint8_t* md5_hash_file(const char* filename);
char* hash_to_hex_str(const uint8_t* hash, uint16_t hash_len);
/**
 * @brief MD5 function for hashing dirs.
*/
uint8_t* md5_hash_dir(const char* dir_path);
/**
 * @brief MD5 function for hashing reg branches.
*/
uint8_t* md5_hash_reg(HKEY key, const char* subkey);
/**
 * @brief Get crypt hash handle value.
 * @param handle_provider provider
 * @param id algorithm id
*/
HCRYPTHASH get_crypt_hash_handle(HCRYPTPROV* handle_provider, ALG_ID id);
/**
 * @brief MD5 function for hashing objects (files, dirs).
 * @param object_type object type
 * @param key hkey ptr (only if objetc_type is _REG_TYPE), can be null
*/
uint8_t* md5_hash_object(const char* path, int object_type, HKEY* key);
/**
 * @brief Get hash value from string.
 * @param hash_str given string with hash [in]
 * @param hash_len hash length [in]
*/
uint8_t* get_hash_value_from_str(const char* hash_str, int hash_len);
/**
 * @brief Get object type for path.
 * @param path path [in]
 * @param hkey  hkey if path is reg branch [out]
*/
int get_type_for_path(const char* path, HKEY* hkey);
#endif