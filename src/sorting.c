/** 
 * @file 
 * @brief Sorting implementation. 
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

#include <assert.h>

#include "sorting.h"
#include "stack.h"
#include "doxygen.h"

/**
 * @ingroup Timsort
 * @struct TimsortRun.
 * @brief Struct to represent run during Timsort.
 */
struct TimsortRun {
  size_t start; ///< Memory offset corresponding to start of run in array.
  size_t len; ///< Total size of run.
};

/**
 * @ingroup Timsort
 * @struct TimsortMergeState.
 * @brief Struct to represent merge state during Timsort.
 */
struct TimsortMergeState {
  TimsortRun* runs; ///< Array representing stack of runs.
  int nruns; ///< Current number of unmerged runs.
  size_t max_runs; ///< Max number of runs.
  int min_gallop; ///< Current galloping threshold.
  int galloping; ///< Current status of galloping mode ( 0: off, 1: on).
};

/**
 * @ingroup InsertionSort
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
 * @ingroup InsertionSort
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
 * @ingroup InsertionSort
 * @brief Sort generic array using binary insertion sort.
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in array.
 * @param size Size of each element in the array.
 * @param compare Function to compare elements.
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
 * @ingroup InsertionSort
 * @brief Sort generic contiguous subarray using binary insertion sort.
 *
 * Uses binary search to find proper index of each element in subarray. 
 * Once the index of an element has been found, the elements beginning at that 
 * index and ending before the current index of the element are shifted to the 
 * right by one to make room for the element's insertion.
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
  void* selected = malloc(size);
  
  size_t m;
  size_t l;
  size_t r;
  for (size_t i = lo + size; i <= hi; i += size) {
    memcpy(selected, arr_p+(i), size);
    for (l = lo - size, r = i; r > l + size;) {
      m = ((r + l) / 2 / size) * size;
      if (compare(selected, arr_p+(m)) < 0) {
        r = m;
      } else {
        l = m;
      }
    }
    memmove(arr_p+(r + size), arr_p+(r), i - r);
    memcpy(arr_p+(r), selected, size);
  }

  free(selected);
}

/**
 * @ingroup SelectionSort
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
 * @ingroup CombSort
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
 * @ingroup MergeSort
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
 * @ingroup MergeSort
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
  } else if (hi - lo <= (LENGTH_THRESHOLD * size)) {
    insert_sort_partial(aux, size, compare, lo, hi);
  } else {
    size_t mid = ((hi + lo) / 2 / size) * size;
    merge_sort_recursive(aux, arr, size, compare, lo, mid);
    merge_sort_recursive(aux, arr, size, compare, mid + size, hi);
    merge_sort_merge(arr, aux, size, compare, lo, mid, hi);
  }
}

/**
 * @ingroup MergeSort
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

/**
 * @ingroup QuickSort
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
 * @ingroup QuickSort
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
 * @ingroup QuickSort
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
 * @ingroup Timsort
 * @brief Sort generic array using Timsort.
 *
 * Timsort (developed by Tim Peters) is a hybrid stable sorting algorithm,
 * combining insertion sort and an optimized merge sort. 
 * The algorithm proceeds as follows:
 *
 * - If array to sort is shorter than 64 elements:
 *   -# Defer to insertion sort.
 *      - When array is this short, minrun will equal the length of the array.
 *        As such, timsort offers no benefit over insertion sort in this case.
 * - Otherwise
 *   -# Calculate the minimum run length.
 *      - Minimum run length is chosen such that (array length / minimum run)
 *      is equal to (or slightly less than) 2 to some power. Doing so
 *      ensures that merges remain balanced for random data (where most
 *      runs are likely to have length equal to the minimum run).
 *   -# Initialize a struct to represent merge state for the duration of the
 *      sort.
 *   -# Find (or create if necessary) runs of at least minrun length. All runs
 *      are sorted to be ascending if they aren't already. 
 *      - On finding a run, push it onto the runs stack. Once there, check
 *        that the run invariants still hold. If they do not, perform
 *        specific merge operations until they do.
 *        Maintaining run invariants ensures that the runs stack is kept
 *        small, that the sort is stable, and that runs are kept at similar
 *        sizes for more balanced merges.
 *   -# Merge the remaining runs in the runs stack into a single sorted run.
 *
 * @todo The runs array is much larger than it needs to be. Technically,
 * the number of unmerged runs shouldn't grow larger than log base phi of N,
 * where phi = (1 + sqrt(5))/2 (golden ratio). However, computing this for
 * large N is expensive, and so for the moment I've opted only to use (N /
 * minrun) + 1. This value is the maximum number of unmerged runs that could
 * exist in the array without merging.  
 *
 * @param arr Array to be sorted.
 * @param nelems Number of elements in array.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @return Void.
 *
 * @see timsort_minrun()
 * @see timsort_find_runs()
 * @see timsort_check_invariants()
 * @see timsort_collapse_runs()
 * @see binary_insert_sort()
 */
