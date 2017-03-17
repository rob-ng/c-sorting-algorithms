/** 
 * @file 
 * @brief Sorting implementation. 
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

#include "sorting.h"
#include "stack.h"
#include "doxygen.h"

/**
 * @struct TimsortRun.
 * @brief Struct to represent run during Timsort.
 */
struct TimsortRun {
  size_t start; ///< Start index of run.
  size_t len; ///< Length of run.
};

/**
 * @struct TimsortMergeState.
 * @brief Struct to represent merge state during Timsort.
 */
struct TimsortMergeState {
  Stack* runs_stack; ///< Stack containing discovered runs.
  TimsortRun* runs; ///< Array of initialized runs.
  size_t max_runs; ///< Max number of runs.
  int min_gallop; ///< Current galloping threshold.
  int galloping; ///< Whether or not in galloping mode.
};

/**
 * @addtogroup SortingAlgorithm
 * @{
 * @addtogroup SimpleSort
 * @{
 * @addtogroup InsertionSort
 * @{
 */

/**
 * @brief Sort generic array using insertion sort.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to compare elements.
 * @return Void.
 *
 * @see insert_sort_partial()
 */
void
insert_sort(void* arr, size_t nelems, size_t size, 
            int (*compare)(const void*, const void*))
{
  if (nelems == 0) {
    return;
  }
  insert_sort_partial(arr, size, compare, 0, (nelems - 1) * size);
}

/**
 * @brief Sort generic contiguous subarray using insertion sort.
 *
 * @param arr Array containing the subarray.
 * @param size Size of each element in the array.
 * @param compare Function to compare elements.
 * @param lo Lower bound of subarray (inclusive).
 * @param hi Upper bound of subarray (inclusive).
 * @return Void.
 */
void
insert_sort_partial(void* arr, size_t size, 
                    int (*compare)(const void*, const void*), 
                    size_t lo, size_t hi) 
{
  char* arr_p = (char*) arr;
  void* curr = malloc(size);
  size_t j;
  for (size_t i = lo + size; i <= hi; i += size) {
    memcpy(curr, arr_p+(i), size);
    j = i - size;
    while ((j >= lo && j <= hi) && compare(arr_p+(j), curr) > 0) {
      j -= size;
    }
    j = (j > hi) ? lo : j + size;
    memmove(arr_p+(j + size), arr_p+(j), i - j);
    memcpy(arr_p+(j), curr, size);
  }
  free(curr);
}

/**
 * @brief Sort generic array using binary insertion sort.
 *
 * @param arr Array containing the subarray.
 * @param size Size of each element in the array.
 * @param compare Function to compare elements.
 * @param lo Lower bound of subarray (inclusive).
 * @param hi Upper bound of subarray (inclusive).
 * @return Void.
 */
void 
binary_insert_sort(void* arr, size_t nelems, size_t size, 
                   int (*compare)(const void*, const void*)) 
{
  if (nelems == 0) {
    return;
  }
  binary_insert_sort_partial(arr, size, compare, 0, (nelems - 1) * size);
}


/**
 * @brief Sort generic contiguous subarray using binary insertion sort.
 *
 * Uses binary search (via bin_search_loc()) to find proper index of each
 * element in subarray. Once the index of an element has been found, 
 * the elements beginning at that index and ending before the current index of 
 * the element are shifted to the right by one to make room for the element's
 * insertion.
 *
 * @note As shifting elements involves overlapping memory, the function uses
 * memmove() rather than memcpy(). While memcpy() is typically faster than
 * memmove(), being able to copy memory in bulk via memmove() (rather than one 
 * at a time, as we would need to with memcpy() to prevent problems with 
 * overlapping memory) results in an enormous performance boost.  
 *
 * @param arr Array containing the subarray.
 * @param size Size of each element in the array.
 * @param compare Function to compare elements.
 * @param lo Lower bound of subarray (inclusive).
 * @param hi Upper bound of subarray (inclusive).
 * @return Void.
 */
void
binary_insert_sort_partial(void* arr, size_t size, 
                           int (*compare)(const void*, const void*), 
                           size_t lo, size_t hi)
{
  char* arr_p = (char*) arr;
  size_t loc;
  void* selected = malloc(size);
  
  for (size_t i = lo + size; i <= hi; i += size) {
    memcpy(selected, arr_p+(i), size);
    loc = bin_search_loc(arr, size, compare, lo, i - size, selected);
    memmove(arr_p+(loc + size), arr_p+(loc), i - loc);
    memcpy(arr_p+(loc), selected, size);
  }

  free(selected);
}

