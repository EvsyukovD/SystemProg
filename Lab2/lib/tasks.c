#include "tasks.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void task_1()
{
    int a, b;
    printf("Enter a and b\n");
    scanf("%d %d", &a, &b);
    int *a_ptr = &a, *b_ptr = &b;
    if (a > b)
    {
        *a_ptr *= 2;
    }
    else
    {
        *b_ptr /= 2;
    }
    printf("a = %d b = %d\n", a, b);
}

void task_2()
{
    double *x = (double *)calloc(1, sizeof(double));
    double *y = (double *)calloc(1, sizeof(double));
    if(!x || !y){
        free(x);
        free(y);
        printf("Can't allocate memory\n");
        return;
    }
    scanf("%lf %lf", x, y);
    *x = *x * 2.0;
    printf("%0.5lf %0.5lf\n", *x, *y);
    free(x);
    free(y);
}

void task_3()
{
    srand(time(NULL));
    int n = 10, m = 20;
    int *x = (int *)calloc(n, sizeof(int));
    int *y = (int *)calloc(m, sizeof(int));
    if(!x || !y){
        free(x);
        free(y);
        printf("Can't allocate memory\n");
        return;
    }
    int num = 0, i = 0, j = 0;
    while (i < n || j < m)
    { // init arrays x and y
        if (i < n)
        {
            x[i] = (rand() % 10) - 5;
        }
        if (j < m)
        {
            y[j] = (rand() % 10) - 5;
        }
        if (i >= 1)
        { // calculate number of x[1] in y array
            if (i == 1)
            {
                num = y[0] == x[1] ? num + 1 : num;
                num = y[1] == x[1] ? num + 1 : num;
            }
            else
            {
                num = y[j] == x[1] ? num + 1 : num;
            }
        }
        i++;
        j++;
    }
    // print x and y arrays
    for (int i = 0; i < n; i++)
    {
        printf("x[%d] = %d\n", i, x[i]);
    }
    for (int i = 0; i < m; i++)
    {
        printf("y[%d] = %d\n", i, y[i]);
    }
    printf("amount = %d\n", num);
    free(x);
    free(y);
}
int int_comparator(const void *a, const void *b)
{

    return (*((int *)a) - *((int *)b));
}
void task_4()
{
    int n = 10;
    int *a = (int *)calloc(n, sizeof(int));
    int *b = (int *)calloc(n, sizeof(int));
    if(!a || !b){
        free(a);
        free(b);
        printf("Can't allocate memory\n");
        return;
    }
    int j = 0; // free position in b array
    printf("a:");
    //init a and in the same time find positive ints in it
    for (int i = 0; i < n; i++)
    {
        a[i] = (rand() % 10) - 5;
        printf("%d ", a[i]);
        if (a[i] > 0)
        {
            b[j] = 3 * a[i];
            j++;
        }
    }
    printf("\n");
    int* ptr = (int *)realloc(b, j * sizeof(int));
    if(!ptr){
        printf("Can't shrink array b\n");
        free(a);
        free(b);
        return;
    }
   // b = ptr;
    // sort
    // size of b = j
    qsort(ptr, j, sizeof(int), int_comparator);
    printf("b:");
    for (int i = 0; i < j; i++)
    {
        printf("%d ", ptr[i]);
    }
    printf("\n");
    free(a);
    free(ptr);
}

void minutes2seconds(int minutes, long *seconds)
{
    *seconds = minutes * 60;
}

void task_5()
{
    int m1, m2; // minutes
    printf("Enter minutes for m1 and m2\n");
    scanf("%d %d", &m1, &m2);
    long sec1, sec2;     // seconds
    minutes2seconds(m1, &sec1);
    minutes2seconds(m2, &sec2);
    printf("%d minutes = %d\n", m1, sec1);
    printf("%d minutes = %d\n", m2, sec2);
}

void release_town_address(TownAddress *townAddress)
{
    if (townAddress)
    {
        free(townAddress->street);
        free(townAddress);
    }
}

void release_profile(Profile *profile)
{
    if (profile)
    {
        free(profile->last_name);
        free(profile->town);
        release_town_address(profile->address);
        free(profile);
    }
}

