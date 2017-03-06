#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "sorting.h"
#include "stack.h"

/**
 * @brief Specifies length at / below which more complex sorts (e.g. quicksort)
 * should defer to simpler sorts (e.g. insertion sort).
 */
#define LENGTH_THRESHOLD 7

/**
 * @brief Default length at which gallop mode should begin in Timsort.
 */
#define MIN_GALLOP 7

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
 * @brief Sort subarray of array using insertion sort which uses binary search.
 *
 * @param arr The array to be sorted.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower bound of subarray.
 * @param hi Upper bound of subarray.
 * @return Void.
 */
void
binary_insert_sort(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi)
{
  char* arr_p = (char*)arr;
  int i, j, l, r, m, loc;
  void* slctd = malloc(size);

  for (i = lo + 1; i <= hi; i++) {
    j = i - 1;
    memcpy(slctd, arr_p+(i*size), size);

    l = lo, r = j;
    while (1) {
      if (r <= l) {
        loc = (compare(slctd, arr_p+(l*size)) > 0) ? (l + 1) : l;
        break;
      }
      m = floor(l + ((r - l) / 2));
      if (compare(arr_p+(m*size), slctd) < 0) {
        l = m + 1;
      } else if (compare(arr_p+(m*size), slctd) > 0) {
        r = m - 1;
      } else {
        loc = m + 1;
        break;
      }
    }

    while (j >= loc) {
      memcpy(arr_p+((j+1)*size), arr_p+(j*size), size);
      j--;
    }
    memcpy(arr_p+((j+1)*size), slctd, size);
  }
  free(slctd);
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
 * @brief Sort array of arbitrary values using Timsort.
 *
 * Timsort (developed by Tim Peters) is a hybrid stable sorting algorithm.
 * Timsort uses a combination of insertion sort and merge sort to first
 * organize the array into roughly equal length runs of ascending elements and
 * then merge those runs into a single sorted run.
 * The algorithm proceeds through the following steps:
 *
 * If array to sort is shorter than 64 elements:
 *   1. Defer to insertion sort.
 * Otherwise:
 *   1. Calculate the minimum run length.
 *   2. Initialize a struct to represent merge state for the duration of the
 *      sort.
 *   3. Find (or create if necessary) runs of at least minrun length. All runs
 *      are sorted to be ascending if they aren't already. On finding a run, 
 *      push it onto the runs stack and check to see if run invariants still 
 *      hold. Merge runs if they do not.
 *   4. Collapse the remaining runs in the runs stack into a single sorted run.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in array.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 *
 * @see timsort_minrun
 * @see timsort_find_runs
 * @see timsort_check_invariants
 * @see timsort_collapse_runs
 */
void 
timsort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*))
{
  const size_t MIN_NELEMS = 64;
  if (nelems < MIN_NELEMS) {
    binary_insert_sort(arr, size, compare, 0, nelems - 1);
  } else {
    size_t minrun = timsort_minrun(nelems);
    const size_t MAX_RUNS = nelems / minrun;
    TimsortRun runs[MAX_RUNS];
    size_t run_ind;
    for (run_ind = 0; run_ind <= MAX_RUNS; run_ind++) {
      TimsortRun run = { 0, 0 };
      runs[run_ind] = run;
    }
    TimsortMergeState merge_state = { stack_init(), runs, MAX_RUNS, MIN_GALLOP, 0 };

    timsort_find_runs(arr, nelems, size, compare, minrun, &merge_state);
    timsort_collapse_runs(arr, size, compare, &merge_state);
    stack_free(&merge_state.runs_stack);
  }
}
/**
 * @brief Find runs of either strictly descending or non-descending elements.
 *
 * Runs must be at least minrun elements long. If a run is shorter than this,
 * it is extended using consecutive elements. There is one exception; the last
 * run may be shorter if there are not enough elements to pad out its length.
 * 
 * Importantly, all runs must be non-descending before they can be pushed onto
 * the runs stack - runs which are descending are reveresed before they are
 * pushed.
 * 
 * Once a new run has been found and pushed on the runs stack, the function
 * calls timsort_check_invariants() to ensure that the run invariants still
 * hold.
 *
 * @param arr Target array.
 * @param nelems Number of elements in array.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @param minrun Minimum acceptable run length.
 * @param merge_state Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort
 * @see timsort_check_invariants
 */
