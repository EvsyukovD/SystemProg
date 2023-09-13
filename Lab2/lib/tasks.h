#ifndef TASKS_H
#define TASKS_H
#include <time.h>
void task_1();
void task_2();
void task_3();
void task_4();
void task_5();
void task_6();
void task_7();
/**
 * @brief Translate minutes into seconds
 * @param minutes - minutes
 * @param seconds - pointer for result
*/
void minutes2seconds(int minutes, long *seconds);

typedef struct TownAddress {
    char *street;
    int house;
    int flat;
} TownAddress;
typedef struct Profile {
    char *last_name;
    char *town;
    TownAddress *address;
} Profile;
typedef struct Trip{
   char* dest;
   struct tm start;
   struct tm finish;
   time_t duration;
}Trip;
/**
 * @brief Select trips from src array by string retrying  
 * and write them to dest array, without memory allocating
 * @param src - source array
 * @param dest - destination array
 * @param trips_num - number of trips in src
 * @param str - target string
 * @param size_of_dest - number of trips which was added to dest
*/
void select_trips_by_string_retry(Trip** src, Trip** dest, int trips_num, const char* str,int* size_of_dest);
/**
 * @brief Check string retry in source string
 * @param src - source string
 * @param str - target string
 * @return 1 if string retries and 0 else
*/
int check_string_retry(char* src, const char* str);
Trip* create_random_trip();
Trip** create_random_trips(int num_of_trips);
int trips_comparator(const void*, const void*);
int int_comparator(const void*, const void*);
TownAddress *create_random_townaddress();
Profile* create_random_data_profile();
Profile **create_random_data_profiles(int size);
int compare(const TownAddress*,const TownAddress*);
void release_town_address(TownAddress *townAddress);
void release_profile(Profile *profile);
void release_trip(Trip*);
void print_trip(const Trip* t);
void print_trips(const Trip** arr, int size);
#endif