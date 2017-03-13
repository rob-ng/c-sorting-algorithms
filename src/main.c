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
 * @brief Size of atest arrays.
 */
#define SIZE 100000

/**
 * @brief Compare function for integers.
 */
int
compare_ints(void* a, void* b)
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
compare_chars(void* a, void* b)
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
      arr_char[i] = 'A' + (rand() % 26);
    }
    for (i = 0; i < SIZE; i++) {
      arr_int[i] = rand() % 10000;
    }
    for (i = SIZE; i --> 0;) {
      arr_int_rev[i] = i;
    }
    timsort(arr_char, sizeof(arr_char)/sizeof(char), sizeof(char), compare_chars);
    timsort(arr_int, sizeof(arr_int)/sizeof(int), sizeof(int), compare_ints);
    timsort(arr_int_rev, sizeof(arr_int)/sizeof(int), sizeof(int), compare_ints);
    for (i = 0; i < SIZE-1; i++) {
      assert(arr_char[i] <= arr_char[i+1]);
      assert(arr_int[i] <= arr_int[i+1]);
      assert(arr_int_rev[i] <= arr_int_rev[i+1]);
    }
  }
  return 0;
}

