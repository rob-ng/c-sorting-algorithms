#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "sorting.h"

#define SIZE 10000

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

int
compare_chars(void* a, void* b)
{
  return strncmp((char*)a, (char*)b, 1);
}

int
main(int argc, char* argv[])
{
  srand(time(NULL));
  clock_t start, end;

  char arr_char[SIZE];
  int arr_int[SIZE];
  int i;
  for (i = 0; i < SIZE; i++) {
    arr_char[i] = 'A' + (rand() % 26);
  }
  for (i = 0; i < SIZE; i++) {
    arr_int[i] = rand() % 10;
  }
  start = clock();
  // quick_sort(arr, sizeof(arr)/sizeof(char), sizeof(char), compareChars);
  timsort(arr_char, sizeof(arr_char)/sizeof(char), sizeof(char), compare_chars);
  timsort(arr_int, sizeof(arr_int)/sizeof(int), sizeof(int), compare_ints);
  end = clock();
  printf("\nElapsed: %f\n", (double)(end - start) / CLOCKS_PER_SEC);
  for (i = 0; i < SIZE; i++) {
    printf("%d ", arr_int[i]);
  }
  printf("\n");
  for (i = 0; i < SIZE; i++) {
    printf("%c ", arr_char[i]);
  }
  int j;
  for (i = 0; i < SIZE - 1; i++) {
    for (j = i+1; j < SIZE; j++) {
      assert(arr_char[i] <= arr_char[j]);
      assert(arr_int[i] <= arr_int[j]);
    }
  }
  return 0;
}

