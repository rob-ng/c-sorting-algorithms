#include <stdio.h>
#include "minunit.h"
#include "../src/stack.h"
#include "../src/sorting.h"

int tests_run = 0;

static char* test_stack_init() {
  Stack* stack = stack_init();
  mu_assert("stack_init: stack should be initialized correctly", stack->head == NULL && stack->len == 0);
  return 0;
}

static char* test_stack_push_empty_stack() {
  Stack* stack = stack_init();
  int a = 3;
  stack_push(stack, &a);
  mu_assert("stack_push: stack should set head to pushed element.", stack->head->data == &a);
  return 0;
}

static char* test_stack_push_nonempty_stack() {
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  mu_assert("stack_push: stack should set head to pushed element.", stack->head->data == &b);
  return 0;
}

static char* test_stack_pop_empty_stack() {
  Stack* stack = stack_init();
  mu_assert("stack_pop: should return 0", stack_pop(stack) == 0);
  return 0;
}

static char* test_stack_pop_nonempty_stack() {
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  mu_assert("stack_pop: should update head and return 1", stack_pop(stack) == 1 && stack->head->data == &a);
  return 0;
}

static char* test_stack_pop_return_nonempty_stack() {
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  mu_assert("stack_pop_return: should update head and return 1", stack_pop_return(stack) == &b && stack->head->data == &a);
  return 0;
}

static char* test_stack_free() {
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  stack_free(&stack);
  mu_assert("stack_free: stack pointer should be NULL", stack == NULL);
  return 0;
}

int compare(void* a, void* b)
{
  int a_int = *((int*)a);
  int b_int = *((int*)b);
  if (a_int < b_int) {
    return -1;
  } else if (b_int < a_int) {
    return 1;
  } else if (a_int == b_int) {
    return 0;
  } else {
    return -999;
  }
}

static char* test_merge_runs_lo_gallop()
{
  int size = 200;
  int arr[200];
  int j;
  for (j = 0; j < 50; j++) {
    arr[j] = j;
  }
  for (j = 50; j < 200; j++) {
    arr[j] = j / 5;
  }
  for (j = 0; j < 200; j++) {
    printf("%d ", arr[j]);
  }
  printf("\n");
  int* arr_p = arr;
  int lo = 0;
  int lo_len = 50;
  int hi = 199;
  int hi_len = 150;
  /*int size = 14;
  int arr[14] = { 1, 3, 5, 7, 9, 11, 13, 15, 6, 8, 10, 12, 14, 16 };
  int* arr_p = arr;
  int lo = 0;
  int lo_len = 8;
  int hi = 13;
  int hi_len = 6;
  int size = 13;
  int arr[13] = { 5, 6, 6, 1, 2, 2, 3, 7, 1, 3, 9, 9, 10 };
  int* arr_p = arr;
  int lo = 3;
  int lo_len = 5;
  int hi = 12;
  int hi_len = 5;*/
  TimsortMergeState state = { 0 };
  state.galloping = 1;
  state.min_gallop = 0;
  timsort_merge_runs_lo(&arr, sizeof(int), compare, lo, lo_len, hi, hi_len, &state);
  int i;
  for (i = 0; i < size; i++) {
    printf("%d ", arr[i]);
  }
  return 0;
}

static char* test_merge_runs_hi_gallop()
{
  int size = 200;
  //int arr[14] = { 0, 1, 2, 3, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6};
  // int arr[14] = { 7, 8, 9, 10, 11, 12, 13, 14, 0, 1, 2, 3, 4, 5 };
  //int arr[14] =   { 3, 3, 3, 3, 3, 3, 1, 2, 3, 2, 2, 2, 2, 2};
  // int arr[14] = { 2, 4, 6, 8, 10, 12, 14, 16, 1, 3, 5, 7, 9, 11 };
  // int arr[9] = { 2, 4, 6, 8, 10, 12, 5, 7, 9 };
  // int arr[14] = { 1, 3, 5, 7, 9, 11, 13, 15, 6, 8, 10, 12, 14, 16 };
  //int arr[14] = { 1, 1, 1, 1, 1, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
  //int arr[13] = { 2, 2, 2, 3, 4, 4, 6, 1, 1, 2, 2, 3, 3 };
  // int arr[12] = { 2, 3, 4, 5, 5, 5, 1, 1, 2, 2, 3, 6 };
  // int arr[13] = { 5, 6, 6, 1, 2, 2, 3, 7, 1, 3, 9, 9, 10 };
  //int arr[14] = { 5, 5, 5, 4, 4, 4, 3, 3, 3, 1, 1, 1, 2, 2 };

  int arr[200];
  int j;
  for (j = 0; j < 150; j++) {
    arr[j] = j;
  }
  for (j = 150; j < 200; j++) {
    arr[j] = j / 5;
  }
  for (j = 0; j < 200; j++) {
    printf("%d ", arr[j]);
  }
  printf("\n");

  int* arr_p = arr;
  int lo = 0;
  int lo_len = 150;
  int hi = 199;
  int hi_len = 50;
  TimsortMergeState state = { 0 };
  state.galloping = 1;
  state.min_gallop = 0;
  timsort_merge_runs_hi(&arr, sizeof(int), compare, lo, lo_len, hi, hi_len, &state);
  int i;
  for (i = 0; i < size; i++) {
    printf("%d ", arr[i]);
  }
  return 0;
}

static char* all_tests() {
  mu_run_test(test_stack_init);
  mu_run_test(test_stack_push_empty_stack);
  mu_run_test(test_stack_push_nonempty_stack);
  mu_run_test(test_stack_pop_empty_stack);
  mu_run_test(test_stack_pop_nonempty_stack);
  mu_run_test(test_stack_pop_return_nonempty_stack);
  mu_run_test(test_stack_free);
  mu_run_test(test_merge_runs_lo_gallop);
  //mu_run_test(test_merge_runs_hi_gallop);
  return 0;
}

int
main()
{
  char* result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);
  return result != 0;
}