void
timsort_find_runs(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*), size_t minrun, TimsortMergeState* merge_state)
{
  char* arr_p = (char*)arr;
  // The curr_run value is the index of the run in 'runs' array. When a run
  // terminates, this value is incremented and the next run begins.
  size_t i, curr_run = 0;
  // Iterate through the array and look for runs. Runs occur either when
  // consecutive values are strictly descending (i+1 < i) or non-descending
  // (i+1 >= i). Because one of these relationships must always be true, a run
  // is always occuring. 
  int new_run = 1;
  for (i = 0; i < nelems-1; i++) {
    if (new_run || (compare(arr_p+(i*size), arr_p+((i+1)*size)) == compare(arr_p+((i-1)*size), arr_p+(i*size)))) {
      if (new_run) {
        merge_state->runs[curr_run].start = i;
      }
      merge_state->runs[curr_run].len++;
      new_run = 0;
    } else {
      if (compare(arr_p+((i-1)*size), arr_p+(i*size)) < 0) {
        reverse_array(arr, merge_state->runs[curr_run].start, merge_state->runs[curr_run].start + merge_state->runs[curr_run].len - 1, size);
      }
      if (merge_state->runs[curr_run].len < minrun) {
        merge_state->runs[curr_run].len = merge_state->runs[curr_run].start + minrun - 1 >= nelems ? nelems - merge_state->runs[curr_run].start : minrun;
        i = merge_state->runs[curr_run].start + merge_state->runs[curr_run].len - 1;
        binary_insert_sort(arr, size, compare, merge_state->runs[curr_run].start, (merge_state->runs[curr_run].start + merge_state->runs[curr_run].len - 1));
      }
      stack_push(merge_state->runs_stack, &merge_state->runs[curr_run]);
      curr_run++;
      if (curr_run > merge_state->max_runs) { break; }
      new_run = 1;
    }
    
    if (new_run) {
      timsort_check_invariants(arr, size, compare, merge_state);
    }
  }
}

/**
 * @brief Check that run invariants hold and update runs stack if they do not.
 * 
 * Suppose the runs stack has at least 3 runs and let X, Y and Z be the top 3 
 * runs, ordered from right to left. Then the following must hold:
 * 1. |X| > |Y| + |Z|
 * 2. |Y| > |Z|
 * If either invariant fails to hold, merge Y with smaller of X and Z and
 * push new merged value onto stack, maintaing order.
 *
 * @param arr Target array.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @param merge_state Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort
 * @see timsort_check_invariants
 */
void
timsort_check_invariants(void* arr, size_t size, int (*compare)(void*, void*), TimsortMergeState* merge_state)
{
  while (1) {
    if (merge_state->runs_stack->len >= 3) {
      TimsortRun* x = (TimsortRun*)stack_pop_return(merge_state->runs_stack);
      TimsortRun* y = (TimsortRun*)stack_pop_return(merge_state->runs_stack);
      TimsortRun* z = (TimsortRun*)stack_pop_return(merge_state->runs_stack);

      if ((x->len <= y->len + z->len) || (y->len <= z->len)) {
        if (x->len < z->len) {
          // PUSH Z BACK ONTO STACK
          stack_push(merge_state->runs_stack, z);
          // MERGE AND PUSH X AND Y
          stack_push(merge_state->runs_stack, timsort_merge_runs(arr, size, compare, y, x, merge_state));
        } else {
          // MERGE AND PUSH Y AND Z
          stack_push(merge_state->runs_stack, timsort_merge_runs(arr, size, compare, z, y, merge_state));
          // PUSH X BACK ONTO STACK
          stack_push(merge_state->runs_stack, x);
        }
      } else {
        stack_push(merge_state->runs_stack, z);
        stack_push(merge_state->runs_stack, y);
        stack_push(merge_state->runs_stack, x);
        return;
      }
    } else {
      return;
    }
  }

}