void 
timsort(void* arr, size_t nelems, size_t size, 
        int (*compare)(const void*, const void*))
{
  enum { TIMSORT_MIN_NELEMS = 64 };
  if (nelems < TIMSORT_MIN_NELEMS) {
    binary_insert_sort(arr, nelems, size, compare);
  } else {
    const size_t minrun = timsort_minrun(nelems);
    /*
     * By maintaining the run invariants, we ensure that the unmerged run
     * lengths produce a Fibonacci-esque series. Moreover, we can guarantee
     * that this series will grow faster than the Fibonacci numbers. Because of
     * this, the number of Fibonacci numbers which sum to roughly 'nelems' is 
     * guaranteed to be larger than the number of runs in an array of size 
     * 'nelems'. We can therefore use the below approximation of that value to 
     * cap the size of the runs array.
     *
     * NOTE: Problem with this is that computing log for large numbers is
     * expensive. So much so that it slows down function by noticeable amount.
     */
    const size_t max_runs = (nelems / minrun) + 1;
    TimsortRun runs[max_runs];
    memset(&runs, 0, (max_runs) * sizeof(TimsortRun));
    TimsortMergeState merge_state = { 
      runs,
      0,
      max_runs,
      MIN_GALLOP,
      0
    };
    timsort_find_runs(arr, nelems, size, compare, minrun, &merge_state);
    timsort_collapse_runs(arr, size, compare, &merge_state);
  }
}
/**
 * @ingroup Timsort
 * @brief Find runs of elements in the array.
 *
 * A run is a sequence of either either strictly descending or non-descending 
 * elements. Because for any sequence of elements {i, i+1}, either 
 * arr[i] > arr[i + 1] or arr[i] <= arr[i +1], a run is always occuring.
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
 * @param ms Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort()
 * @see timsort_check_invariants()
 */
void
timsort_find_runs(void* arr, size_t nelems, size_t size, 
                  int (*compare)(const void*, const void*), 
                  size_t minrun, TimsortMergeState* ms)
{
  char* arr_p = (char*) arr;
  size_t minrun_size = minrun * size;
  size_t nelems_size = nelems * size;

  // Result of comparison between previous element and current element.
  int vs_prev = -1; 
  // Result of comparison between current element and next element.
  int vs_next = -1; 
  // Indicate beginning of new run.
  int new_run = 1;

  for (size_t i = 0, max_size = nelems_size - size; i <= max_size; i += size) {
    vs_next = i < max_size ? compare(arr_p+(i), arr_p+(i + size)) : -999;
    if (i < max_size
        /*
         * Note: Previously only checked first condition. However, first
         * condition doesn't fully account for non-descending series (e.g.
         * going from strict increase to equality and back).
         */
        && (((vs_next == vs_prev) || (vs_next + vs_prev < 0)) || new_run)) {
      vs_prev = vs_next;
      new_run = 0;
    } else {
      ms->runs[ms->nruns].len = i - ms->runs[ms->nruns].start + size;
      if (vs_prev > 0) {
        reverse_array(arr, ms->runs[ms->nruns].start,
                      ms->runs[ms->nruns].start 
                      + ms->runs[ms->nruns].len - size, 
                      size);
      }
      if (ms->runs[ms->nruns].len < minrun_size) {
        ms->runs[ms->nruns].len = ms->runs[ms->nruns].start + minrun_size - size 
                                  >= nelems_size 
                                  ? nelems_size - ms->runs[ms->nruns].start 
                                  : minrun_size;
        i = ms->runs[ms->nruns].start + ms->runs[ms->nruns].len - size;
        insert_sort_partial(arr, size, compare, ms->runs[ms->nruns].start, 
                            (ms->runs[ms->nruns].start 
                             + ms->runs[ms->nruns].len - size));
      }
      ms->nruns++;
      timsort_check_invariants(arr, size, compare, ms);
      ms->runs[ms->nruns].start = i + size;
      new_run = 1;
    }
  }
}

