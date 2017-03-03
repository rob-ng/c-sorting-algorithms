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
 */
void
insert_sort_bin_partial(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi)
{
  char* arr_p = (char*)arr;
  int i, j, loc;
  void *slctd = malloc(size);

  for (i = lo + 1; i <= hi; i++) {
    j = i - 1;
    memcpy(slctd, arr_p+(i*size), size);

    loc = bin_search(arr, size, compare, lo, j, slctd);

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
 */
void 
timsort(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*))
{
  const size_t MIN_NELEMS = 64;
  if (nelems < MIN_NELEMS) {
    insert_sort(arr, nelems, size, compare);
  } else {
    size_t minrun = timsort_minrun(nelems);
    timsort_find_runs(arr, nelems, size, compare, minrun);
  }
}
/**
 * @brief Find runs in array and merge them.
 *
 * BE VERY VERY VERY CAREFUL ABOUT BOUNDS FOR RUNS!!!!!!!!!!!!!!!
 */
void
timsort_find_runs(void* arr, size_t nelems, size_t size, int (*compare)(void*, void*), size_t minrun)
{
  char* arr_p = (char*)arr;

  // As all runs must be at least minrun long, max runs occurs when every run
  // is exactly minrun elements long.
  const size_t MAX_RUNS = nelems / minrun;

  // Array to contain runs. 
  TimsortRun runs[MAX_RUNS];

  // Initialize runs.
  size_t run_ind;
  for (run_ind = 0; run_ind <= MAX_RUNS; run_ind++) {
    TimsortRun run;
    run.start = 0;
    run.len = 0;
    runs[run_ind] = run;
  }

  // Create stack for holding completed runs.
  Stack* runs_stack = stack_init();

  // The curr_run value is the index of the run in 'runs' array. When a run
  // terminates, this value is incremented and the next run begins.
  size_t i, curr_run = 0;
  int run_change = 0;

  // Iterate through the array and look for runs. Runs occur either when
  // consecutive values are strictly descending (i+1 < i) or non-descending
  // (i+1 >= i). Because one of these relationships must always be true, a run
  // is always occuring. 
  // While a run is happening, we only increment its end index.
  // When a run terminates, we must do a few things:
  // 1. Set 'descending' to either 1 or 0 depending on what the new
  //    relationship is.
  // 2. Check end index of the run. If the length of the run is less than the
  //    minimum run, we extend its len to minrun.
  //    To ensure that the new run is sorted, with use insert_sort_partial() on
  //    the new elements.
  //    We then reassign i to the end index of the run.
  // 3. Push current completed run onto runs stack.
  // 4. Increment curr_run and update its start and len values.
  int new_run = 1;
  for (i = 0; i < nelems-1; i++) {
    // Descending
    if (compare(arr_p+(i*size), arr_p+((i+1)*size)) > 0) {
      // If just starting or continuing strict descent.
      if (new_run || compare(arr_p+((i-1)*size), arr_p+(i*size)) > 0) {
        if (new_run) {
          runs[curr_run].start = i;
        }
        runs[curr_run].len++;
        new_run = 0;
      } else {
        if (runs[curr_run].len < minrun) {
          runs[curr_run].len = minrun;
          if (runs[curr_run].start + runs[curr_run].len - 1 >= nelems) {
            runs[curr_run].len = nelems - runs[curr_run].start;
            i = nelems - 1;
          } else {
            i = runs[curr_run].start + runs[curr_run].len - 1;
          }
          insert_sort_partial(arr, size, compare, runs[curr_run].start, (runs[curr_run].start + runs[curr_run].len));
        }
        stack_push(runs_stack, &runs[curr_run]);
        curr_run++;
        if (curr_run > MAX_RUNS) { break; }
        new_run = 1;
      }
    } else {
      // If just starting or continuing non-descent.
      if (new_run || compare(arr_p+((i-1)*size), arr_p+(i*size)) <= 0) {
        if (new_run) {
          runs[curr_run].start = i;
        }
        runs[curr_run].len++;
        new_run = 0;
      } else {
        if (runs[curr_run].len < minrun) {
          reverse_array(arr, runs[curr_run].start, runs[curr_run].start + runs[curr_run].len - 1, size);
          runs[curr_run].len = minrun;
          if (runs[curr_run].start + runs[curr_run].len - 1 >= nelems) {
            runs[curr_run].len = nelems - runs[curr_run].start;
            i = nelems - 1;
          } else {
            i = runs[curr_run].start + runs[curr_run].len - 1;
          }
          insert_sort_partial(arr, size, compare, runs[curr_run].start, (runs[curr_run].start + runs[curr_run].len));
        }
        stack_push(runs_stack, &runs[curr_run]);
        curr_run++;
        if (curr_run > MAX_RUNS) { break; }
        new_run = 1;
      }
    }

    // Check if runs_stack contains at least 3 runs.
    // If it does, check that for top 3 runs X, Y, and Z, the following
    // invariants hold:
    // 1. |X| > |Y| + |Z|
    // 2. |Y| > |Z|
    // If either invariant fails to hold, merge Y with smaller of X and Z and
    // push new merged value onto stack.
    while (1) {
      if (runs_stack->len >= 3) {
        TimsortRun* x = (TimsortRun*)stack_peek(runs_stack);
        stack_pop(runs_stack);
        TimsortRun* y = (TimsortRun*)stack_peek(runs_stack);
        stack_pop(runs_stack);
        TimsortRun* z = (TimsortRun*)stack_peek(runs_stack);
        stack_pop(runs_stack);

        if ((x->len < y->len + z->len) || (y->len < z->len)) {
          TimsortRun* merged_run;
          if (x->len < z->len) {
            // PUSH Z BACK ONTO STACK
            stack_push(runs_stack, z);
            // MERGE AND PUSH X AND Y
            merged_run = timsort_merge_runs(arr, size, compare, y, x);
            stack_push(runs_stack, merged_run);
          } else {
            // MERGE AND PUSH Y AND Z
            merged_run = timsort_merge_runs(arr, size, compare, z, y);
            stack_push(runs_stack, merged_run);
            // PUSH X BACK ONTO STACK
            stack_push(runs_stack, x);
          }
        } else {
          stack_push(runs_stack, z);
          stack_push(runs_stack, y);
          stack_push(runs_stack, x);
          break;
        }
      } else {
        break;
      }
    }
  }

  while (runs_stack->len > 1) {
    // Merge top 2 arrays until runs_stack contains just the sorted array.
    TimsortRun* second = stack_peek(runs_stack);
    stack_pop(runs_stack);
    TimsortRun* first = stack_peek(runs_stack);
    stack_pop(runs_stack);
    TimsortRun* merged_run = timsort_merge_runs(arr, size, compare, first, second);
    stack_push(runs_stack, merged_run);
  }

  stack_free(runs_stack);
}

/**
 * @brief Merge 2 consecutive runs.
 */
TimsortRun*
timsort_merge_runs(void* arr, size_t size, int (*compare)(void*, void*), TimsortRun* frst, TimsortRun* scnd)
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

  char* temp = NULL;
  size_t i, j, k;
  if (frst_len_adj < scnd_len_adj) {
    temp = malloc(size * frst_len_adj);
    memcpy(temp, arr_p+(lo*size), frst_len_adj*size);
    i = 0, j = scnd->start;
    for (k = lo; k <= hi; k++) {
      if (i < frst_len_adj && (j > hi || compare(temp+(i*size), arr_p+(j*size)) < 0)) {
        memcpy(arr_p+(k*size), temp+(i*size), size);
        i++;
      } else {
        memcpy(arr_p+(k*size), arr_p+(j*size), size);
        j++;
      }
    }
  } else {
    temp = malloc(size * scnd_len_adj);
    memcpy(temp, arr_p+(scnd->start*size), scnd_len_adj*size);
    i = scnd_len_adj - 1, j = frst->start + frst->len - 1;
    for (k = hi + 1; k --> lo;) {
      if (i >= 0 && (j < lo || compare(temp+(i*size), arr_p+(j*size)) > 0)) {
        memcpy(arr_p+(k*size), temp+(i*size), size);
        if (i != 0) { 
          i--; 
        }
      } else {
        memcpy(arr_p+(k*size), arr_p+(j*size), size);
        if (j != 0) {
          j--;
        }
      }
    }
  }

  free(temp);
  frst->len = frst->len + scnd->len;
  return frst;
   
/*

  TimsortRun* sml;
  TimsortRun* lrg;
  if (a->len < b->len) {
    sml = a;
    lrg = b;
  } else {
    sml = b;
    lrg = a;
  }
  size_t sml_end = sml->start + sml->len - 1;
  size_t lrg_end = lrg->start + lrg->len - 1;
  size_t sml_adj_len, lrg_adj_len;

  int lo, hi;
  if (sml->start < lrg->start) {
    // Location of first element of lrg in sml
    lo = bin_search(arr, sml->start, sml_end, size, arr_p+(lrg->start*size), compare);
    // Location of last element of sml in lrg
    hi = bin_search(arr, lrg->start, lrg_end, size, arr_p+(sml_end*size), compare);
    if (lo == -1) { lo = sml->start; }
    if (hi == -1) { hi = lrg_end; }
    sml_adj_len = sml->len - (lo - sml->start);
    lrg_adj_len = lrg->len - (hi - lrg->start);
  } else {
    // Location of first element of sml in lrg
    lo = bin_search(arr, lrg->start, lrg_end, size, arr_p+(sml->start*size), compare);
    // Location of last element of lrg in sml
    hi = bin_search(arr, sml->start, sml_end, size, arr_p+(lrg_end*size), compare);
    if (lo == -1) { lo = lrg->start; }
    if (hi == -1) { hi = sml_end; }
    sml_adj_len = sml->len - (hi - sml->start);
    lrg_adj_len = lrg->len - (lo - lrg->start);
  }

  //void* temp_p = malloc(size * (sml->len));
  //memcpy(temp_p, arr_p + (size*(sml->start)), size * (sml->len));
  //char* temp = (char*)temp_p;
  char* temp = malloc(size * sml_adj_len);
  if (sml->start < lrg->start) {
    memcpy(temp, arr_p + (size *  lo), size * sml_adj_len);
  } else {
    memcpy(temp, arr_p + (size *  hi), size * sml_adj_len);
  }

  size_t i, j, k;
  if (sml->start < lrg->start) {
    i = 0, j = lrg->start;
    for (k = sml->start; k <= lrg_end; k++) {
      if (i < sml->len && (j > lrg_end || compare(temp+(i*size), arr_p+(j*size)) < 0)) {
        memcpy(arr_p+(k*size), temp+(i*size), size);
        i++;
      } else {
        memcpy(arr_p+(k*size), arr_p+(j*size), size);
        j++;
      }
    }

    i = 0, j = lrg->start;
    for (k = lo; k < hi; k++) {
      if (i < sml_adj_len && (j > hi || compare(temp+(i*size), arr_p+(j*size)) < 0)) {
        memcpy(arr_p+(k*size), temp+(i*size), size);
        i++;
      } else {
        memcpy(arr_p+(k*size), arr_p+(j*size), size);
        j++;
      }
    }
  } else {
    i = hi, j = lrg_end;
    for (k = hi; k --> lo;) {
      if (i >= 0 && (j < lo || compare(temp+(i*size), arr_p+(j*size)) > 0)) {
        memcpy(arr_p+(k*size), temp+(i*size), size);
        if (i != 0) { 
          i--; 
        }
      } else {
        memcpy(arr_p+(k*size), arr_p+(j*size), size);
        if (j != 0) {
          j--;
        }
      }
    }
    i = sml->len-1, j = lrg_end;
    for (k = sml_end + 1; k --> lrg->start;) {
      if (i >= 0 && (j < lrg->start || compare(temp+(i*size), arr_p+(j*size)) > 0)) {
        memcpy(arr_p+(k*size), temp+(i*size), size);
        if (i != 0) { 
          i--; 
        }
      } else {
        memcpy(arr_p+(k*size), arr_p+(j*size), size);
        if (j != 0) {
          j--;
        }
      }
    }
  }

  free(temp);
  lrg->start = sml->start < lrg->start ? sml->start : lrg->start;
  lrg->len = sml->len + lrg->len;
  return lrg;
 
  */
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
 * @brief Find index of value within an array using binary search.
 */
int
bin_search(void* arr, size_t size, int (*compare)(void*, void*), size_t lo, size_t hi, void* target)
{
  char* arr_p = (char*)arr;
  int m, l = lo, r = hi;
  while (1) {
    if (r <= l) {
      return (compare(target, arr_p+(l*size)) > 0) ? (l + 1) : l;
    }

    m = floor(l + ((r - l) / 2));

    if (compare(arr_p+(m*size), target) < 0) {
      l = m + 1;
    } else if (compare(arr_p+(m*size), target) > 0) {
      r = m - 1;
    } else {
      return m + 1;
    }
  }
}