/**
 * @brief Merge top two runs in run stack until only one run remains.
 *
 * Once all the runs have been merged, the array will be fully sorted.
 *
 * @param arr Target array.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @param merge_state Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort
 */
void
timsort_collapse_runs(void* arr, size_t size, int (*compare)(void*, void*), TimsortMergeState* merge_state)
{
  while (merge_state->runs_stack->len > 1) {
    TimsortRun* second = (TimsortRun*)stack_pop_return(merge_state->runs_stack);
    TimsortRun* first = (TimsortRun*)stack_pop_return(merge_state->runs_stack);
    stack_push(merge_state->runs_stack, timsort_merge_runs(arr, size, compare, first, second, merge_state));
  }
}

/**
 * @brief Merge 2 consecutive runs.
 *
 * @param arr Target array.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @param frst Leftmost run. 
 * @param scnd Rightmost run.
 * @param merge_state Struct containing information about merges and runs.
 * @return Pointer to first run, now with length updated to include second.
 *
 * @see timsort
 * @see timsort_check_invariants
 * @see timsort_collapse_runs
 */
TimsortRun*
timsort_merge_runs(void* arr, size_t size, int (*compare)(void*, void*), TimsortRun* frst, TimsortRun* scnd, TimsortMergeState* merge_state)
{
  char* arr_p = (char*)arr;
  if (frst->start > scnd->start) { 
    swap(frst, scnd, sizeof(TimsortRun)); 
  }

  int lo = bin_search(arr, size, compare, frst->start, frst->start + frst->len - 1, arr_p+(scnd->start*size));
  int hi = bin_search(arr, size, compare, scnd->start, scnd->start + scnd->len - 1, arr_p+((frst->start + frst->len - 1)*size));
  lo = lo >= 0 ? lo : frst->start;
  hi = hi >= 0 ? hi : scnd->start + scnd->len - 1;
  size_t frst_len_adj = frst->len - (lo - frst->start);
  size_t scnd_len_adj = hi - scnd->start + 1;

  size_t i, j, k;
  if (frst_len_adj < scnd_len_adj) {
    timsort_merge_runs_lo(arr, size, compare, lo, frst_len_adj, hi, scnd_len_adj, merge_state);
  } else {
    timsort_merge_runs_hi(arr, size, compare, lo, frst_len_adj, hi, scnd_len_adj, merge_state);
  }

  frst->len = frst->len + scnd->len;
  return frst;
}

/**
 * @brief Merge runs from left to right.
 *
 * Called when the leftmost run is smaller than the rightmost run.
 *
 * @param arr Target array.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower index bound of merge.
 * @param lo_len Length of smaller leftmost run.
 * @param hi Upper index bound of merge.
 * @param hi_len Length of larger rightmost run.
 * @param merge_state Struct containing information about merges and runs.
 * @retun Void.
 *
 * @see timsort
 * @see timsort_merge_runs
 */
void
timsort_merge_runs_lo(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t lo_len, size_t hi, size_t hi_len, TimsortMergeState* merge_state) 
{
  char* arr_p = (char*)arr;
  char* temp = malloc(size * lo_len);
  memcpy(temp, arr_p+(lo*size), lo_len * size);

  size_t k, l = 0, r = hi - hi_len + 1;
  for (k = lo; k <= hi; k++) {
    if (l < lo_len && (r > hi || compare(temp+(l * size), arr_p+(r * size)) < 0)) {
      memcpy(arr_p+(k * size), temp+(l * size), size);
      l++;
    } else {
      memcpy(arr_p+(k * size), arr_p+(r * size), size);
      r++;
    }
  }

  free(temp);
}

