#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "sorting.h"

#define SIZE 10000

int
compareInts(void* a, void* b)
{
  return ((*(int*)a) - (*(int*)(b)));
}

int
compareChars(void* a, void* b)
{
  return strncmp((char*)a, (char*)b, 1);
}

int
main(int argc, char* argv[])
{
  srand(time(NULL));
  clock_t start, end;

  char arr[SIZE];
  int i;
  for (i = 0; i < SIZE; i++) {
    arr[i] = 'A' + (rand() % 26);
  }
  start = clock();
  //insert_sort_bin_partial(arr, sizeof(char), compareChars, 0, (sizeof(arr)/sizeof(char)) - 1);
  //quick_sort(arr, sizeof(arr)/sizeof(char), sizeof(char), compareChars);
  timsort(arr, sizeof(arr)/sizeof(char), sizeof(char), compareChars);
  end = clock();
  printf("\nElapsed: %f\n", (double)(end - start) / CLOCKS_PER_SEC);
  for (i = 0; i < SIZE; i++) {
    printf("%c ", arr[i]);
  }
  return 0;
}

