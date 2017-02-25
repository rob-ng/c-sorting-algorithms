#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sorting.h"

/**
 * @brief Specifies length at / below which more complex sorts (e.g. quicksort)
 * should defer to simpler sorts (e.g. insertion sort).
 */
#define LENGTH_THRESHOLD 7

/**
 * @brief Sort array of arbitrary values using insertion sort.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void
insert_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*,void*))
{
  insert_sort_partial(arr, size, compare, 0, nelems);
}

/**
 * @brief Sort subarray of array using insertion sort.
 *
 * @param arr Array to be sorted.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower bound (inclusive).
 * @param hi Upper bound (exclusive).
 * @return Void.
 */
void
insert_sort_partial(void* arr, size_t size, int (*compare)(void*,void*), size_t lo, size_t hi) 
{
  char* arrp = (char*)arr;
  size_t i, j;
  void *a, *b;
  for (i = lo + 1; i < hi; i++) {
    j = i;
    a = arrp+(j*size);
    b = arrp+((j-1)*size);
    while (j > lo && compare(a, b) < 0) {
      swap(a, b, size);
      j--;
      a = arrp+(j*size);
      b = arrp+((j-1)*size);
    }
  }
}

/**
 * @brief Sort array of arbitrary values using selection sort.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elemenets in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void
select_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*))
{
  char* arr_p = (char*)arr;
  size_t i, j, select_ind;
  for (i = 0; i < nelems; i++) {
    select_ind = i;
    for (j = i; j < nelems; j++) {
      if (compare(arr_p+(j*size), arr_p+(select_ind*size)) < 0) {
        select_ind = j;
      }
    }
    if (select_ind != i) {
      swap(arr_p+(i*size), arr_p+(select_ind*size), size);
    }
  }
}

/**
 * Sort array of arbitrary values using comb sort (bubble sort variant).
 *
 * Comb sort is an improvement over standard bubble sort. As in bubble sort,
 * comb sort operates by repeatedly looping through an array and swapping
 * elements until the array is sorted. What distinguishes the two is that
 * bubble sort always compares consecutive elements, while comb sort compares
 * elements a distance k apart. On each new iteration through the loop, the
 * value of k is shrunk by some fixed factor. Once k becomes less then one, k's
 * value is set to one and comb sort becomes identical to bubble sort.
 *
 * The primary benefit of comb sort over bubble sort is its ability to move
 * small values away from the end of the list. Bubble sort's is least efficient
 * when small values are at the end of the array, as comparisons are made
 * from left to right. As such, each such element can be moved left at most 1 index
 * per iteration. So if the smallest element where located at an index n, n - 1
 * loop iterations would have to occur for it to be sorted.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in array.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void
comb_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*))
{
  char* arr_p = (char*)arr;
  size_t gap = nelems;
  double shrink = 1.3;
  int sorted = 0;

  while (!sorted) {
    gap = floor(gap / shrink);
    if (gap > 1) {
      sorted = 0;
    } else {
      gap = 1;
      sorted = 1;
    }

    int i = 0;
    while (i + gap < nelems) {
      if (compare(arr_p+(i*size), arr_p+((i+gap)*size)) > 0) {
        swap(arr_p+(i*size), arr_p+((i+gap)*size), size);
        sorted = 0;
        i += gap;
      }
      i += 1;
    }
  }
}




/**
 * @brief Sort array of arbitrary values using merge sort.
 *
 * Merge sort is not efficient for small arrays. As such, merge sort is only
 * performed if the length of the array is above LENGTH_THRESHOLD.
 *
 * The function creates an auxillary array to make merging more efficient.
 * Without the auxillary array, each merge would require updating the
 * auxillary array with the current values of the main array in the interval
 * [lo, hi] before performing comparisons. By instead alternating the roles 
 * of the main and auxillary array between merges, we can skip this process 
 * entirely, as the array which is used for comparisons in the next call is 
 * the array to which values were copied in the previous call.
 * 
 * Because of this, we also need make the initial call to merge_sort_sort()
 * with the main and auxillary arrays swapped. This is so that the first merge
 * involves copying values into the main array from the auxillary array.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void
merge_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*))
{
  if (nelems <= LENGTH_THRESHOLD) {
    insert_sort(arr, nelems, size, compare);
  } else {
    void* aux = malloc(size * nelems);
    memcpy(aux, arr, size * nelems);
    merge_sort_sort(aux, arr, size, compare, 0, nelems-1);
    free(aux);
  }
}

/**
 * @brief Helper (merge_sort): Recursively perform merge sort.
 *
 * Merge sort is only perfomed if current index interval [lo, hi] is
 * sufficiently large. If it isn't, insertion sort is used to sort the array in
 * that interval.
 *
 * Note that the merge_sort_sort() and merge_sort_merge() calls swap the order
 * of arr and aux. This ensures that the aux array (INTO which values are
 * copied from arr in merge) is the array FROM which values are copied in the next
 * merge. See merge_sort() documentation for why this is done.
 *
 * Note: we use lo + (hi - lo) / 2 rather than (hi + lo) / 2 to guard against
 * overflow.
 *
 * @param arr Array from which values will be copied.
 * @param aux Array into which values will be copied.
 * @param size Size of each element in either array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower bound of subarray (inclusive).
 * @param hi Upper bound of subarray (inclusive).
 * @return Void.
 */
