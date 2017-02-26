#include <stdio.h>
#include "minunit.h"
#include "../src/stack.h"

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

static char* test_stack_pop_stack_one_element() {
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  mu_assert("stack_pop: should update head and return 1", stack_pop(stack) == 1 && stack->head->data == &a);
  return 0;
}

static char* test_stack_free() {
  Stack* stack = stack_init();
  int a = 3;
  int b = 4;
  stack_push(stack, &a);
  stack_push(stack, &b);
  stack_free(stack);
  mu_assert("stack_free: should have len 0 and head should be NULL", stack->head == NULL && stack->len == 0);
  return 0;
}

static char* all_tests() {
  mu_run_test(test_stack_init);
  mu_run_test(test_stack_push_empty_stack);
  mu_run_test(test_stack_push_nonempty_stack);
  mu_run_test(test_stack_pop_empty_stack);
  mu_run_test(test_stack_pop_stack_one_element);
  mu_run_test(test_stack_free);
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