/** @} */ // End InsertionSort

/**
 * @addtogroup SelectionSort
 * @{
 */

/**
 * @brief Sort generic array using selection sort.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elemenets in the array.
 * @param size Size of each element in the array.
 * @param compare Function to compare elements.
 * @return Void.
 */
void
select_sort(void* arr, size_t nelems, size_t size, 
            int (*compare)(const void*, const void*))
{
  /**
   * @brief To avoid issues with size_t wrapping, and as an empty array is
   * sorted, return immediately if nelems is 0.
   */
  if (nelems == 0) { 
    return; 
  }
  char* arr_p = (char*) arr;
  size_t min_ind;
  for (size_t i = 0, max_i = ((nelems - 1) * size); i < max_i; i += size) {
    min_ind = i;
    for (size_t j = i + size, max_j = (max_i + size); j < max_j; j += size) {
      if (compare(arr_p+(j), arr_p+(min_ind)) < 0) {
        min_ind = j;
      }
    }
    if (min_ind != i) {
      swap(arr_p+(i), arr_p+(min_ind), size);
    }
  }
}

/** 
 * @}
 * @}
 */ // End SelectionSort, End SimpleSort 

/**
 * @addtogroup BubbleSort
 * @{
 * @addtogroup CombSort
 * @{
 */

/**
 * @brief Sort generic array using comb sort.
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
 * from left to right. As such, each such element can be moved left at most 1 
 * index per iteration. So if the smallest element where located at an index n,
 * n - 1 loop iterations would have to occur for it to be sorted.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in array.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @return Void.
 */
void
comb_sort(void* arr, size_t nelems, size_t size, 
          int (*compare)(const void*, const void*))
{
  char* arr_p = (char*) arr;
  const size_t MAX_MEM = nelems * size;
  const double SHRINK = 1.3 * size;

  size_t gap = MAX_MEM;
  size_t i = 0;
  int sorted = 0;

  while (!sorted) {
    gap = floor(gap / SHRINK) * size;
    if (gap > size) {
      sorted = 0;
    } else {
      gap = size;
      sorted = 1;
    }

    i = 0;
    while (i + gap < MAX_MEM) {
      if (compare(arr_p+(i), arr_p+(i + gap)) > 0) {
        swap(arr_p+(i), arr_p+(i + gap), size);
        sorted = 0;
      }
      i += size; 
    }
  }
}

/**
 * @}
 * @}
 */ // End CombSort, End BubbleSort

/**
 * @addtogroup EfficientSort
 * @{
 * @addtogroup MergeSort
 * @{
 */

/**
 * @brief Sort generic array using merge sort.
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
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 */
void
merge_sort(void* arr, size_t nelems, size_t size, 
           int (*compare)(const void*, const void*))
{
  if (nelems <= LENGTH_THRESHOLD) {
    insert_sort(arr, nelems, size, compare);
  } else {
    void* aux = malloc(size * nelems);
    memcpy(aux, arr, size * nelems);
    merge_sort_recursive(aux, arr, size, compare, 0, (nelems - 1) * size);
    free(aux);
  }
}

/**
 * @brief Recursively perform merge sort.
 *
 * Merge sort is only perfomed if current index interval [lo, hi] is
 * sufficiently large. If it isn't, insertion sort is used to sort the array in
 * that interval.
 *
 * Note that the merge_sort_sort() and merge_sort_merge() calls swap the order
 * of arr and aux. This ensures that the aux array (INTO which values are
 * copied from arr in merge) is the array FROM which values are copied in the 
 * next merge. See merge_sort() documentation for why this is done.
 *
 * @note The parameters 'hi' and 'lo' are multiples of 'size'. As such, 
 * calculating 'mid' is slightly more involved as it must also be a multiple 
 * of 'size'.
 *
 * @param arr Array from which values will be copied.
 * @param aux Array into which values will be copied.
 * @param size Size of each element in either array.
 * @param compare Function to compare elements.
 * @param lo Lower bound of subarray (inclusive).
 * @param hi Upper bound of subarray (inclusive).
 * @return Void.
 *
 * @see merge_sort()
 * @see merge_sort_merge()
 */
