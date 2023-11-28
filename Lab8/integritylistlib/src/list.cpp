#include "../include/list.h"
#include <stdlib.h>
#include <stdio.h>
	 LinkedList* CreateList() {
		LinkedList* res = (LinkedList*)calloc(1,sizeof(LinkedList));
		if(!res){
		   fprintf(stderr, "Cannot allocate memory for linked list");
		}
		return res;
	 }
	 Item* FindByIndex(const LinkedList* list, int index) {
		if (index < 0 || index >= list->size) {
			return NULL;
		}
		Item* ptr = list->head;
		int i = 0;
		while (ptr)
		{
			if (i == index) {
				return ptr;
			}
			ptr = ptr->next;
			i++;
		}
		return NULL;
	 }
     Item* FindByComparator(Item* start,void *val,int (*DataComparator)(void*,void*)){
	    Item* ptr = start;
		while (ptr)
		{
			if (!DataComparator(val,ptr->data)) {
				return ptr;
			}
			ptr = ptr->next;
		}
		return NULL;
	 }
	 int AddZeroItemToTail(LinkedList* list) {
		Item* item = (Item*) calloc(1, sizeof(Item));
		if(!item){
			return 0;
		}
		item->prev = list->tail;
//		Item* item = new Item(nullptr, list.size, nullptr, list.tail);
		if (!list->tail) {
			list->head = item;
			list->tail = item;
		}
		else {
			list->tail->next = item;
			list->tail = item;
		}
		list->size++;
		return 1;
	 }

	 int Add(LinkedList* list, void* data) {
		  if(!AddZeroItemToTail(list)){
			return 0;
		  }
		  list->tail->data = data;
		  return 1;
	 }

	void* RemoveByIndex(LinkedList* list, int index) {
		void* res;
		Item* ptr = FindByIndex(list, index);
		if (!ptr) {
			return NULL;
		}
		if (list->head == ptr) {
			list->head = list->head->next;
			if (list->head == NULL) {
				list->tail = NULL;
			}
			else {
				list->head->prev = NULL;
			}
			res = ptr->data;
			free(ptr);
			list->size--;
			return res;
		}
		if (list->tail == ptr) {
			list->tail = list->tail->prev;
			list->tail->next = NULL;
			res = ptr->data;
			free(ptr);
			list->size--;
			return res;
		}
		Item* prev = ptr->prev;
		prev->next = ptr->next;
		ptr->next->prev = prev;
		res = ptr->data;
		free(ptr);
		list->size--;
		return res;
	}

	void Erase(LinkedList* list) {
		if (!list) {
			return;
		}
		while (list->size > 0) {
			RemoveByIndex(list, 0);
		}
		free(list);
	}
    
	void DeepErase(LinkedList *list, void (*ReleaseDataFptr)(void*)){
		if (!list || !ReleaseDataFptr) {
			return;
		}
		while (list->size > 0) {
			   ReleaseDataFptr(RemoveByIndex(list, 0));
		}
		free(list);
	}

	/*void* GetDataByIndex(const LinkedList* list, int index) {
		Item* item = FindByIndex(list, index);
		if (item) {
			return item->data;
		}
		return NULL;
	}*/