/**
 * @ingroup Timsort
 * @brief Maintain run invariants to ensure stable, balanced merges.
 * 
 * Let X, Y and Z be the top 3 runs in the run stack, ordered from left to 
 * right. Then the following must hold:
 * 1. |X| > |Y| + |Z|
 * 2. |Y| > |Z|
 * If either invariant fails to hold, merge Y with smaller of X and Z and
 * push new merged value onto stack, maintaing order.
 *
 * @note If the runs stack contains only 2 runs, we still check that the second
 * invariant holds.
 *
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param ms Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort()
 * @see timsort_find_runs()
 */
void
timsort_check_invariants(void* arr, size_t size, 
                         int (*compare)(const void*, const void*), 
                         TimsortMergeState* ms)
{
  while (ms->nruns > 1) {
    int n = ms->nruns - 2;
    if (n >= 1 
        && (ms->runs[n - 1].len <= ms->runs[n].len + ms->runs[n + 1].len)) {
      if (ms->runs[n - 1].len < ms->runs[n + 1].len) {
        timsort_merge_runs(arr, size, compare, 
                           &ms->runs[n - 1], &ms->runs[n], ms);
        ms->runs[n] = ms->runs[n + 1];
        ms->nruns--;
      } else {
        timsort_merge_runs(arr, size, compare, 
                           &ms->runs[n], &ms->runs[n + 1], ms);
        ms->nruns--;
      }
    } else if (n >= 0 && (ms->runs[n].len <= ms->runs[n + 1].len)) {
      timsort_merge_runs(arr, size, compare, 
                         &ms->runs[n], &ms->runs[n + 1], ms);
      ms->nruns--;
    } else {
      break;
    } 
  }
}

/**
 * @ingroup Timsort
 * @brief Merge top two runs in run stack until only one run remains.
 *
 * Once all the runs have been merged, the array will be fully sorted.
 *
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param ms Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort()
 * @see timsort_merge_runs()
 */
void
timsort_collapse_runs(void* arr, size_t size, 
                      int (*compare)(const void*, const void*), 
                      TimsortMergeState* ms)
{
  while (ms->nruns > 1) {
    timsort_merge_runs(arr, size, compare, &ms->runs[ms->nruns - 2], 
                       &ms->runs[ms->nruns - 1], ms);
    ms->nruns--;
  }
}

/**
 * @ingroup Timsort
 * @brief Merge two consecutive runs.
 *
 * To optimize merges, the function first finds the locations of right[0] in
 * left[] (lo), and left[max] in right[] (hi). As runs are increasing, all 
 * values in left[] before 'lo' are smaller than all value in right[], and 
 * likewise all values in right above 'hi' are greater than all values in 
 * left[]. These values can be ignored during the merge.
 *
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param left Leftmost run. 
 * @param right Rightmost run.
 * @param ms Struct containing information about merges and runs.
 * @return Pointer to left run, now with length updated to include right's.
 *
 * @see timsort()
 * @see timsort_check_invariants()
 * @see timsort_collapse_runs()
 * @see timsort_merge_runs_lo()
 * @see timsort_merge_runs_hi()
 * @see bin_search_loc()
 */