void
merge_sort_recursive(void* arr, void* aux, size_t size, 
                int (*compare)(const void*, const void*), 
                size_t lo, size_t hi)
{
  if (hi <= lo) {
    return;
  }// else if (hi - lo <= (LENGTH_THRESHOLD * size)) {
    //insert_sort_partial(aux, size, compare, lo, hi + 1);//+ size);
  //} 
  else {
    size_t mid = ((hi + lo) / 2 / size) * size;
    merge_sort_recursive(aux, arr, size, compare, lo, mid);
    merge_sort_recursive(aux, arr, size, compare, mid + size, hi);
    merge_sort_merge(arr, aux, size, compare, lo, mid, hi);
  }
}

/**
 * @brief Merge subarrays by updating aux with sorted values taken from arr.
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
merge_sort_merge(void* arr, void* aux, size_t size, 
                 int (*compare)(const void*, const void*), 
                 size_t lo, size_t mid, size_t hi)
{
  char* arr_p = (char*) arr;
  char* aux_p = (char*) aux;
  size_t i = lo;
  size_t j = mid + size;
  for (size_t k = lo; k <= hi; k += size) {
    if (i <= mid && (j > hi || compare(arr_p+(i), arr_p+(j)) < 0)) {
      memcpy(aux_p+(k), arr_p+(i), size);
      i += size;
    } else {
      memcpy(aux_p+(k), arr_p+(j), size);
      j += size;
    }
  }
}

/** @} */ // End MergeSort

/**
 * @addtogroup QuickSort
 * @{
 */

/**
 * @brief Sort generic array using quicksort.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in the array.
 * @param size Size of each element in the array.
 * @param compare Function to be used to compare elements.
 * @return Void.
 *
 * @see quick_sort_sort()
 */
void
quick_sort(void* arr, size_t nelems, size_t size, 
           int (*compare)(const void*, const void*))
{
  if (nelems == 0) {
    return;
  }
  quick_sort_recursive(arr, size, compare, 0, (nelems - 1) * size);
}

/**
 * @brief Recursively perform quicksort.
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
 *
 * @see quick_sort()
 * @see quick_sort_partition()
 */
void
quick_sort_recursive(void* arr, size_t size, 
                     int (*compare)(const void*, const void*), 
                     size_t lo, size_t hi)
{
  if (hi <= lo) {
    return;
  } else if (hi - lo <= LENGTH_THRESHOLD * size) {
    insert_sort_partial(arr, size, compare, lo, hi);
  } else {
    size_t pivot = quick_sort_partition(arr, size, compare, lo, hi);
    quick_sort_recursive(arr, size, compare, lo, pivot);
    quick_sort_recursive(arr, size, compare, pivot + size, hi);
  }
}

/**
 * @brief Partition subarray around pivot element.
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
 * @param compare Function to compare elements.
 * @param lo Lower bound of the subarray (inclusive).
 * @param hi Upper bound of the subarray (inclusive).
 * @return Final index of the pivot element.
 */
size_t
quick_sort_partition(void* arr, size_t size, 
                     int (*compare)(const void*, const void*), 
                     size_t lo, size_t hi)
{
  char* arr_p = (char*) arr;
  size_t mid = ((hi + lo) / 2 / size) * size;
  size_t pivot = median_three(arr, size, lo, mid, hi, compare);

  size_t left = lo - size, right = hi + size;
  while (1) {
    do {
      left += size;
    } while (compare(arr_p+(left), arr_p+(pivot)) < 0);

    do {
      right -= size;
    } while (compare(arr_p+(right), arr_p+(pivot)) > 0);

    if (left >= right) {
      return right;
    }

    swap(arr_p+(left), arr_p+(right), size);
    if (left == pivot || right == pivot) {
      pivot = (left == pivot) ? right : left;
    }
  }
}

/** 
 * @}
 * @}
 */ // End QuickSort, End EfficientSort

/**
 * @addtogroup HybridSort
 * @{
 * @addtogroup Timsort
 * @{
 */

