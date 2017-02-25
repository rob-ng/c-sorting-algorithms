#ifndef MY_SORTING_ALGORITHMS_
#define MY_SORTING_ALGORITHMS_
#include <stdlib.h>

/* Simple Sorts */
void insert_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*));
void insert_sort_partial(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi);

void select_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*));


/* Efficient Sorts */
void merge_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*));
void merge_sort_sort(void* arr, void* aux, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi);
void merge_sort_merge(void* arr, void* aux, size_t size, int (*compare)(void*, void*), size_t lo, size_t mid, size_t hi);

void quick_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*));
void quick_sort_sort(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi);
size_t quick_sort_partition(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi);


/* Helpers */
void swap(void* a, void* b, size_t size);
size_t median_three(void* arr, size_t size, size_t a, size_t b, size_t c, int (*compare)(void*, void*));

#endif /* MY_SORTING_ALGORITHMS_ */
