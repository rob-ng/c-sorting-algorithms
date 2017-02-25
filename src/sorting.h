#ifndef MY_SORTING_ALGORITHMS_
#define MY_SORTING_ALGORITHMS_
#include <stdlib.h>

/*=== Simple Sorts ===*/

/**
 * @brief Sort array using insertion sort.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void insert_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*));
/**
 * @brief Sort subarray of array using insertion sort.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @param lo The lower bound of the subarray (inclusive).
 * @param hi The upper bound of the subarray (exlusive).
 * @return Void.
 */
void insert_sort_partial(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi);

/**
 * @brief Sort array using selection sort
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void select_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*));


/*=== Efficient Sorts ===*/

/**
 * @brief Sort array using merge sort.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void merge_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*));
/**
 * @brief Helper (merge_sort): Perform merge sort recursively.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower bound of current subarray (inclusive).
 * @param hi Upper bound of current subarray (inclusive).
 * @return Void.
 */
void merge_sort_sort(void* arr, void* aux, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi);
/**
 * @brief Helper (merge_sort): "Merge" arrays by updating aux array using sorted values taken from arr.
 *
 * @param arr Array from which values should be copied.
 * @param aux Array into which values should be copied.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower bound of current subarray (inclusive).
 * @param lo Middle of current subarray (inclusive).
 * @param hi Upper bound of current subarray (inclusive).
 * @return Void.
 */
void merge_sort_merge(void* arr, void* aux, size_t size, int (*compare)(void*, void*), size_t lo, size_t mid, size_t hi);

/**
 * @brief Sort array using quicksort
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void quick_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*));
/**
 * @brief Helper (quick_sort): Perform quicksort recursively.
 *
 * @param arr Array to be sorted.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower bound of subarray (inclusive).
 * @param hi Upper bound of subarray (inclusive).
 * @return Void.
 */
void quick_sort_sort(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi);
/**
 * @brief Helper (quick_sort): Partion subarray of arr given by bounds around
 * pivot.
 *
 * Elements smaller than pivot should go to its left, larger to its right.
 * 
 * @param arr Array to be sorted.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower bound of subarray (inclusive).
 * @param hi Upper bound of subarray (inclusive).
 * @return Final index of pivot point after partioning.
 */
size_t quick_sort_partition(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi);


/*=== Helpers ===*/

/**
 * @brief Swap values referenced by two pointers.
 *
 * @param a Pointer to first value.
 * @param b Pointer to second value.
 * @param size Size of referenced values (must be same).
 * @return Void.
 */
void swap(void* a, void* b, size_t size);
/**
 * @brief Determine median of three array elements and return its index.
 *
 * @param arr Array containing the elements.
 * @param size Size of each element.
 * @param a First element.
 * @param b Second element.
 * @param c Third element.
 * @param compare Function to be used to compare elements.
 */
size_t median_three(void* arr, size_t size, size_t a, size_t b, size_t c, int (*compare)(void*, void*));

#endif /* MY_SORTING_ALGORITHMS_ */
