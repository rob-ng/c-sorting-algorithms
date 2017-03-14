#include <string.h>
#include <stdio.h>
#include <time.h>
#include "minunit.h"
#include "../src/stack.h"
#include "../src/sorting.h"

int tests_run = 0;

//##############################################################################
//# STACK TESTS
//##############################################################################

static char* 
test_stack_init() 
{
  Stack* stack = stack_init();
  mu_assert("stack_init: stack should be initialized correctly", 
            stack->head == NULL && stack->len == 0);
  return 0;
}

static char* 
test_stack_push_empty_stack() 
{
  Stack* stack = stack_init();
  int a = 3;
  stack_push(stack, &a);
  mu_assert("stack_push: stack should set head to pushed element.", 
            stack->head->data == &a);
  return 0;
}

static char* 
test_stack_push_nonempty_stack() 
{
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  mu_assert("stack_push: stack should set head to pushed element.", 
            stack->head->data == &b);
  return 0;
}

static char* 
test_stack_pop_empty_stack() 
{
  Stack* stack = stack_init();
  mu_assert("stack_pop: should return 0", stack_pop(stack) == 0);
  return 0;
}

static char* 
test_stack_pop_nonempty_stack() 
{
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  mu_assert("stack_pop: should update head and return 1", 
            stack_pop(stack) == 1 && stack->head->data == &a);
  return 0;
}

static char* test_stack_pop_return_nonempty_stack() 
{
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  mu_assert("stack_pop_return: should update head and return 1", 
            stack_pop_return(stack) == &b && stack->head->data == &a);
  return 0;
}

static char* test_stack_free() 
{
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  stack_free(&stack);
  mu_assert("stack_free: stack pointer should be NULL", stack == NULL);
  return 0;
}

//##############################################################################
//# SORTING TESTS
//##############################################################################

int
compare_ints(const void* a_p, const void* b_p)
{
  int a = *((int*) a_p);
  int b = *((int*) b_p);
  if (a < b) {
    return -1;
  } else if (a > b) {
    return 1;
  } else {
    return 0;
  }
}

typedef struct TestInput {
  int arr[100];
  int len;
} TestInput;

typedef union SortingFunc {
  void (*insert)(void*, size_t, size_t, int (*cmp)(const void*, const void*));
} SortingFunc;

TestInput*
get_sorting_tests()
{
  srand(time(NULL));
  static TestInput default_tests[29] = {
    { {}, 0 },
    { {0}, 1 },
    { {0, 0}, 2 },
    { {0, 0, 0}, 3 },
    { {0, 1}, 2 },
    { {1, 0}, 2 },
    { {0, 1, 2}, 3 },
    { {0, 2, 1}, 3 },
    { {1, 0, 2}, 3 },
    { {1, 2, 0}, 3 },
    { {2, 0, 1}, 3 },
    { {2, 1, 0}, 3 },
    { {0, 1, 1}, 3 },
    { {1, 0, 1}, 3 },
    { {1, 1, 0}, 3 },
    { {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 10 },
    { {10, 9, 8, 7, 6, 5, 4, 3, 2, 1}, 10 },
    { {42, 9, 17, 54, 602, -3, 54, 999, -11}, 9 },
    { {-11, -3, 9, 17, 42, 54, 54, 602, 999 }, 9 }
  };

  int start_index = 19;
  for (int i = 0; i < 10; i++) {
    TestInput rand_test;
    rand_test.len = 100;
    for (int j = 0; j < 100; j++) {
      rand_test.arr[j] = rand() % 100000;
    }
    default_tests[start_index + i] = rand_test;
  }

  return default_tests;
}

static char* 
test_binary_insert_sort()
{
  srand(time(NULL));

  TestInput* tests = get_sorting_tests();

  int successful_tests = 0;
  int matching = 0;
  TestInput my_test;
  TestInput compare_test;
  for (int t = 0; t < 29; t++) {
    matching = 0;
    my_test = *(tests+(t));
    memcpy(&compare_test, &my_test, sizeof(my_test));
    size_t len = my_test.len > 0 ? my_test.len - 1 : 0;
    binary_insert_sort(&my_test.arr, sizeof(int), compare_ints, 0, 
                       len);
    qsort(&compare_test.arr, compare_test.len, sizeof(int), compare_ints);
    for (int j = 0; j < my_test.len; j++) {
      if (my_test.arr[j] == compare_test.arr[j]) {
        matching++;
      } else {
        printf("FAILED! Test: %d\n", t);
        printf("My sort: ");
        for (int f = 0; f < my_test.len; f++) {
          printf("%d ", my_test.arr[f]);
        }
        printf("\n");
        printf("Default sort: ");
        for (int f = 0; f < compare_test.len; f++) {
          printf("%d ", compare_test.arr[f]);
        }
        printf("\n");
        break;
      }
    }
    if (matching == my_test.len) {
      successful_tests++;
    }
  }
  mu_assert("insert_sort(): failed to sort input", successful_tests == 29);
  return 0;
}

static char* 
test_insert_sort() 
{
  srand(time(NULL));

  TestInput* tests = get_sorting_tests();

  int successful_tests = 0;
  int matching = 0;
  TestInput my_test;
  TestInput compare_test;
  for (int t = 0; t < 29; t++) {
    matching = 0;
    my_test = *(tests+(t));
    memcpy(&compare_test, &my_test, sizeof(my_test));
    insert_sort(&my_test.arr, my_test.len, sizeof(int), compare_ints);
    qsort(&compare_test.arr, compare_test.len, sizeof(int), compare_ints);
    for (int j = 0; j < my_test.len; j++) {
      if (my_test.arr[j] == compare_test.arr[j]) {
        matching++;
      } else {
        printf("FAILED! Test: %d\n", t);
        printf("My sort: ");
        for (int f = 0; f < my_test.len; f++) {
          printf("%d ", my_test.arr[f]);
        }
        printf("\n");
        printf("Default sort: ");
        for (int f = 0; f < compare_test.len; f++) {
          printf("%d ", compare_test.arr[f]);
        }
        printf("\n");
        break;
      }
    }
    if (matching == my_test.len) {
      successful_tests++;
    }
  }
  mu_assert("insert_sort(): failed to sort input", successful_tests == 29);
  return 0;
}

static char* all_tests() {
  // Stack
  mu_run_test(test_stack_init);
  mu_run_test(test_stack_push_empty_stack);
  mu_run_test(test_stack_push_nonempty_stack);
  mu_run_test(test_stack_pop_empty_stack);
  mu_run_test(test_stack_pop_nonempty_stack);
  mu_run_test(test_stack_pop_return_nonempty_stack);
  mu_run_test(test_stack_free);

  // Sorts
  mu_run_test(test_insert_sort);
  mu_run_test(test_binary_insert_sort);

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