void
timsort_merge_runs(void* arr, size_t size, 
                   int (*compare)(const void*, const void*), 
                   TimsortRun* left, TimsortRun* right, 
                   TimsortMergeState* ms)
{
  char* arr_p = (char*) arr;

  size_t lo = bin_search_loc(arr, size, compare, 
                             left->start, left->start + left->len - size, 
                             arr_p+(right->start));
  size_t hi = bin_search_loc(arr, size, compare, 
                             right->start, right->start + right->len - size, 
                             arr_p+(left->start + left->len - size));

  size_t left_len_adj = left->len - (lo - left->start);
  size_t right_len_adj = hi - right->start + size;

  if (left_len_adj < right_len_adj) {
    timsort_merge_runs_lo(arr, size, compare, lo, left_len_adj, 
                          hi, right_len_adj, ms);
  } else {
    timsort_merge_runs_hi(arr, size, compare, lo, left_len_adj, 
                          hi, right_len_adj, ms);
  }

  left->len = left->len + right->len;
  return;
}

/**
 * @ingroup Timsort
 * @brief Merge two consecutive runs from left to right.
 *
 * This function is called when the leftmost run is smaller than the rightmost 
 * run. There are two modes in which merges can be performed:
 *
 * Standard mode:
 * Standard merge sort where elements are compared one-to-one.
 * During this mode, we keep track of the "winning" array. If an array has
 * "won" more than the ms->min_gallop, we enter galloping mode.
 *
 * Galloping Mode (galloping right):
 * In galloping mode, merges are performed as a pair of operations:
 * -# Find the location of left[0] in right[]. Merge all values (slice1) in 
 *    right[] up to this point and then merge left[0].
 * -# Find the location of right[0] in left[]. Merge all values (slice2) in
 *    left[] up to this point and then merge right[0].
 * Note: The runs left[] and right[] are altered between these operations.
 *
 * Galloping mode lets us take advantage of subruns in data, and by performing
 * merges in bulk, can be quite efficient for certain types of data. However, 
 * galloping is not always effective. In particular, it is not at all efficient
 * with random data. 
 *
 * To account for this, we check after the above operations that slice1 and 
 * slice2 are both sufficiently large - at least as large as
 * ms->min_gallop * element_size. If this is the case, then galloping mode was
 * effective and we decrement ms->min_gallop to make subsequent returns to
 * galloping mode easier. If this wasn't the case, we exit galloping mode and
 * increment ms->min_gallop to make entering galloping mode harder. In this
 * way, the algorithm quickly reacts to data for which galloping is ill-suited
 * and limits its effect on performance.
 * 
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param lo Lower bound of merge (inclusive).
 * @param lo_len Length of smaller leftmost run.
 * @param hi Upper bound of merge (inclusive).
 * @param hi_len Length of larger rightmost run.
 * @param ms Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort()
 * @see timsort_merge_runs()
 * @see timsort_gallop_right()
 */
