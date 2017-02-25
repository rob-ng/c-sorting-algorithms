#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sorting.h"

#define SIZE 10000

int
compareInts(void* a, void* b)
{
  return ((*(int*)a) - (*(int*)(b)));
}

int
main(int argc, char* argv[])
{
  srand(time(NULL));
  clock_t start, end;

  int arr[SIZE];
  int i;
  for (i = 0; i < SIZE; i++) {
    arr[i] = rand() % 10;
  }
  start = clock();
  quick_sort(arr, sizeof(arr)/sizeof(int), sizeof(int), compareInts);
  end = clock();
  printf("Elapsed: %f", (double)(end - start) / CLOCKS_PER_SEC);
  for (i = 0; i < SIZE; i++) {
    printf("%d ", arr[i]);
  }
  return 0;
}

