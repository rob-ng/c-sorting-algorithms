#include <stdlib.h>
#include <string.h>
#include "sorting.h"

#define LENGTH_THRESHOLD 7

void
insert_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*,void*))
{
  insert_sort_partial(arr, size, compare, 0, nelems);
}

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
 * @param arr The array to be sorted.
 * @param nelems The number of elemenets in the array.
 * @param size The size of each element in the array.
 * @param compare The function to be used to compare elements.
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
 * @param arr The array to be sorted.
 * @param nelems The number of elements in the array.
 * @param compare The function to be used to compare elements.
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
 * @param arr The array from which values will be copied.
 * @param aux The array into which values will be copied.
 * @param size The size of each element in either array.
 * @param compare The function to be used to compare elements.
 * @param lo The lower subarray index bound.
 * @param hi The upper subarray index bound.
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
 * @brief Helper (merge_sort): Update aux subarray with sorted values.
 *
 * @param arr The array from which values are copied.
 * @param aux The array to which sorted values are copied.
 * @param size The size of each element in either array.
 * @param compare The function to be used to compare elements.
 * @param lo The lower bound of the sub array.
 * @param mid The midpoint of the subarray.
 * @param hi The higher bound of the subarry.
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

void
swap(void* a, void* b, size_t size)
{
  void* tmp = malloc(sizeof(size));

  memcpy(tmp, a, size);
  memcpy(a, b, size);
  memcpy(b, tmp, size);

  free(tmp);
}