TownAddress *create_random_townaddress()
{
    int str_size = 20;
    const char *some_streets[] = {"Street 1", "Street 2", "Street 3"};
    int n = sizeof(some_streets) / sizeof(char *);
    TownAddress *res = (TownAddress *)calloc(1, sizeof(TownAddress));
    if(!res){
        return NULL;
    }
    res->flat = 1;  // rand() % 300 + 1;
    res->house = 1; // rand() % 100 + 1;
    res->street = (char *)calloc(str_size, sizeof(char));
    if(!res->street){
        release_town_address(res);
        return NULL;
    }
    int i = 0; // rand() % n;
    strcpy(res->street, some_streets[i]);
    return res;
}
Profile *create_random_data_profile()
{
    int town_str_size = 10, last_name_size = 10;
    const char *towns[] = {"Town 1", "Town 2", "Town 3"};
    int towns_num = sizeof(towns) / sizeof(char *);
    const char *last_names[] = {"Ivanov", "Petrov", "Sidorov", "Abramov", "Vasiliev", "Smirnov", "Zaharov"};
    int last_names_num = sizeof(last_names) / sizeof(char *);

    Profile *res = (Profile *)calloc(1, sizeof(Profile));
    if(!res){
        return NULL;
    }
    res->town = (char *)calloc(town_str_size, sizeof(char));
    res->last_name = (char *)calloc(last_name_size, sizeof(char));

    if(!res->town || !res->last_name){
        release_profile(res);
        return NULL;
    }
    strcpy(res->town, towns[rand() % towns_num]);
    strcpy(res->last_name, last_names[rand() % last_names_num]);
    res->address = create_random_townaddress();
    if(!res->address){
        release_profile(res);
        return NULL;
    }
    return res;
}
void release_profiles(Profile** profiles, int size){
    if(profiles){
        for (int i = 0; i < size; i++)
        {
            release_profile(profiles[i]);
        }
        free(profiles);
    }
}
Profile **create_random_data_profiles(int num_of_profiles)
{
    Profile **profiles = (Profile **)calloc(num_of_profiles, sizeof(Profile *));
    if(!profiles){
        return NULL;
    }
    for (int i = 0; i < num_of_profiles; i++)
    {
        profiles[i] = create_random_data_profile();
        if(!profiles[i]){
            release_profiles(profiles, i + 1);
            return NULL;
        }
    }
    return profiles;
}
int compare(const TownAddress *a, const TownAddress *b)
{
    return a->flat == b->flat && a->house == b->house && !strcmp(a->street, b->street);
}
void task_6()
{
    int num_of_profiles = 100;
    Profile **profiles = create_random_data_profiles(num_of_profiles);

    for (int i = 0; i < num_of_profiles; i++)
    {
        for (int j = i + 1; j < num_of_profiles; j++)
        {
            if (strcmp(profiles[i]->town, profiles[j]->town) && compare(profiles[i]->address, profiles[j]->address))
            {
                printf("First: %s, second: %s\n", profiles[i]->last_name, profiles[j]->last_name);
                i = num_of_profiles;
                break;
            }
        }
    }
    release_profiles(profiles, num_of_profiles);
}
void release_trips(Trip** t, int size){
     if(t){
        for (int i = 0; i < size; i++)
        {
            release_trip(t[i]);
        }
        free(t);
     }
}
Trip *create_random_trip()
{
    int str_size = 20;
    srand(time(NULL));
    static int i = 0;
    const char *towns[] = {"Moscow", "St. Peterburg", "Omsk",  "Paris", "Washington", "Tomsk","Yakutsk","Vladivostok", 
    "Osaka", "Okinava", "Archangelsk", "Arzamas"};
    int towns_num = sizeof(towns) / sizeof(char *);
    Trip *trip = (Trip *)calloc(1, sizeof(Trip));
    if(!trip){
        return NULL;
    }
    trip->dest = (char *)calloc(str_size, sizeof(char));
    if(!trip->dest){
        release_trip(trip);
        return NULL;
    }
    strcpy(trip->dest, towns[i % towns_num]);
    i++;// - for static i

    struct tm fixed_time_start = {0, 0, 0, 1, 1, 2020}; // fixed start time
    time_t hour = 60;
    time_t random_duration = hour * (rand() % 3 + 1);
    trip->duration = random_duration;
    trip->start = fixed_time_start;
    time_t finish = mktime(&fixed_time_start) + random_duration; // finish time
    trip->finish = *gmtime(&finish);
    return trip;
}
Trip **create_random_trips(int num_of_trips)
{
    Trip **trips = (Trip **)calloc(num_of_trips, sizeof(Trip *));
    if(!trips){
        return NULL;
    }
    for (int i = 0; i < num_of_trips; i++)
    {
        trips[i] = create_random_trip();
        if(!trips[i]){
            release_trips(trips, i + 1);
            return NULL;
        }
    }
    return trips;
}
int trips_comparator(const void *a, const void *b)
{
    Trip **t1 = (Trip **)a;
    Trip **t2 = (Trip **)b;
    return strcmp((*t1)->dest, (*t2)->dest);
}
void release_trip(Trip *trip)
{
    if (trip)
    {
        free(trip->dest);
        free(trip);
    }
}
int check_string_retry(char *src, const char *str)
{
    char *ptr = strstr(src, str);
    if (ptr)
    {
        ptr = strstr(ptr + 1, str);
        if (ptr)
        {
            return 1;
        }
    }
    return 0;
}
void select_trips_by_string_retry(Trip **src, Trip **dest, int trips_num, const char *str, int *size_of_dest)
{
    int j = 0;
    for (int i = 0; i < trips_num; i++)
    {
        if (check_string_retry(src[i]->dest, str))
        {
            dest[j] = src[i];
            // src[i] = NULL;
            j++;
        }
    }
    *size_of_dest = j;
}
void print_trip(const Trip* t){
    printf("Dest: %s\n", t->dest);
    printf("Time start: %d:%d:%d\n", t->start.tm_hour, t->start.tm_min, t->start.tm_sec);
    printf("Time finish: %d:%d:%d\n", t->finish.tm_hour, t->finish.tm_min, t->finish.tm_sec);
    printf("Duration %d\n", t->duration);
}
void print_trips(const Trip** trips, int size){
    for (int i = 0; i < size; i++)
    {
        print_trip(trips[i]);
    }
}
void task_7()
{
    int num_of_trips = 10;
    Trip **trips = create_random_trips(num_of_trips);
    Trip **auxarray = (Trip **)calloc(num_of_trips, sizeof(Trip *));
    if(!trips || !auxarray){
        release_trips(trips, num_of_trips);
        release_trips(auxarray, num_of_trips);
        printf("Can't allocate memory\n");
        return;
    }
    int size = 0;
    printf("Trips:\n");
    print_trips(trips, num_of_trips);
    select_trips_by_string_retry(trips, auxarray, num_of_trips, "a", &size);
    qsort(trips, size, sizeof(Trip *), trips_comparator);
    printf("\nNew array:\n");
    print_trips(auxarray, size);
    release_trips(trips, num_of_trips);
    free(auxarray);
}