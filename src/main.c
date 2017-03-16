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
#define SIZE 50000

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

  char arr_char[SIZE];
  int arr_int[SIZE];
  int arr_int_rev[SIZE];
  int i, sort_ind;
  int wrong_int = 0, wrong_char = 0;
  for (sort_ind = 0; sort_ind < 1; sort_ind++) {
    for (i = 0; i < SIZE; i++) {
      arr_int[i] = rand() % 10000;
    }
    /*for (i = 0; i < SIZE; i++) {
      arr_char[i] = 'A' + (rand() % 26);
    }
    for (i = SIZE; i --> 0;) {
      arr_int_rev[i] = i;
    }*/
    clock_t start = clock();
    merge_sort(arr_int, sizeof(arr_int)/sizeof(int), sizeof(int), 
                compare_ints);
    clock_t end = clock();
    printf("Elapsed: %f\n", (double) (end - start) / CLOCKS_PER_SEC);
    for (i = 0; i < SIZE-1; i++) {
      assert(arr_int[i] <= arr_int[i+1]);
    }
  }
  return 0;
}

