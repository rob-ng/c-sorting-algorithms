#include <stdlib.h>
#include <string.h>
#include "sorting.h"

void
insert_sort(void* arr, size_t nelems, size_t size, int (*compare)(void*,void*))
{
  insert_sort_partial(arr, size, compare, 0, nelems);
}

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

void
swap(void* a, void* b, size_t size)
{
  void* tmp = malloc(sizeof(size));

  memcpy(tmp, a, size);
  memcpy(a, b, size);
  memcpy(b, tmp, size);

  free(tmp);
}