/**
 * @brief Merge runs from right to left.
 *
 * Called when the rightmost run is smaller than the leftmost run.
 *
 * @param arr Target array.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower index bound of merge.
 * @param lo_len Length of larger leftmost run.
 * @param hi Upper index bound of merge.
 * @param hi_len Length of smaller rightmost run.
 * @param merge_state Struct containing information about merges and runs.
 * @retun Void.
 *
 * @see timsort
 * @see timsort_merge_runs
 */
void
timsort_merge_runs_hi(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t lo_len, size_t hi, size_t hi_len, TimsortMergeState* merge_state) 
{
  char* arr_p = (char*)arr;
  char* temp = malloc(size * hi_len);
  memcpy(temp, arr_p+((hi - hi_len + 1) * size), hi_len * size);

  size_t k, l = lo + lo_len - 1, r = hi_len - 1;
  for (k = hi + 1; k --> lo;) {
    if (r >= 0 && (l < lo || compare(temp+(r * size), arr_p+(l * size)) > 0)) {
      memcpy(arr_p+(k * size), temp+(r * size), size);
      if (r > 0) {
        r--;
      }
    } else {
      memcpy(arr_p+(k * size), arr_p+(l * size), size);
      if (l > 0) {
        l--;
      }
    }
  }

  free(temp);
}

/**
 * @brief Find minimum run size to use in timsort.
 *
 * The minimum run size is given by the 6 most significant bits of the array's
 * length. Consequently, minrun will range between 32 and 64 inclusive, and 
 * any array with length < 64 will have minimum run size equal to their entire 
 * length.
 *
 * @param nelems Number of elements in array.
 * @return Size of minimum run.
 *
 * @see timsort
 */
size_t
timsort_minrun(size_t nelems)
{
	const size_t SIZE_T_SIZE = sizeof(nelems);
	const size_t MAX_BITS = SIZE_T_SIZE * CHAR_BIT;
	int lead_zeros;
	if (MAX_BITS == 32) {
		lead_zeros = __builtin_clzl(nelems);
	} else if (MAX_BITS == 64) {
		lead_zeros = __builtin_clzll(nelems);
	}
	// Indicates number of bits which will be shifted.
	int shifts = (MAX_BITS - lead_zeros) - 6;
	int i, pad = 0;
	for (i = 0; i < shifts; i++) {
			// If any bits not in 6 most significant are set, pad final result by 1.
			// The & operator selects only bits that share the same 'index'.
			// As such, this operation returns 1 iff the bit and index i in nelems is
			// set.
			if (nelems & (1 << i)) {
					pad = 1;
			}
			nelems = nelems >> 1;
	}
	return nelems + pad;
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

/**
 * @brief Reverse given array.
 *
 * @param arr Target array.
 * @param start Lower index bound.
 * @param end Upper index bound.
 * @param size Size of each element in array.
 * @return Void.
 */
void
reverse_array(void* arr, size_t start, size_t end, size_t size)
{
  if (start >= end) { 
    return;
  }
  char* arr_p = (char*)arr;
  while (start < end) {
    swap(arr_p+(start*size), arr_p+(end*size), size);
    start++;
    end--;
  }
}

/**
 * @brief Use binary search to find index of value within subarray of an array.
 *
 * @param arr Array to be searched.
 * @param size Size of each element in array.
 * @param compare Function to be used to compare elements.
 * @param lo Lower bound of subarray.
 * @param hi Upper bound of subarray.
 * @param target Item to search for in subarray.
 * @return Index of item if found, else -1.
 *
 */
int
bin_search(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi, void* target)
{
  char* arr_p = (char*)arr;
  int m, l = lo, r = hi;
  while (1) {
    if (r < l || (r < 0 || l < 0)) {
      return -1;
    } else {
      m = floor(l + ((r - l) / 2));
      if (compare(arr_p+(m*size), target) > 0) {
        l = m + 1;
      } else if (compare(arr_p+(m*size), target) < 0) {
        r = m - 1;
      } else {
        return m;
      }
    }
  }
}
