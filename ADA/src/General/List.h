#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct _list_element {
	void * ptr_to_value;
	_list_element * ptr_to_next_element;
};

typedef struct List {
	_list_element * first_element;
	size_t type_size;
	uint32_t length;
} List;

// Initialize list
List * InitializeList(size_t element_size) {
	List * base_list = (List *)malloc(sizeof(List));
	base_list->type_size = element_size;
	base_list->length = 0;

	return base_list;
}

// Adds an item to the base_list.
int AddItemToList(List * base_list, void * value_to_add) {
	_list_element ** current_element = &(base_list->first_element);
	for (int i = 0; i < base_list->length; i++) {
		current_element = &((*current_element)->ptr_to_next_element);
	}

	*current_element = (_list_element *)malloc(sizeof(_list_element));

	(*current_element)->ptr_to_value = (void *)malloc(base_list->type_size);
	memcpy((*current_element)->ptr_to_value, value_to_add, base_list->type_size);

	return ++base_list->length; // Returns new length of the list
}

// Remove last element
int RemoveLastElementOfList(List * base_list) {
	if (base_list->length > 0) {
		_list_element * last_element = base_list->first_element;
		for (int i = 0; i < base_list->length - 1; i++) {
			last_element = last_element->ptr_to_next_element;
		}

		free(last_element->ptr_to_value);
		free(last_element);
		base_list->length--;
	}

	return base_list->length;
}

// Clear the entire list
int ClearList(List * base_list) {
	for (int i = 0; i < base_list->length; i++) {
		RemoveLastElementOfList(base_list);
	}
	
	if (base_list->length > 0) {
		// Error emptying list!
		return base_list->length;
	}
	else {
		return 0;
	}
}

// Get element at index in the list
_list_element * GetElementAtIndex(List * base_list, int index) {
	if (base_list->length == 0) {
		// Index out of range!
		return NULL;
	}

	_list_element * current_element = base_list->first_element;
	for (int i = 0; i < index; i++) {
		current_element = current_element->ptr_to_next_element;
	}

	return current_element;
}