/** 
 * @file 
 * @brief Main.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "sorting.h"

/** 
 * @def SIZE
 * @brief Size of test arrays.
 */
#define SIZE 100000

/**
 * @brief Compare function for integers.
 */
int
compare_ints(const void* a, const void* b)
{
  int diff = ((*(int*)a) - (*(int*)(b)));
  if (diff < 0) {
    return -1;
  } else if (diff > 0) {
    return 1;
  } else {
    return 0; 
  }
}

/**
 * @brief Compare function for chars.
 */
int
compare_chars(const void* a, const void* b)
{
  int diff = strncmp((char*)a, (char*)b, 1);
  if (diff < 0) {
    return -1;
  } else if (diff > 0) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * @brief Main function.
 */
int
main(int argc, char* argv[])
{
  srand(time(NULL));
  clock_t start, end;

  //char arr_char[SIZE];
  int arr_int_me[SIZE];
  int arr_int_cmp[SIZE];
  int rand_int = 0;
  //int arr_int_rev[SIZE];
  for (int sort_ind = 0; sort_ind < 1; sort_ind++) {
    for (int i = 0; i < SIZE; i++) {
      rand_int = rand() % 10000;
      arr_int_me[i] = rand_int;
      arr_int_cmp[i] = rand_int;
    }
    /*for (i = 0; i < SIZE; i++) {
      arr_char[i] = 'A' + (rand() % 26);
    }
    for (i = SIZE; i --> 0;) {
      arr_int_rev[i] = i;
    }*/
    clock_t cmp_start = clock();
    qsort(arr_int_cmp, sizeof(arr_int_cmp)/sizeof(int), sizeof(int),
          compare_ints);
    clock_t cmp_end = clock();
    printf("Elapsed: %f\n", (double) (cmp_end - cmp_start) / CLOCKS_PER_SEC);
    clock_t start = clock();
    quick_sort(arr_int_me, sizeof(arr_int_me)/sizeof(int), sizeof(int), 
                compare_ints);
    clock_t end = clock();
    printf("Elapsed: %f\n", (double) (end - start) / CLOCKS_PER_SEC);
    for (int i = 0; i < SIZE; i++) {
      assert(arr_int_me[i] == arr_int_cmp[i]);
    }
  }
  return 0;
}

