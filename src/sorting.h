#ifndef MY_SORTING_ALGORITHMS_
#define MY_SORTING_ALGORITHMS_
#include <stdlib.h>

/* Simple Sorts */
void insert_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*));
void insert_sort_partial(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi);


/* Helpers */
void swap(void* a, void* b, size_t size);

#endif /* MY_SORTING_ALGORITHMS_ */
