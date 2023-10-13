#ifndef LIST_H
#define LIST_H
#include <stdio.h>
	//List element structure
	typedef struct Item {
		void* data;
		struct Item* next;
		struct Item* prev;
	} Item;

	//List structure
	typedef struct LinkedList {
		Item* head;
		Item* tail;
		int size; // length of the list
	}LinkedList;

	/**
	 Create empty list (all fields are zero or NULL)
	 @return pointer to LinkedList structure
	*/
	 LinkedList* CreateList();

    /**
	 * Find item in list by its index
	 * @param list given list
	 * @param index index
	 * @return pointer to the Item structure
	*/
    Item* FindByIndex(const LinkedList* list, int index);
    /**
	 * Find first item in list by comparing it with given value
	 * @param start given Item start pointer
	 * @param firstVal value for comparing
	 * @param DataComparator data comparator
	 * @return pointer to the Item structure
	*/
    Item* FindByComparator(Item* start, void* firstVal, int (*DataComparator)(void*,void*));
	 /**
	   Add new item with given data in the end of the list
	   @param list given list
	   @param data given data
	   @return 1 if item has been added successfully and 0 else
	 */
	 int Add(LinkedList* list, void* data);

	/**
	 * Remove item from list by its index
	   @param list given list
	   @param index given index
	   @return pointer to data
	*/
	 void* RemoveByIndex(LinkedList* list, int index);
    /**
	 * Remove first item in list by comparing it with given value
	 * @param start given start Item pointer
	 * @param firstVal value for comparing
	 * @param DataComparator data comparator
	 * @return pointer to the Item structure
	*/
	 void* RemoveByComparator(Item* start,void* firstVal,int (*DataComparator)(void*,void*));
	/**
	 * Erase given list without releasing data
	 @param  list given list
	*/
	 void Erase(LinkedList* list);
     /**
	  * Erase list with releasing data in items
	  * @param list given list
	  * @param ReleaseDataFptr release data function
	 */
     void DeepErase(LinkedList *list, void (*ReleaseDataFptr)(void*));
#endif