void
merge_sort_sort(void* arr, void* aux, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi)
{
  if (hi <= lo) {
    return;
  } else if (hi - lo <= LENGTH_THRESHOLD) {
    insert_sort_partial(aux, size, compare, lo, hi+1);
  } else {
    size_t mid = lo + (hi - lo) / 2;
    merge_sort_sort(aux, arr, size, compare, lo, mid);
    merge_sort_sort(aux, arr, size, compare, mid+1, hi);
    merge_sort_merge(arr, aux, size, compare, lo, mid, hi);
  }
}

/**
 * @brief Helper (merge_sort): "Merge" subarrays by updating aux with sorted values taken from arr.
 *
 * @param arr Array from which values are copied.
 * @param aux Array to which sorted values are copied.
 * @param size Size of each element in either array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower bound of the sub array.
 * @param mid Midpoint of the subarray.
 * @param hi Higher bound of the subarry.
 * @return Void.
 */
void
merge_sort_merge(void* arr, void* aux, size_t size, int (*compare)(void*, void*), size_t lo, size_t mid, size_t hi)
{
  char* arr_p = (char*)arr;
  char* aux_p = (char*)aux;
  void *a, *b;
  size_t i = lo, j = mid+1, k;
  for (k = lo; k <= hi; k++) {
    if (i <= mid && (j > hi || compare(arr_p+(i*size), arr_p+(j*size)) < 0)) {
      memcpy(aux_p+(k*size), arr_p+(i*size), size);
      i++;
    } else {
      memcpy(aux_p+(k*size), arr_p+(j*size), size);
      j++;
    }
  }
}

/**
 * @brief Sort array of arbitrary values using quicksort.
 *
 * If array is small enough, function defers to insertion sort instead.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void
quick_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*))
{
  if (nelems <= LENGTH_THRESHOLD) {
    insert_sort(arr, nelems, size, compare);
  } else {
    quick_sort_sort(arr, size, compare, 1, nelems-1);
  }
}

/**
 * @brief Helper (quick_sort): Recursively perform quicksort.
 *
 * Quicksort is not efficient for small arrays. As such, insertion sort is used
 * when subarray is small.
 *
 * @param arr Array to be sorted.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower index bound of current subarray (inclusive).
 * @param hi Upper index bound of the current subarray (invclusive).
 * @return Void.
 */
void
quick_sort_sort(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi)
{
  if (hi <= lo) {
    return;
  } else if (hi - lo <= LENGTH_THRESHOLD) {
    insert_sort_partial(arr, size, compare, lo, hi+1);
  } else {
    size_t pivot = quick_sort_partition(arr, size, compare, lo, hi);
    quick_sort_sort(arr, size, compare, lo, pivot);
    quick_sort_sort(arr, size, compare, pivot+1, hi);
  }
}

/**
 * @brief Helper (quick_sort): Partition subarray around pivot element.
 *
 * Smaller elements are moved to left of pivot, larger to right of pivot.
 * The pivot index is set to the median of the lower, middle, and upper bounds
 * of the subarray. Choosing the median element results in a faster sort than
 * always using one of lo, mid, and hi.
 *
 * Aside from choice of pivot, this is the Hoare partition scheme.
 *
 * @param arr Array containing the subarray.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower index bound of the subarray (inclusive).
 * @param hi Upper index bound of the subarray (inclusive).
 * @return Final index of the pivot element.
 */
size_t
quick_sort_partition(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi)
{
  char* arr_p = (char*)arr;
  size_t mid = lo + (hi - lo) / 2;
  size_t pivot = median_three(arr, size, lo, mid, hi, compare);

  size_t left = lo - 1, right = hi + 1;
  while (1) {
    do {
      left++;
    } while (compare(arr_p+(left*size), arr_p+(pivot*size)) < 0);

    do {
      right--;
    } while (compare(arr_p+(right*size), arr_p+(pivot*size)) > 0);

    if (left >= right) {
      return right;
    }

    swap(arr_p+(left*size), arr_p+(right*size), size);
    if (left == pivot || right == pivot) {
      pivot = (left == pivot) ? right : left;
    }
  }
}

/**
 * @brief Swap the values referenced by two pointers.
 */
void
swap(void* a, void* b, size_t size)
{
  void* tmp = malloc(sizeof(size));

  memcpy(tmp, a, size);
  memcpy(a, b, size);
  memcpy(b, tmp, size);

  free(tmp);
}

/**
 * @brief Find median of three elements in given array and return its index.
 *
 * Choosing median of lo, mid, and hi during quicksort gives a better pivot
 * point than using either a fixed position or the midpoint of hi and lo.
 *
 * @param arr Array containing the elements.
 * @param size Size of each element.
 * @param a First element.
 * @param b Second element.
 * @param c Third element.
 * @param compare Function to be used to compare elements.
 * @return Index of the median element.
 */
size_t
median_three(void* arr, size_t size, size_t a, size_t b, size_t c, int (*compare)(void*, void*))
{
  char* arr_p = (char*)arr;
  if (compare(arr_p+(a*size), arr_p+(b*size)) > 0) {
    if (compare(arr_p+(b*size), arr_p+(c*size)) > 0) {
      return b;
    } else if (compare(arr_p+(a*size), arr_p+(c*size)) > 0) {
      return c;
    } else {
      return a;
    }
  } else {
    if (compare(arr_p+(a*size), arr_p+(c*size)) > 0) {
      return a;
    } else if (compare(arr_p+(b*size), arr_p+(c*size)) > 0) {
      return c;
    } else {
      return b;
    }
  }
}