/**
 * @brief Sort generic array using Timsort.
 *
 * Timsort (developed by Tim Peters) is a hybrid stable sorting algorithm.
 * Timsort uses a combination of insertion sort and merge sort to first
 * organize the array into roughly equal length runs of ascending elements and
 * then merge those runs into a single sorted run.
 * The algorithm proceeds through the following steps:
 *
 * - If array to sort is shorter than 64 elements:
 *   -# Defer to insertion sort.
 * - Otherwise
 *   -# Calculate the minimum run length.
 *        - Minimum run length is chosen such that (array length / minimum run)
 *          is equal to (or slightly less than) 2 to some power. Doing so
 *          ensures that merges remain balanced for random data (where most
 *          runs are likely to have length equal to the minimum run).
 *   -# Initialize a struct to represent merge state for the duration of the
 *      sort.
 *   -# Find (or create if necessary) runs of at least minrun length. All runs
 *      are sorted to be ascending if they aren't already. 
 *        - On finding a run, push it onto the runs stack and check to see 
 *          if run invariants still hold. Merge runs until they do.
 *   -# Collapse the remaining runs in the runs stack into a single sorted run.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in array.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @return Void.
 *
 * @see timsort_minrun
 * @see timsort_find_runs
 * @see timsort_check_invariants
 * @see timsort_collapse_runs
 */
