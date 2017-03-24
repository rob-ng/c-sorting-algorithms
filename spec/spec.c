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
//# SORTING TEST SETUP
//##############################################################################

// Number of test cases.
#define NUM_TESTS 29

int
compare_ints(const void* a, const void* b)
{
  int aval = *((const int*)a); 
  int bval = *((const int*)b); 
  return (aval < bval) ? -1 : (aval > bval);
}

int
compare_chars(const void* a, const void* b)
{
  int diff = strncmp((char*)a, (char*)b, 1);
  return (diff < 0) ? -1 : (diff > 0);
}

typedef struct TestInput {
  int arr[100];
  int len;
} TestInput;

TestInput*
get_sorting_tests()
{
  srand(time(NULL));
  static TestInput default_tests[NUM_TESTS] = {
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

  for (int i = 19; i < NUM_TESTS; i++) {
    TestInput rand_test;
    rand_test.len = 100;
    for (int j = 0; j < 100; j++) {
      rand_test.arr[j] = rand() % 100000;
    }
    default_tests[i] = rand_test;
  }

  return default_tests;
}

//##############################################################################
//# SORTING HELPER TESTS
//##############################################################################
// Note: Require altering function declaration to non-static before running.
/*static char*
test_bin_search_loc()
{
  int arr[10] = { -4, -2, 0, 0, 1, 3, 3, 3, 7, 10 };
  int target = 5;
  size_t index = bin_search_loc(&arr, sizeof(int), compare_ints, 
                                0, 9 * sizeof(int), &target) / sizeof(int);
  mu_assert("bin_search_loc: should have index 8", index == 8);

  target = -5;
  index = bin_search_loc(&arr, sizeof(int), compare_ints, 
                         0, 9 * sizeof(int), &target) / sizeof(int);
  mu_assert("bin_search_loc: should have index 0 for small element", 
            index == 0);

  target = 20;
  index = bin_search_loc(&arr, sizeof(int), compare_ints, 
                         0, 9 * sizeof(int), &target) / sizeof(int);
  mu_assert("bin_search_loc: should return last index of array for target"
            "greater than all other elements in the array", index == 9);
  
  return 0;
}*/

//##############################################################################
//# SORTING TESTS
//##############################################################################

static char* 
test_sort_no_bounds(void (*sort)(void*, size_t, size_t, 
                                 int (*cmp)(const void*, const void*)),
                    char* sort_name) 
{
  srand(time(NULL));

  TestInput* tests = get_sorting_tests();

  int successful_tests = 0;
  int matching = 0;
  TestInput my_test;
  TestInput compare_test;
  for (int t = 0; t < NUM_TESTS; t++) {
    matching = 0;
    my_test = *(tests+(t));
    memcpy(&compare_test, &my_test, sizeof(my_test));
    sort(&my_test.arr, my_test.len, sizeof(int), compare_ints);
    qsort(&compare_test.arr, compare_test.len, sizeof(int), compare_ints);
    for (int j = 0; j < my_test.len; j++) {
      if (my_test.arr[j] == compare_test.arr[j]) {
        matching++;
      } else {
        printf("Sort (%s) FAILED! Test: %d\n", sort_name, t);
        for (int f = 0; f < my_test.len; f++) {
          if (compare_ints(&my_test.arr[f], &compare_test.arr[f]) == 0) {
            printf("%d ", my_test.arr[f]);
          } else {
            printf("(%d | %d) ", my_test.arr[f], compare_test.arr[f]);
          } 
        }
        printf("\n");
        break;
      }
    }
    if (matching == my_test.len) {
      successful_tests++;
    }
  }
  static char msg_buff[100];
  int msg_created;
  msg_created = snprintf(msg_buff, 100, "Sort (%s): failed to sort input", 
                         sort_name);
  if (msg_created >= 0 && msg_created < 100) {
    mu_assert(msg_buff, successful_tests == NUM_TESTS);
  } else {
    printf("Error generating message for (%s)\n", sort_name);
  }
  return 0;
}

static char* 
test_sort_bounds(void (*sort)(void*, size_t, 
                              int (*cmp)(const void*, const void*),
                              size_t lo, size_t hi), char* sort_name)
{
  srand(time(NULL));

  TestInput* tests = get_sorting_tests();

  int successful_tests = 0;
  int matching = 0;
  TestInput my_test;
  TestInput compare_test;
  for (int t = 0; t < NUM_TESTS; t++) {
    matching = 0;
    my_test = *(tests+(t));
    memcpy(&compare_test, &my_test, sizeof(my_test));
    size_t len = my_test.len > 0 ? my_test.len - 1 : 0;
    sort(&my_test.arr, sizeof(int), compare_ints, 0, 
                       len);
    qsort(&compare_test.arr, compare_test.len, sizeof(int), compare_ints);
    for (int j = 0; j < my_test.len; j++) {
      if (my_test.arr[j] == compare_test.arr[j]) {
        matching++;
      } else {
        printf("Sort (%s) FAILED! Test: %d\n", sort_name, t);
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
  static char msg_buff[100];
  int msg_created;
  msg_created = snprintf(msg_buff, 100, "Sort (%s): failed to sort input", 
                         sort_name);
  if (msg_created >= 0 && msg_created < 100) {
    mu_assert(msg_buff, successful_tests == NUM_TESTS);
  } else {
    printf("Error generating message for (%s)\n", sort_name);
  }
  return 0;
}

static char*
test_timsort_stress_integers()
{
  enum { STRESS_TEST_SIZE = 5368709 };
  int* tst = malloc(STRESS_TEST_SIZE * sizeof(int));
  int* def = malloc(STRESS_TEST_SIZE * sizeof(int));
  int correct = 0;
  int random;
  srand(time(NULL));


  // Sorted Fully (Asc)
  for (size_t i = 0; i < STRESS_TEST_SIZE; i++) {
    tst[i] = def[i] = i;
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  qsort(def, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Int (Sorted Fully Asc) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  // Sorted Fully (Dsc)
  for (size_t i = STRESS_TEST_SIZE - 1; i --> 0;) {
    tst[i] = def[i] = i;
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  qsort(def, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Int (Sorted Fully Dsc) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  // Sorted Partial (80% Asc)
  tst[0] = def[0] = 0;
  for (size_t i = 0; i < STRESS_TEST_SIZE; i++) {
    random = rand() % 10;
    if (random < 8) {
      tst[i] = def[i] = i + 1;
    } else {
      tst[i] = def[i] = i - 1;
    }
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  qsort(def, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Int (Sorted Partial Asc) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  // Sorted Partial (80% Dsc)
  tst[0] = def[0] = 0;
  for (size_t i = 0; i < STRESS_TEST_SIZE; i++) {
    random = rand() % 10;
    if (random < 2) {
      tst[i] = def[i] = i + 1;
    } else {
      tst[i] = def[i] = i - 1;
    }
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  qsort(def, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Int (Sorted Partial Dsc) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  // Unsorted Grouped
  const int group_size = 500;
  for (size_t i = 0; i < STRESS_TEST_SIZE - group_size; i += group_size) {
    random = rand() % 10000;
    for (size_t j = i; j < i + group_size; j++) {
      tst[j] = def[j] = random;
    }
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  qsort(def, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Int (Unsorted Grouped) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  // Unsorted
  for (size_t i = 0; i < STRESS_TEST_SIZE; i++) {
    random = rand() % 10000;
    tst[i] = def[i] = random;
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  qsort(def, STRESS_TEST_SIZE, sizeof(int), compare_ints);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Int (Unsorted) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  free(tst);
  free(def);
}

static char*
test_timsort_stress_chars()
{
  enum { STRESS_TEST_SIZE = 5368709 };
  char* tst = malloc(STRESS_TEST_SIZE * sizeof(char));
  char* def = malloc(STRESS_TEST_SIZE * sizeof(char));
  int correct = 0;
  char random;
  char prev;
  srand(time(NULL));


  // Sorted Fully (Asc)
  char curr_char = 'A';
  size_t interval = STRESS_TEST_SIZE / 25;
  for (size_t i = 0; i < STRESS_TEST_SIZE; i += interval) { 
    for (int j = i; j < i + interval; j++) {
      if (j >= STRESS_TEST_SIZE) { break; }
      tst[j] = def[j] = curr_char;
    }
    curr_char++;
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  qsort(def, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    } 
  }
  mu_assert("Timsort Stress Test Chars (Sorted Fully Asc) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  // Sorted Fully (Dsc)
  // NOTE: With chars, it is not possible to have a fully dsc array longer than
  // 128 elements (as Dsc array requires strictly descending).


  // Sorted Partial (80% Asc)
  tst[0] = def[0] = 64;
  for (size_t i = 1; i < STRESS_TEST_SIZE; i++) {
    random = rand() % 10;
    prev = tst[i - 1];
    if (random < 8) {
      tst[i] = def[i] = prev < 128 ? prev + 1 : prev;
    } else {
      tst[i] = def[i] = prev > 0 ? prev - 1 : 64;
    }
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  qsort(def, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Chars (Sorted Partial Asc) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  // Sorted Partial (80% Dsc)
  tst[0] = def[0] = 64;
  for (size_t i = 1; i < STRESS_TEST_SIZE; i++) {
    random = rand() % 10;
    prev = tst[i - 1];
    if (random < 2) {
      tst[i] = def[i] = prev < 128 ? prev + 1 : prev;
    } else {
      tst[i] = def[i] = prev > 0 ? prev - 1 : 64;
    }
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  qsort(def, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Chars (Sorted Partial Dsc) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  // Unsorted Grouped
  const int group_size = 500;
  for (size_t i = 0; i < STRESS_TEST_SIZE - group_size; i += group_size) {
    random = rand() % 26;
    for (size_t j = i; j < i + group_size; j++) {
      tst[j] = def[j] = random;
    }
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  qsort(def, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Chars (Unsorted Grouped) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  // Unsorted
  for (size_t i = 0; i < STRESS_TEST_SIZE; i++) {
    random = rand() % 26;
    tst[i] = def[i] = random;
  }
  timsort(tst, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  qsort(def, STRESS_TEST_SIZE, sizeof(char), compare_chars);
  for (size_t j = 0; j < STRESS_TEST_SIZE; j++) {
    if (tst[j] == def[j]) {
      correct++;
    }
  }
  mu_assert("Timsort Stress Test Chars (Unsorted) failed to sort",
            correct == STRESS_TEST_SIZE);
  correct = 0;


  free(tst);
  free(def);
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

  // Helpers
  //mu_run_test(test_bin_search_loc);

  // Sorts
  mu_run_test_on_arg(test_sort_no_bounds, insert_sort, "insert_sort");
  mu_run_test_on_arg(test_sort_no_bounds, binary_insert_sort, 
                     "binary_insert_sort");
  mu_run_test_on_arg(test_sort_no_bounds, select_sort, "select_sort");
  mu_run_test_on_arg(test_sort_no_bounds, comb_sort, "comb_sort");
  mu_run_test_on_arg(test_sort_no_bounds, merge_sort, "merge_sort");
  mu_run_test_on_arg(test_sort_no_bounds, quick_sort, "quick_sort");
  mu_run_test_on_arg(test_sort_no_bounds, timsort, "timsort");

  // Stress Tests
  mu_run_test(test_timsort_stress_integers);
  mu_run_test(test_timsort_stress_chars);

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