void
timsort_merge_runs_lo(void* arr, size_t size, 
                      int (*compare)(const void*, const void*), 
                      size_t lo, size_t lo_len, 
                      size_t hi, size_t hi_len, 
                      TimsortMergeState* ms) 
{
  char* arr_p = (char*) arr;
  char* temp = malloc(lo_len);
  memcpy(temp, arr_p+(lo), lo_len);

  const size_t min_gallop_size = ms->min_gallop * size;

  int l_won = 0;
  int r_won = 0;

  size_t slice1;
  size_t slice2;

  size_t l = 0;
  size_t r = hi - hi_len + size;

  for (size_t k = lo; k <= hi; k += size) {
    if (ms->galloping) {
      if (l < lo_len && r <= hi) {
        slice1 = timsort_gallop_right(arr, size, compare, r, hi, temp+(l));
        memmove(arr_p+(k), arr_p+(r), slice1);
        memcpy(arr_p+(k + slice1), temp+(l), size);
        k += slice1;
        l += size;
        r += slice1;

        // If any of these conditions hold, the next gallop operation will fail.
        // Hence we should exit.
        if (r > hi || l >= lo_len || k >= hi) {
          ms->galloping = 0;
          ms->min_gallop++;
          continue;
        }

        k += size;

        slice2 = timsort_gallop_right(temp, size, compare, l, lo_len - size, 
                                      arr_p+(r));
        memmove(arr_p+(k), temp+(l), slice2);
        memcpy(arr_p+(k + slice2), arr_p+(r), size);
        k += slice2;
        r += size;
        l += slice2;

        if (slice1 < min_gallop_size || slice2 < min_gallop_size) {
          ms->galloping = 0;
          ms->min_gallop++;
        } else {
          ms->min_gallop--;
        }
      } else {
        slice1 = (r <= hi) ? hi - r + size : 0;
        slice2 = (l < lo_len) ? lo_len - l : 0;
        memmove(arr_p+(k), arr_p+(r), slice1); 
        memmove(arr_p+(k), temp+(l), slice2);
        ms->galloping = 0;
        ms->min_gallop++;
        // Done with merge so break
        break;
      }
    } else {
      if (l < lo_len && (r > hi || compare(temp+(l), arr_p+(r)) < 0)) {
        memcpy(arr_p+(k), temp+(l), size);
        l += size;
        l_won++;
        r_won = 0;
      } else {
        memcpy(arr_p+(k), arr_p+(r), size);
        r += size;
        l_won = 0;
        r_won++;
      }
      if (l_won > ms->min_gallop || r_won > ms->min_gallop) {
        ms->galloping = 1;
        l_won = r_won = 0;
      }
    }
  }

  free(temp);
}

/**
 * @ingroup Timsort
 * @brief Gallop left to right to find slice of elements in source array less
 * than target.
 *
 * In order to find this slice, we need to find the number of elements in the
 * source array smaller than the target. Given that the source array
 * is sorted and ascendingr, it suffices to find where the target would be
 * located in the source array.
 *
 * To determine this location, we perform a pair of searches:
 * -# We first perform an exponential search to find the value k such that 
 *    base + ((2^(k-1) - 1) * size < target <= base + ((2^k - 1) * size).
 *    This condenses the range of values in which the target must lie.
 * -# We then perform binary search using this range.
 *
 * @note The slice is a memory offset corresponding to the total size of 
 * these elements.
 *
 * @param src Array to gallop over.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param base Initial offset to begin gallop.
 * @param limit Maximum offset for galloping (inclusive).
 * @param target Target element.
 * @return Total size of elements in source array less than target.
 *
 * @see timsort()
 * @see timsort_merge_runs_lo()
 */
size_t
timsort_gallop_right(void* src, size_t size, 
                     int (*compare)(const void*, const void*), 
                     size_t base, size_t limit, void* target)
{
  char* src_p = (char*) src;

  size_t slice = 0;
  if (compare(target, src_p+(base)) > 0) {
    size_t srch_lo = base;
    size_t srch_hi = base;
    // Power of 2 used for exponential search.
    size_t gallop_exp = 2;
    size_t gallop_ind;
    while (1) {
      srch_hi = base + ((gallop_exp - 1) * size);
      // Only srch_hi needs to be checked, as srch_lo contains previous value
      // of srch_hi which was valid.
      if (srch_hi > limit) {
        srch_hi = limit;
        break;
      }
      // Note: Because we assign srch_hi to srch_lo if the comparison fails, we
      // know compare(target, src_p+(srch_lo) > 0 for the next search.
      if (!(compare(target, src_p+(srch_hi)) <= 0)) {
        gallop_exp *= 2;
        srch_lo = srch_hi;
      } else {
        break;
      }
    }
    gallop_ind = bin_search_loc(src, size, compare, srch_lo, srch_hi, target);
    slice = gallop_ind - base;
    /*
     * The bin_search_loc() function can at most return the offset of the last
     * element of the array that it is searching. While this is fine if the 
     * target is less than or equal to the last element (as the slice need not 
     * include the final element), it means that the slice will be off by one 
     * if the target element is larger than all elements in the array
     * (as gallop_ind - slice can be at most array_size - size).
     * This conditional is needed to account for this possibility.
    */
    if (gallop_ind == limit && compare(target, src_p+(gallop_ind)) > 0) {
      slice += size;
    }
  }
  return slice;
}

