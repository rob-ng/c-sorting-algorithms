#ifndef MY_SORTING_ALGORITHMS_
#define MY_SORTING_ALGORITHMS_

#include "stack.h"

/* @def LENGTH_THRESHOLD
 * Maximum array length at which complex sorts should defer to simpler 
 * sorts. */
#define LENGTH_THRESHOLD 7
/* @def MIN_GALLOP
 * Default minimum galloping threshold for Timsort. */
#define MIN_GALLOP 7

typedef struct TimsortRun TimsortRun;
typedef struct TimsortMergeState TimsortMergeState;

////////////////////////////////////////////////////////////////////////////////
/// SIMPLE SORTS
////////////////////////////////////////////////////////////////////////////////

void insert_sort(void* arr, size_t nelems, size_t size, 
                 int (*compare)(void*, void*));

void insert_sort_partial(void* arr, size_t size, 
                         int (*compare)(void*, void*), 
                         size_t lo, size_t hi);

void binary_insert_sort(void* arr, size_t size, 
                        int (*compare)(void*, void*), 
                        size_t lo, size_t hi);

void select_sort(void* arr, size_t nelems, size_t size, 
                 int (*compare)(void*, void*));

////////////////////////////////////////////////////////////////////////////////
/// BUBBLE SORTS
////////////////////////////////////////////////////////////////////////////////

void comb_sort(void* arr, size_t nelems, size_t size, 
               int (*compare)(void*, void*));

////////////////////////////////////////////////////////////////////////////////
/// EFFICIENT SORTS
////////////////////////////////////////////////////////////////////////////////

void merge_sort(void* arr, size_t nelems, size_t size, 
                int (*compare)(void*, void*));

void merge_sort_sort(void* arr, void* aux, size_t size, 
                     int (*compare)(void*, void*), 
                     size_t lo, size_t hi);

void merge_sort_merge(void* arr, void* aux, size_t size, 
                      int (*compare)(void*, void*), 
                      size_t lo, size_t mid, size_t hi);


void quick_sort(void* arr, size_t nelems, size_t size, 
                int (*compare)(void*, void*));

void quick_sort_sort(void* arr, size_t size, 
                     int (*compare)(void*, void*), 
                     size_t lo, size_t hi);

size_t quick_sort_partition(void* arr, size_t size, 
                            int (*compare)(void*, void*), 
                            size_t lo, size_t hi);

////////////////////////////////////////////////////////////////////////////////
/// HYBRID SORTS
////////////////////////////////////////////////////////////////////////////////

void timsort(void* arr, size_t nelems, size_t size, 
             int (*compare)(void*, void*));

static size_t timsort_minrun(size_t nelems);

static void timsort_find_runs(void* arr, size_t nelems, size_t size, 
                              int (*compare)(void*, void*), 
                              size_t minrun, TimsortMergeState* merge_state);

static TimsortRun* timsort_merge_runs(void* arr, size_t size, 
                                      int (*compare)(void*, void*), 
                                      TimsortRun* a, TimsortRun* b, 
                                      TimsortMergeState* merge_state);

static void timsort_merge_runs_lo(void* arr, size_t size, 
                                  int (*compare)(void*, void*), 
                                  size_t lo, size_t lo_len, 
                                  size_t hi, size_t hi_len, 
                                  TimsortMergeState* merge_state); 

static void timsort_merge_runs_hi(void* arr, size_t size, 
                                  int (*compare)(void*, void*), 
                                  size_t lo, size_t lo_len, 
                                  size_t hi, size_t hi_len, 
                                  TimsortMergeState* merge_state);

static int timsort_gallop_right(void* src, size_t size, 
                                int (*compare)(void*, void*), 
                                int base, int limit, void* target);

static int timsort_gallop_left(void* src, size_t size, 
                               int (*compare)(void*, void*), 
                               int base, int limit, void* target);

static void timsort_check_invariants(void* arr, size_t size, 
                                     int (*compare)(void*, void*), 
                                     TimsortMergeState* merge_state);

static void timsort_collapse_runs(void* arr, size_t size, 
                                  int (*compare)(void*, void*), 
                                  TimsortMergeState* merge_state);

static int timsort_binary_search(void* arr, size_t size, 
                                 int (*compare)(void*, void*), 
                                 size_t lo, size_t hi, void* target);

////////////////////////////////////////////////////////////////////////////////
/// HELPERS
////////////////////////////////////////////////////////////////////////////////

void swap(void* a, void* b, size_t size);

size_t median_three(void* arr, size_t size, size_t a, size_t b, size_t c, 
                    int (*compare)(void*, void*));

static void reverse_array(void* arr, size_t start, size_t end, size_t size);

static int bin_search(void* arr, size_t size, 
                      int (*compare)(void*, void*), 
                      size_t lo, size_t hi, void* target);

#endif /* MY_SORTING_ALGORITHMS_ */