void 
timsort(void* arr, size_t nelems, size_t size, 
        int (*compare)(const void*, const void*))
{
  const size_t MIN_NELEMS = 64;
  if (nelems < MIN_NELEMS) {
    //binary_insert_sort(arr, size, compare, 0, nelems - 1);
  } else {
    size_t minrun = timsort_minrun(nelems);
    const size_t MAX_RUNS = nelems / minrun;
    TimsortRun runs[MAX_RUNS];
    size_t run_ind;
    for (run_ind = 0; run_ind <= MAX_RUNS; run_ind++) {
      TimsortRun run = { 0, 0 };
      runs[run_ind] = run;
    }
    TimsortMergeState merge_state = { 
      stack_init(), 
      runs, 
      MAX_RUNS, 
      MIN_GALLOP, 
      0 
    };

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
 * @param arr Array to search for runs.
 * @param nelems Number of elements in array.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param minrun Minimum acceptable run length.
 * @param merge_state Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort
 * @see timsort_check_invariants
 */
void
timsort_find_runs(void* arr, size_t nelems, size_t size, 
                  int (*compare)(const void*, const void*), 
                  size_t minrun, TimsortMergeState* merge_state)
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
    if (new_run || (compare(arr_p+(i*size), arr_p+((i+1)*size)) 
                    == compare(arr_p+((i-1)*size), arr_p+(i*size)))) {
      if (new_run) {
        merge_state->runs[curr_run].start = i;
      }
      merge_state->runs[curr_run].len++;
      new_run = 0;
    } else {
      if (compare(arr_p+((i-1)*size), arr_p+(i*size)) < 0) {
        reverse_array(arr, merge_state->runs[curr_run].start, 
                      merge_state->runs[curr_run].start + 
                      merge_state->runs[curr_run].len - 1, 
                      size);
      }
      if (merge_state->runs[curr_run].len < minrun) {
        merge_state->runs[curr_run].len = merge_state->runs[curr_run].start + 
                                          minrun - 1 >= nelems 
                                          ? nelems - merge_state->runs[curr_run].start 
                                          : minrun;
        i = merge_state->runs[curr_run].start 
          + merge_state->runs[curr_run].len - 1;
        /*binary_insert_sort(arr, size, compare, 
                           merge_state->runs[curr_run].start, 
                           (merge_state->runs[curr_run].start 
                            + merge_state->runs[curr_run].len - 1));*/
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
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param merge_state Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort
 * @see timsort_check_invariants
 */
void
timsort_check_invariants(void* arr, size_t size, 
                         int (*compare)(const void*, const void*), 
                         TimsortMergeState* merge_state)
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
          stack_push(merge_state->runs_stack, 
                     timsort_merge_runs(arr, size, compare, y, x, merge_state));
        } else {
          // MERGE AND PUSH Y AND Z
          stack_push(merge_state->runs_stack, 
                     timsort_merge_runs(arr, size, compare, z, y, merge_state));
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
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param merge_state Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort
 * @see timsort_merge_runs
 */
void
timsort_collapse_runs(void* arr, size_t size, 
                      int (*compare)(const void*, const void*), 
                      TimsortMergeState* merge_state)
{
  while (merge_state->runs_stack->len > 1) {
    TimsortRun* second = (TimsortRun*)stack_pop_return(merge_state->runs_stack);
    TimsortRun* first = (TimsortRun*)stack_pop_return(merge_state->runs_stack);
    stack_push(merge_state->runs_stack, 
               timsort_merge_runs(arr, size, compare, first, second, 
                                  merge_state));
  }
}

/**
 * @brief Merge 2 consecutive runs.
 *
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param frst Leftmost run. 
 * @param scnd Rightmost run.
 * @param merge_state Struct containing information about merges and runs.
 * @return Pointer to first run, now with length updated to include second's.
 *
 * @see timsort
 * @see timsort_check_invariants
 * @see timsort_collapse_runs
 * @see timsort_merge_runs_lo
 * @see timsort_merge_runs_hi
 */
TimsortRun*
timsort_merge_runs(void* arr, size_t size, 
                   int (*compare)(const void*, const void*), 
                   TimsortRun* frst, TimsortRun* scnd, 
                   TimsortMergeState* merge_state)
{
  char* arr_p = (char*)arr;
  if (frst->start > scnd->start) { 
    swap(frst, scnd, sizeof(TimsortRun)); 
  }

  int lo = bin_search(arr, size, compare, frst->start, 
                      frst->start + frst->len - 1, 
                      arr_p+(scnd->start*size));
  int hi = bin_search(arr, size, compare, scnd->start, 
                      scnd->start + scnd->len - 1, 
                      arr_p+((frst->start + frst->len - 1)*size));
  lo = lo >= 0 ? lo : frst->start;
  hi = hi >= 0 ? hi : scnd->start + scnd->len - 1;
  size_t frst_len_adj = frst->len - (lo - frst->start);
  size_t scnd_len_adj = hi - scnd->start + 1;

  size_t i, j, k;
  if (frst_len_adj < scnd_len_adj) {
    timsort_merge_runs_lo(arr, size, compare, lo, frst_len_adj, 
                          hi, scnd_len_adj, merge_state);
  } else {
    timsort_merge_runs_hi(arr, size, compare, lo, frst_len_adj, 
                          hi, scnd_len_adj, merge_state);
  }

  frst->len = frst->len + scnd->len;
  return frst;
}

/**
 * @brief Merge runs from left to right.
 *
 * Called when the leftmost run is smaller than the rightmost run.
 * There are two modes in which merges can be performed:
 *
 * Standard mode:
 * Standard merge sort where elements are compared one-to-one.
 * During this mode, we keep track of the "winning" array. If an array has
 * "won" more than the merge_state->min_gallop, we enter galloping mode.
 *
 * Galloping Mode:
 * Let A and B be two runs, A the smaller run.
 * Let k be the index to merge into in the main array, l be the initial index
 * of A, and r the initial index of B.
 * We first find the location of A[l] in B. We then merge
 * values from B up to this index into the main array (the number of elements
 * is slice1). We then merge A[l]. We then increment k, l, and r to account 
 * for the merged elements. Next, we find the location of B[r] in A and 
 * perform a similar operation (number of elements from A is slice2). 
 * Again, we increment k, l and r to account  for the merged elements. 
 * After all of this, we check that slice1 and slice2 are both >=
 * merge_state->min_run. If so, we decrement merge_state->min_run to make
 * entering into galloping mode easier. If not, we increment
 * merge_state->min_run to make entering galloping mode harder and we exit out
 * of galloping mode.
 * 
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param lo Lower index bound of merge (inclusive).
 * @param lo_len Length of smaller leftmost run.
 * @param hi Upper index bound of merge (inclusive).
 * @param hi_len Length of larger rightmost run.
 * @param merge_state Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort
 * @see timsort_merge_runs
 * @see timsort_gallop_right
 */
void
timsort_merge_runs_lo(void* arr, size_t size, 
                      int (*compare)(const void*, const void*), size_t lo, size_t lo_len, 
                      size_t hi, size_t hi_len, TimsortMergeState* merge_state) 
{
  char* arr_p = (char*)arr;
  char* temp = malloc(size * lo_len);
  memcpy(temp, arr_p+(lo*size), lo_len * size);

  int winner = -1, prev_winner = -1, count = 0;
  size_t srch_lo, srch_hi;
  size_t slice1, slice2, gallop_ind, gallop_exp;

  size_t k, l = 0, r = hi - hi_len + 1;
  for (k = lo; k <= hi; k++) {
    if ((l < lo_len && r <= hi) && merge_state->galloping) {
      slice1 = timsort_gallop_right(arr, size, compare, r, hi, temp+(l * size));
      memcpy(arr_p+(k * size), arr_p+(r * size), (slice1) * size);
      memcpy(arr_p+((k + slice1) * size), temp+(l * size), size);
      k += slice1;
      l += 1;
      r += slice1;

      if (k >= hi || r > hi || l >= lo_len) {
        merge_state->galloping = 0;
        merge_state->min_gallop++;
        continue;
      }

      k++;

      slice2 = timsort_gallop_right(temp, size, compare, l, lo_len, 
                                    arr_p+(r * size));
      memcpy(arr_p+(k * size), temp+(l * size), (slice2) * size);
      memcpy(arr_p+((k + slice2) * size), arr_p+(r * size), size);
      k += slice2;
      r += 1;
      l += slice2;

      if (slice1 < merge_state->min_gallop 
          || slice2 < merge_state->min_gallop) {
        merge_state->galloping = 0;
        merge_state->min_gallop++;
      } else {
        merge_state->min_gallop--;
      }
    } else {
      if (l < lo_len 
          && (r > hi || compare(temp+(l * size), arr_p+(r * size)) < 0)) {
        memcpy(arr_p+(k * size), temp+(l * size), size);
        l++;
        winner = 0;
      } else {
        memcpy(arr_p+(k * size), arr_p+(r * size), size);
        r++;
        winner = 1;
      }

      if (prev_winner == -1 || prev_winner == winner) {
        count++;
        if (count >= merge_state->min_gallop) {
          merge_state->galloping = 1;
          count = 0;
          prev_winner = winner = -1;
          continue;
        }
      } else {
        count = 0;
      }
      prev_winner = winner;
    }
  }

  free(temp);
}

/**
 * @brief Gallop left->right to find number of elements in source array less
 * than target.
 *
 * @param src Array to gallop over.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param base Index to begin gallop.
 * @param limit Index limit for galloping.
 * @param target Target element.
 * @return Number of elements in source array less than target.
 *
 * @see timsort
 * @see timsort_merge_runs_lo
 */
int
timsort_gallop_right(void* src, size_t size, 
                     int (*compare)(const void*, const void*), int base, 
                     int limit, void* target)
{
  char* src_p = (char*)src;
  int srch_lo = 0, srch_hi = 0, gallop_exp = 1, gallop_ind = 0, slice = 0;
  if (compare(target, src_p+(base * size)) <= 0) {
    slice = 0;
  } else {
    while (1) {
      srch_lo = base + ((int)pow(2, gallop_exp - 1) - 1);
      srch_hi = base + ((int)pow(2, gallop_exp) - 1);
      if (srch_lo > limit || srch_hi > limit) {
        srch_lo = srch_lo > limit ? limit : srch_lo;
        srch_hi = limit;
        break;
      }
      if (!((compare(target, src_p+(srch_lo * size)) > 0) 
            && (compare(target, src_p+(srch_hi * size)) <= 0))) {
        gallop_exp++;
      } else {
        break;
      }
    }
    gallop_ind = timsort_binary_search(src, size, compare, 
                                       srch_lo, srch_hi, target);
    slice = gallop_ind - base;
    if (compare(target, src_p+(gallop_ind * size)) > 0) {
      slice++;
    }
  }
  return slice;
}

/**
 * @brief Merge runs from right to left.
 *
 * Called when the rightmost run is smaller than the leftmost run.
 *
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param lo Lower bound of merge (inclusive).
 * @param lo_len Length of larger leftmost run.
 * @param hi Upper bound of merge (inclusive).
 * @param hi_len Length of smaller rightmost run.
 * @param merge_state Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort
 * @see timsort_merge_runs
 * @see timsort_gallop_left
 */
void
timsort_merge_runs_hi(void* arr, size_t size, 
                      int (*compare)(const void*, const void*), size_t lo, size_t lo_len, 
                      size_t hi, size_t hi_len, TimsortMergeState* merge_state) 
{
  char* arr_p = (char*)arr;
  char* temp = malloc(size * hi_len);
  memcpy(temp, arr_p+((hi - hi_len + 1) * size), hi_len * size);

  int winner = -1, prev_winner = -1, count = 0;
  int srch_lo, srch_hi;
  int slice1, slice2, gallop_ind, gallop_exp;

  int k, l = lo + lo_len - 1, r = hi_len - 1;
  for (k = hi + 1; k --> lo;) {
    if ((r >= 0 && (l >= 0 && l >= lo)) && merge_state->galloping) {
      slice1 = timsort_gallop_left(arr, size, compare, l, lo, temp+(r * size));
      if (slice1 > 0) {
        memcpy(arr_p+((k - slice1 + 1) * size), arr_p+((l - slice1 + 1) * size),
               slice1 * size);
      }
      memcpy(arr_p+((k - slice1) * size), temp+(r * size), size);
      k -= slice1;
      r -= 1;
      l -= slice1;

      if (k <= lo || r < 0 || (l < 0 || l < lo)) {
        merge_state->galloping = 0;
        merge_state->min_gallop++;
        continue;
      }

      k--;

      slice2 = timsort_gallop_left(temp, size, compare, r, 0, arr_p+(l * size));
      if (slice2 > 0) {
        memcpy(arr_p+((k - slice2 + 1) * size), temp+((r - slice2 + 1) * size),
               slice2 * size);
      }
      memcpy(arr_p+((k - slice2) * size), arr_p+(l * size), size);
      k -= slice2;
      l -= 1;
      r -= slice2;

      if (slice1 < merge_state->min_gallop 
          || slice2 < merge_state->min_gallop) {
        merge_state->galloping = 0;
        merge_state->min_gallop++;
      } else {
        merge_state->min_gallop--;
      }
    } else {
      if (r >= 0 && ((l < 0 || l < lo) 
                     || compare(temp+(r * size), arr_p+(l * size)) > 0)) {
        memcpy(arr_p+(k * size), temp+(r * size), size);
        r--;
        winner = 0;
      } else {
        memcpy(arr_p+(k * size), arr_p+(l * size), size);
        l--;
        winner = 1;
      }
      if (prev_winner == -1 || prev_winner == winner) {
        count++;
        if (count >= merge_state->min_gallop) {
          merge_state->galloping = 1;
          count = 0;
          prev_winner = winner = -1;
          continue;
        }
      } else {
        count = 0;
      }
      prev_winner = winner;
    }
  }

  free(temp);
}

/**
 * @brief Gallop right->left to find number of elements in source array greater
 * than target.
 *
 * @param src Array to gallop over.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param base Index to begin gallop.
 * @param limit Index limit for galloping.
 * @param target Target element.
 * @return Number of elements in souce array greater than target.
 *
 * @see timsort
 * @see timsort_merge_runs_hi
 */
int 
timsort_gallop_left(void* src, size_t size, 
                    int (*compare)(const void*, const void*), 
                    int base, int limit, void* target) {
  char* src_p = (char*)src;
  int srch_lo = 0, srch_hi = 0, gallop_exp = 1, gallop_ind = 0, slice = 0;
  if (compare(target, src_p+(base * size)) >= 0) { 
    slice = 0;
  } else {
    while (1) {
      srch_lo = base - ((int)pow(2, gallop_exp) - 1);
      srch_hi = base - ((int)pow(2, gallop_exp - 1) - 1);
      if ((srch_lo < 0 || srch_lo < limit) 
          || (srch_hi < 0 || srch_hi < limit)) {
        srch_lo = limit;
        srch_hi = (srch_hi < 0 || srch_hi < limit) ? limit : srch_hi;
        break;
      }
      if (!((compare(target, src_p+(srch_lo * size)) > 0) 
            && (compare(target, src_p+(srch_hi * size)) <= 0))) {
        gallop_exp++;
      } else {
        break;
      }
    }
    gallop_ind = timsort_binary_search(src, size, compare, srch_lo, srch_hi, 
                                       target);
    slice = base - gallop_ind;
    if (compare(target, src_p+(gallop_ind * size)) < 0) {
      slice++;
    }
  }
  return slice;
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
    // The & operator selects only bits that share the same 'index'. As such, 
    // this operation returns 1 iff the bit and index i in nelems is set.
    if (nelems & (1 << i)) {
      pad = 1;
    }
    nelems = nelems >> 1;
  }
  return nelems + pad;
}

/**
 * @brief Find position of element within array using binary search.
 *
 * Used in Timsort when galloping. When this version of binary search fails to
 * find the element being search for, it just returns 'l'. Comparisons between
 * the target and the value at arr[l] are left to galloping functions.
 *
 * @param arr Array to be searched.
 * @param size Size of each element in array.
 * @param compare Function for comparing elements.
 * @param lo Lower index bound for searching.
 * @param hi Upper index bound for searching.
 * @param target Element to search for.
 * @return Index where element either is, or should be, located in the array.
 *
 * @see timsort
 * @see timsort_gallop_left
 * @see timsort_gallop_right
 */
int
timsort_binary_search(void* arr, size_t size, 
                      int (*compare)(const void*, const void*), 
                      size_t lo, size_t hi, void* target)
{
  char* arr_p = (char*)arr;
  int m, l = lo, r = hi;
  while (1) {
    if (r <= l) {
      return l;
    }
    m = floor(l + ((r - l) / 2));
    if (compare(target, arr_p+(m * size)) > 0) {
      l = m + 1;
    } else if (compare(target, arr_p+(m * size)) < 0) {
      r = m - 1;
    } else {
      return m + 1;
    }
  }
}

/**
 * @}
 * @}
 */ // End Timsort, End HybridSort

/**
 * @addtogroup SortingHelper
 * @{
 */

/**
 * @brief Swap the values referenced by two pointers.
 *
 * To speed up swaps, the function uses different strategies for standard typed
 * elements (e.g. elements whose size is at most 64 (minimum size of long
 * long)) and larger elements.
 *
 * @param a First pointer.
 * @param b Second pointer.
 * @param size Size of the values referenced by the pointers.
 * @return Void.
 */
void
swap(void* a, void* b, size_t size)
{
  enum { max_size = 64 };
  if (size <= max_size) {
    char tmp[size];
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
  } else {
    void* tmp = malloc(sizeof(size));
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
    free(tmp);
  }
}

/**
 * @brief Find median of three elements in given array and return its index.
 *
 * When attempting to find a pivot point (e.g. in quicksort), the median is a
 * better option than using a fixed position (e.g. always first element).
 *
 * @param arr Array containing the elements.
 * @param size Size of each element.
 * @param a First element.
 * @param b Second element.
 * @param c Third element.
 * @param compare Function to compare elements.
 * @return Index of the median element.
 */
size_t
median_three(void* arr, size_t size, size_t a, size_t b, size_t c, 
             int (*compare)(const void*, const void*))
{
  char* arr_p = (char*) arr;
  if (compare(arr_p+(a), arr_p+(b)) > 0) {
    if (compare(arr_p+(b), arr_p+(c)) > 0) {
      return b;
    } else if (compare(arr_p+(a), arr_p+(c)) > 0) {
      return c;
    } else {
      return a;
    }
  } else {
    if (compare(arr_p+(a), arr_p+(c)) > 0) {
      return a;
    } else if (compare(arr_p+(b), arr_p+(c)) > 0) {
      return c;
    } else {
      return b;
    }
  }
}

/**
 * @brief Reverse given array.
 *
 * @param arr Array to reverse.
 * @param start Lower index bound (inclusive).
 * @param end Upper index bound (inclusive).
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
 * @brief Search for an element using binary search in a contiguous subarray.
 *
 * @param arr Array containing the subarray.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param lo Lower bound of subarray (inclusive).
 * @param hi Upper bound of subarray (inclusive).
 * @param target Item to search for.
 * @return Index of item if found, else -1.
 *
 */
int
bin_search(void* arr, size_t size, int (*compare)(const void*, const void*), 
           size_t lo, size_t hi, void* target)
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

/**
 * @brief Find location of target value in array using binary search.
 *
 * @note In the case that the target element is not actually located in the 
 * array, the search returns where the target element would be located were it
 * in the array.
 *
 * @param arr Array to be searched.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param lo Lower bound for search (inclusive).
 * @param hi Upper bound for search (inclusive).
 * @param target Element to search for.
 * @return
 */
size_t
bin_search_loc(void* arr, size_t size, 
                int (*compare)(const void*, const void*),
                size_t lo, size_t hi, void* target)
{
  char* arr_p = (char*) arr;
  size_t l = lo;
  size_t m = 0;
  size_t r = hi;

  while (1) {
    if (r > hi || r <= l) {
      return compare(target, arr_p+(l)) > 0 ? (l + size) : l;
    }
    m = ((r + l) / 2 / size) * size;
    if (compare(target, arr_p+(m)) > 0) {
      l = m + size;
    } else if (compare(target, arr_p+(m)) < 0) {
      r = m - size;
    } else {
      return m + size;
    }
  }
}
/** 
 * @}
 * @}
 */ // End SortingHelper, End SortingAlgorithm