/**
 * @ingroup Timsort
 * @brief Merge two consecutive runs from right to left.
 *
 * This function is called when the rightmost run is smaller than the leftmost 
 * run. There are two modes in which merges can be performed:
 *
 * Standard mode:
 * Standard merge sort where elements are compared one-to-one.
 * During this mode, we keep track of the "winning" array. If an array has
 * "won" more than the ms->min_gallop, we enter galloping mode.
 *
 * Galloping Mode (galloping left):
 * In galloping mode, merges are performed as a pair of operations:
 * -# Find the location of right[max] in left[]. Merge all values (slice1) in 
 *    left[] down to this point and then merge right[max].
 * -# Find the location of left[max] in right[]. Merge all values (slice2) in
 *    right[] down to this point and then merge left[max].
 * Note: The runs left[] and right[] are altered between these operations.
 *
 * Galloping mode lets us take advantage of subruns in data, and by performing
 * merges in bulk, can be quite efficient for certain types of data. However, 
 * galloping is not always effective. In particular, it is not at all efficient
 * with random data. 
 *
 * To account for this, we check after the above operations that slice1 and 
 * slice2 are both sufficiently large - at least as large as
 * ms->min_gallop * element_size. If this is the case, then galloping mode was
 * effective and we decrement ms->min_gallop to make subsequent returns to
 * galloping mode easier. If this wasn't the case, we exit galloping mode and
 * increment ms->min_gallop to make entering galloping mode harder. In this
 * way, the algorithm quickly reacts to data for which galloping is ill-suited
 * and limits its effect on performance.
 *
 * @param arr Array containing runs.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param lo Lower bound of merge (inclusive).
 * @param lo_len Length of larger leftmost run.
 * @param hi Upper bound of merge (inclusive).
 * @param hi_len Length of smaller rightmost run.
 * @param ms Struct containing information about merges and runs.
 * @return Void.
 *
 * @see timsort()
 * @see timsort_merge_runs()
 * @see timsort_gallop_left()
 */
