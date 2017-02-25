#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sorting.h"

#define SIZE 10

int
compareInts(void* a, void* b)
{
  return ((*(int*)a) - (*(int*)(b)));
}

int
main(int argc, char* argv[])
{
  srand(time(NULL));

  int arr[SIZE];
  int i;
  for (i = 0; i < SIZE; i++) {
    arr[i] = rand() % 10;
  }
  insert_sort(arr, sizeof(arr)/sizeof(int), sizeof(int), compareInts);
  for (i = 0; i < SIZE; i++) {
    printf("%d ", arr[i]);
  }
  return 0;
}

