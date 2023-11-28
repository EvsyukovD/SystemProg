#include "integrity.h"
#include <stdlib.h>
#include <stdio.h>
#include <wincrypt.h>
#include <fileapi.h>
#include <winreg.h>
#pragma comment(lib, "Advapi32.lib")
#define BUFSIZE 1024
#define HKCR "HKEY_CLASSES_ROOT"
#define HKCU "HKEY_CURRENT_USER"
#define HKLM "HKEY_LOCAL_MACHINE"
#define HKU "HKEY_USERS"
#define HKCC "HKEY_CURRENT_CONFIG"
wchar_t *Char2Wchar(const char *str)
{
    wchar_t *wstr = NULL;
    int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    wstr = (wchar_t *)calloc(size, sizeof(wchar_t));
    if (!wstr)
    {
        fprintf(stderr, "Cannot allocate memory for wide char str\n");
        return NULL;
    }
    MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, size * sizeof(wchar_t));
    return wstr;
}
uint8_t *get_hash_value_from_str(const char *hash_str, int hash_len)
{
    uint8_t *hash = NULL;
    if (hash_str)
    {
        hash = (uint8_t *)calloc(hash_len, 1);
        if (!hash)
        {
            fprintf(stderr, "Cannot allocate memory for transformation char[] -> uint8_t\n");
            return NULL;
        }
        int j = 0;
        for (int i = 0; i < hash_len; i++)
        {
            uint8_t high = isdigit(hash_str[j]) ? hash_str[j] - '0' : hash_str[j] - 'a' + 10;            // high 4 bits
            uint8_t low = isdigit(hash_str[j + 1]) ? hash_str[j + 1] - '0' : hash_str[j + 1] - 'a' + 10; // low 4 bits
            hash[i] = (high << 4) | low;
            j += 2;
        }
    }
    return hash;
}
int get_type_for_path(const char *path, HKEY *ptr_hkey)
{
    if (!strncmp(path, HKCU, strlen(HKCU)))
    {
        *ptr_hkey = HKEY_CURRENT_USER;
        return _REG_TYPE;
    }
    if (!strncmp(path, HKLM, strlen(HKLM)))
    {
        *ptr_hkey = HKEY_LOCAL_MACHINE;
        return _REG_TYPE;
    }
    if (!strncmp(path, HKCC, strlen(HKCC)))
    {
        *ptr_hkey = HKEY_CURRENT_CONFIG;
        return _REG_TYPE;
    }
    if (!strncmp(path, HKU, strlen(HKU)))
    {
        *ptr_hkey = HKEY_USERS;
        return _REG_TYPE;
    }
    DWORD attributes = GetFileAttributesA(path);
    if (attributes & FILE_ATTRIBUTE_DEVICE || attributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        return _FOLDER_TYPE;
    }
    else
    {
        return _FILE_TYPE;
    }
}
uint8_t *md5_hash_reg(HKEY reserved_key, const char *subkey)
{
    HKEY hkey = NULL;
    if (RegOpenKeyA(reserved_key, subkey, &hkey) != ERROR_SUCCESS)
    {
        return NULL;
    }
    char value_name[1024] = {0};
    DWORD value_name_size = 1023;
    char value[1024] = {0};
    DWORD index = 0;
    DWORD data_size = 1023;
    BOOL isFinish = FALSE;
    LSTATUS status;
    DWORD dw_status = 0;
    HCRYPTPROV handle_provider;
    DWORD hash_byte_length = MD5LEN;
    HCRYPTHASH handle_hash = get_crypt_hash_handle(&handle_provider, CALG_MD5);
    if (!handle_hash)
    {
        RegCloseKey(hkey);
        return NULL;
    }
    while (!isFinish)
    {
        status = RegEnumValueA(hkey, index, value_name, &value_name_size, NULL, NULL, (BYTE *)value, &data_size);
        if (status == ERROR_SUCCESS)
        {
            index++;
            // hashing value name
            if (!CryptHashData(handle_hash, (BYTE *)value_name, value_name_size, 0))
            {
                dw_status = GetLastError();
                fprintf(stderr, "CryptHashData failed: %d\n", status);
                CryptReleaseContext(handle_provider, 0);
                CryptDestroyHash(handle_hash);
                RegCloseKey(hkey);
                return NULL;
            }
            // hashing data
            if (!CryptHashData(handle_hash, (BYTE *)value, data_size, 0))
            {
                dw_status = GetLastError();
                fprintf(stderr, "CryptHashData failed: %d\n", status);
                CryptReleaseContext(handle_provider, 0);
                CryptDestroyHash(handle_hash);
                RegCloseKey(hkey);
                return NULL;
            }
        } else if(status == ERROR_NO_MORE_ITEMS){
            isFinish = TRUE;
        } else {
            fprintf(stderr, "Error during enum values, status = %d\n", status);
            CryptReleaseContext(handle_provider, 0);
            CryptDestroyHash(handle_hash);
            RegCloseKey(hkey);
            return NULL;
        }
        memset(value,0, sizeof(value));
        memset(value_name,0, sizeof(value_name));
        data_size = 1023;
        value_name_size = 1023;
    }
    uint8_t *hash = (uint8_t *)calloc(hash_byte_length, 1);
    if (!hash)
    {
        fprintf(stderr, "Cannot allocate %d bytes for md5 hash\n", hash_byte_length);
        CryptReleaseContext(handle_provider, 0);
        CryptDestroyHash(handle_hash);
        RegCloseKey(hkey);
        return NULL;
    }
    if (!CryptGetHashParam(handle_hash, HP_HASHVAL, (BYTE *)hash, &hash_byte_length, 0))
    {
        dw_status = GetLastError();
        fprintf(stderr, "CryptGetHashParam failed: %d\n", dw_status);
        CryptReleaseContext(handle_provider, 0);
        CryptDestroyHash(handle_hash);
        RegCloseKey(hkey);
        free(hash);
        return NULL;
    }
    CryptReleaseContext(handle_provider, 0);
    CryptDestroyHash(handle_hash);
    RegCloseKey(hkey);
    return hash;
}
uint8_t *md5_hash_object(const char *path, int type, HKEY* key)
{
    switch (type)
    {
    case _FILE_TYPE:
        return md5_hash_file(path);
        break;
    case _FOLDER_TYPE:
        return md5_hash_dir(path);
        break;
    case _REG_TYPE:
        char* sub_key = strstr(path, "\\") + 1;
        return md5_hash_reg(*key, sub_key);
        break;
    }
    /*DWORD attributes = GetFileAttributesA(path);

    if(attributes & FILE_ATTRIBUTE_DEVICE || attributes & FILE_ATTRIBUTE_DIRECTORY){
       return md5_hash_dir(path);
    }
    else{
      return md5_hash_file(path);
    }*/
    fprintf(stderr,"Undefined object type\n");
    return NULL;
}
HCRYPTHASH get_crypt_hash_handle(HCRYPTPROV *handle_provider, ALG_ID id)
{
    DWORD status = 0;
    HCRYPTHASH handle_hash = 0;
    // Get handle to the crypto provider
    if (!CryptAcquireContextA(handle_provider,
                              NULL,
                              NULL,
                              PROV_RSA_FULL,
                              CRYPT_VERIFYCONTEXT))
    {
        status = GetLastError();
        fprintf(stderr, "CryptAcquireContext failed: %d\n", status);
        return 0;
    }

    if (!CryptCreateHash(*handle_provider, id, 0, 0, &handle_hash))
    {
        status = GetLastError();
        fprintf(stderr, "CryptAcquireContext failed: %d\n", status);
        CryptReleaseContext(*handle_provider, 0);
        return 0;
    }
    return handle_hash;
}
uint8_t *md5_hash_dir(const char *path)
{
    HCRYPTPROV handle_provider = 0;
    DWORD status = 0;
    HCRYPTHASH handle_hash = 0;
    HANDLE handle_file = INVALID_HANDLE_VALUE;
    DWORD hash_byte_length = MD5LEN;
    handle_file = CreateFileA(path,
                              GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL,
                              OPEN_EXISTING,
                              FILE_FLAG_BACKUP_SEMANTICS,
                              NULL);
    if (handle_file == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Error during opening dir %d\n", GetLastError());
        return NULL;
    }
    LPBY_HANDLE_FILE_INFORMATION info = (LPBY_HANDLE_FILE_INFORMATION)calloc(1, sizeof(BY_HANDLE_FILE_INFORMATION));
    if (!info)
    {
        fprintf(stderr, "Cannot allocate memory for file info\n");
        CloseHandle(handle_file);
        return NULL;
    }
    GetFileInformationByHandle(handle_file, info);
    // hashing dirs: we will hash dir size, creation time and attributes only
    DWORD values[5] = {0}; // dword array for dir size, time and attributes values (summary 5 dword values)
    values[0] = info->dwFileAttributes;
    values[1] = info->nFileSizeHigh;
    values[2] = info->nFileSizeLow;
    values[3] = info->ftCreationTime.dwHighDateTime;
    values[4] = info->ftCreationTime.dwLowDateTime;
    const int values_num = sizeof(values) / sizeof(values[0]);
    char *buffer = (char *)values;
    handle_hash = get_crypt_hash_handle(&handle_provider, CALG_MD5);
    if (!handle_hash)
    {
        CloseHandle(handle_file);
        return NULL;
    }
    if (!CryptHashData(handle_hash, (BYTE *)buffer, values_num * sizeof(DWORD), 0))
    {
        status = GetLastError();
        fprintf(stderr, "CryptHashData failed: %d\n", status);
        CryptReleaseContext(handle_provider, 0);
        CryptDestroyHash(handle_hash);
        CloseHandle(handle_file);
        return NULL;
    }
    uint8_t *hash = (uint8_t *)calloc(hash_byte_length, 1);
    if (!hash)
    {
        fprintf(stderr, "Cannot allocate %d bytes for md5 hash\n", hash_byte_length);
        CryptReleaseContext(handle_provider, 0);
        CryptDestroyHash(handle_hash);
        CloseHandle(handle_file);
        return NULL;
    }
    if (!CryptGetHashParam(handle_hash, HP_HASHVAL, (BYTE *)hash, &hash_byte_length, 0))
    {
        status = GetLastError();
        fprintf(stderr, "CryptGetHashParam failed: %d\n", status);
        CryptReleaseContext(handle_provider, 0);
        CryptDestroyHash(handle_hash);
        CloseHandle(handle_file);
        free(hash);
        return NULL;
    }
    CryptReleaseContext(handle_provider, 0);
    CryptDestroyHash(handle_hash);
    CloseHandle(handle_file);
    return hash;
}
uint8_t *md5_hash_file(const char *filename)
{
    DWORD status = 0;
    BOOL result = FALSE;
    HCRYPTPROV handle_provider = 0;
    HCRYPTHASH handle_hash = 0;
    HANDLE handle_file = NULL;
    BYTE buffer[BUFSIZE];
    DWORD bytes_read = 0;
    DWORD hash_byte_length = MD5LEN;
    // Logic to check usage goes here.
    handle_file = CreateFileA(filename,
                              GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL,
                              OPEN_EXISTING,
                              FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_BACKUP_SEMANTICS,
                              NULL);

    if (INVALID_HANDLE_VALUE == handle_file)
    {
        status = GetLastError();
        fprintf(stderr, "Error opening file %s\nError: %d\n", filename,
                status);
        return NULL;
    }
    /* // Get handle to the crypto provider
     if (!CryptAcquireContextA(&handle_provider,
         NULL,
         NULL,
         PROV_RSA_FULL,
         CRYPT_VERIFYCONTEXT))
     {
         status = GetLastError();
         fprintf(stderr,"CryptAcquireContext failed: %d\n", status);
         CloseHandle(handle_file);
         return NULL;
     }

     if (!CryptCreateHash(handle_provider, CALG_MD5, 0, 0, &handle_hash))
     {
         status = GetLastError();
         fprintf(stderr,"CryptAcquireContext failed: %d\n", status);
         CloseHandle(handle_file);
         CryptReleaseContext(handle_provider, 0);
         return NULL;
     }*/
    handle_hash = get_crypt_hash_handle(&handle_provider, CALG_MD5);
    if (!handle_hash)
    {
        CloseHandle(handle_file);
        return NULL;
    }
    // hash content of file via buffer & cycle
    while (result = ReadFile(handle_file, buffer, BUFSIZE - 1,
                             &bytes_read, NULL))
    {
        if (0 == bytes_read)
        {
            break;
        }

        if (!CryptHashData(handle_hash, (BYTE *)buffer, bytes_read, 0))
        {
            status = GetLastError();
            fprintf(stderr, "CryptHashData failed: %d\n", status);
            CryptReleaseContext(handle_provider, 0);
            CryptDestroyHash(handle_hash);
            CloseHandle(handle_file);
            return NULL;
        }
    }

    if (!result)
    {
        status = GetLastError();
        fprintf(stderr, "ReadFile failed: %d\n", status);
        CryptReleaseContext(handle_provider, 0);
        CryptDestroyHash(handle_hash);
        CloseHandle(handle_file);
        return NULL;
    }
    uint8_t *hash = (uint8_t *)calloc(hash_byte_length, 1);
    if (!hash)
    {
        fprintf(stderr, "Cannot allocate %d bytes for md5 hash\n", hash_byte_length);
        CryptReleaseContext(handle_provider, 0);
        CryptDestroyHash(handle_hash);
        CloseHandle(handle_file);
        return NULL;
    }
    if (!CryptGetHashParam(handle_hash, HP_HASHVAL, (BYTE *)hash, &hash_byte_length, 0))
    {
        status = GetLastError();
        fprintf(stderr, "CryptGetHashParam failed: %d\n", status);
        CryptReleaseContext(handle_provider, 0);
        CryptDestroyHash(handle_hash);
        CloseHandle(handle_file);
        free(hash);
        return NULL;
    }
    CryptReleaseContext(handle_provider, 0);
    CryptDestroyHash(handle_hash);
    CloseHandle(handle_file);
    return hash;
}
char *hash_to_hex_str(const uint8_t *hash, uint16_t hash_len)
{
    char *res = (char *)calloc(2 * hash_len + 1, 1);
    const char *hex_digits = "0123456789abcdef";
    char *ptr = res;
    for (int i = 0; i < hash_len; i++)
    {
        sprintf(ptr, "%c%c", hex_digits[hash[i] >> 4], hex_digits[hash[i] & 0xf]);
        ptr += 2;
    }
    return res;
}