void
timsort_merge_runs_hi(void* arr, size_t size, 
                      int (*compare)(const void*, const void*), 
                      size_t lo, size_t lo_len, 
                      size_t hi, size_t hi_len, 
                      TimsortMergeState* ms) 
{
  char* arr_p = (char*) arr;
  char* temp = malloc(hi_len);
  memcpy(temp, arr_p+(hi - hi_len + size), hi_len);

  const size_t min_gallop_size = ms->min_gallop * size;

  int l_won = 0;
  int r_won = 0;

  size_t slice1; 
  size_t slice2; 

  size_t l = lo + lo_len - size; 
  size_t r = hi_len - size;

  for (size_t k = hi; k >= lo && k <= hi; k -= size) {
    if (ms->galloping) {
      // Condition given that indices are size_t
      if (r <= hi && (l >= lo && l <= hi)) {
        slice1 = timsort_gallop_left(arr, size, compare, l, lo, temp+(r));
        // To avoid going out of bounds, check that slice is at least 1.
        if (slice1 > 0) {
          memmove(arr_p+(k - slice1 + size), arr_p+(l - slice1 + size), slice1);
        }
        memcpy(arr_p+(k - slice1), temp+(r), size);
        k -= slice1;
        r -= size;
        l -= slice1;

        // If any of these conditions hold, the next gallop operation will fail.
        // Hence we should exit.
        if ((r > hi) || (l < lo || l > lo) || (k <= lo || k > hi)) {
          ms->galloping = 0;
          ms->min_gallop++;
          continue;
        }

        k -= size;

        slice2 = timsort_gallop_left(temp, size, compare, r, 0, arr_p+(l));
        // To avoid going out of bounds, check that slice is at least 1.
        if (slice2 > 0) {
          memmove(arr_p+(k - slice2 + size), temp+(r - slice2 + size), slice2);
        }
        memcpy(arr_p+(k - slice2), arr_p+(l), size);
        k -= slice2;
        l -= size;
        r -= slice2;

        if (slice1 < min_gallop_size || slice2 < min_gallop_size) {
          ms->galloping = 0;
          ms->min_gallop++;
        } else {
          ms->min_gallop--;
        }
      } else {
        slice1 = (l >= lo && l <= hi) ? lo_len - l + size : 0;
        slice2 = (r <= hi) ? r + size : 0;
        memmove(arr_p+(k - slice1 + size), arr_p+(l - slice1 + size), slice1); 
        memmove(arr_p+(k - slice2 + size), temp+(r - slice2 + size), slice2);
        ms->galloping = 0;
        ms->min_gallop++;
        break;
      }
    } else {
      if (r <= hi 
          && ((l < lo || l > hi) || compare(temp+(r), arr_p+(l)) > 0)) {
        memcpy(arr_p+(k), temp+(r), size);
        r -= size;
        r_won++;
        l_won = 0;
      } else {
        memcpy(arr_p+(k), arr_p+(l), size);
        l -= size;
        r_won = 0;
        l_won++;
      }
      if (l_won > ms->min_gallop || r_won > ms->min_gallop) {
        ms->galloping = 1;
        l_won = r_won = 0;
      }
    }
  }

  free(temp);
}

/**
 * @ingroup Timsort
 * @brief Gallop right to left to find slice of elements in source array greater
 * than target.
 *
 * In order to find this slice, we need to find the number of elements in the
 * source array greater than the target. Given that the source array
 * is sorted and ascending, it suffices to find where the target would be
 * located in the source array.
 *
 * To determine this location, we perform a pair of searches:
 * -# We first perform an exponential search to find the value k such that 
 *    base - ((2^(k) - 1) * size < target <= base - ((2^(k-1) - 1) * size).
 *    This condenses the range of values in which the target must lie.
 * -# We then perform binary search using this range.
 *
 * @note The slice is a memory offset corresponding to the total size of 
 * these elements.
 *
 * @param src Array to gallop over.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param base Initial offset to begin gallop.
 * @param limit Maximum offset for galloping (inclusive).
 * @param target Target element.
 * @return Total size of elements in souce array greater than target.
 *
 * @see timsort()
 * @see timsort_merge_runs_hi()
 */
size_t 
timsort_gallop_left(void* src, size_t size, 
                    int (*compare)(const void*, const void*), 
                    size_t base, size_t limit, void* target) 
{
  char* src_p = (char*) src;

  size_t slice = 0;
  if (compare(target, src_p+(base)) < 0) {
    size_t srch_lo = base;
    size_t srch_hi = base;
    // A power of 2 to use in exponential search.
    size_t gallop_exp = 2; 
    size_t gallop_ind;
    while (1) {
      srch_lo = base - ((gallop_exp - 1) * size);
      // Only srch_lo needs to be checked, as srch_hi contains previous value
      // of srch_lo which was valid.
      if (srch_lo < limit || srch_lo > base) {
        srch_lo = limit;
        break;
      }
      // Note: Because we assign srch_lo to srch_hi if the comparison fails, we
      // know compare(target, src_p+(srch_hi) <= 0 for the next search.
      if (!(compare(target, src_p+(srch_lo)) > 0)) {
        gallop_exp *= 2;
        srch_hi = srch_lo;
      } else {
        break;
      }
    }
    gallop_ind = bin_search_loc(src, size, compare, srch_lo, srch_hi, target);
    slice = base - gallop_ind;
    /*
     * The bin_search_loc() function must return a non-negative offset. As
     * such, the size of slice is at most array_size - size. However, when the 
     * target is less than first value in the source array, the slice ought to
     * include all elements in the array and therefore be equal to array_size.
     * This conditional accounts for this.
     */
    if (compare(target, src_p+(gallop_ind)) < 0) {
      slice += size;
    }
  }
  return slice;
}

/**
 * @ingroup Timsort
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
 * @see timsort()
 */
size_t
timsort_minrun(size_t nelems)
{
  const size_t total_bits = sizeof(nelems) * CHAR_BIT;
  int lead_zeros = 0;
  if (total_bits == 32) {
    lead_zeros = __builtin_clzl(nelems);
  } else if (total_bits == 64) {
    lead_zeros = __builtin_clzll(nelems);
  } 
  assert(lead_zeros != 0);
  // Number of bits which will be shifted.
  int shifts = (total_bits - lead_zeros) - 6;
  size_t pad = 0;
  for (int i = 0; i < shifts; i++) {
    /*
     * If any bits not in 6 most significant are set, pad final result by 1. 
     * The & operator selects only bits that share the same 'index'. As such, 
     * this operation returns 1 iff the bit and index i in nelems is set.
     */
    if (nelems & (1 << i)) {
      pad = 1;
    }
    nelems = nelems >> 1;
  }
  return nelems + pad;
}

/**
 * @ingroup SortingHelper
 * @brief Swap the values referenced by two pointers.
 *
 * To speed up swaps, the function uses different strategies for standard typed
 * elements (e.g. elements whose size is at most 8 bytes (minimum size of long
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
  enum { max_size = 8 };
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
 * @ingroup SortingHelper
 * @brief Find median of three elements in given array and return its location.
 *
 * @note Location is given as memory offset.
 *
 * @note When attempting to find a pivot point (e.g. in quicksort), the median 
 * is a better option than using a fixed position (e.g. always first element).
 *
 * @param arr Array containing the elements.
 * @param size Size of each element.
 * @param a First element.
 * @param b Second element.
 * @param c Third element.
 * @param compare Function to compare elements.
 * @return Location of the median element in array.
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
 * @ingroup SortingHelper
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
  char* arr_p = (char*) arr;
  while (start < end) {
    swap(arr_p+(start), arr_p+(end), size);
    start += size;
    end -= size;
  }
}

/**
 * @ingroup SortingHelper
 * @brief Find location of target value in array using binary search.
 *
 * To account for duplicate values, the function performs a final linear search
 * once a location has been found until it finds the final duplicate.
 *
 * @note Location is given as a a memory offset. 
 *
 * @note In the case that the target element is not actually located in the 
 * array, the search returns where the target element would be located were it
 * in the array.
 *
 * @todo Use second binary search instead of linear search to find either first
 * or last instance of duplicate.
 *
 * @param arr Array to be searched.
 * @param size Size of each element in array.
 * @param compare Function to compare elements.
 * @param lo Lower bound for search (inclusive).
 * @param hi Upper bound for search (inclusive).
 * @param target Element to search for.
 * @return Location where element belongs in the array.
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
  int cmp;

  while (1) {
    if (r > hi || r <= l) {
      cmp = compare(target, arr_p+(l));
      if (cmp == 0) {
        return l;
      } else if (cmp < 0) {
        do {
          l -= size;
        } while ((l <= hi && l >= lo) && compare(target, arr_p+(l)) < 0);
        return (l >= hi || l < lo) ? lo : l + size;
      } else {
        do {
          l += size;
        } while (l <= hi && compare(target, arr_p+(l)) > 0);
        return l > hi ? hi : l;
      }
    }
    // Note that value in parantheses is truncated before being multiplied by
    // size.
    m = ((r + l) / 2 / size) * size;
    cmp = compare(target, arr_p+(m));
    if (cmp > 0) {
      l = m + size;
    } else if (cmp < 0) {
      r = m - size;
    } else {
      do {
        m += size;
      } while (m <= hi && compare(target, arr_p+(m)) == 0);
      return m > hi ? hi : m;
    }
  }
}

