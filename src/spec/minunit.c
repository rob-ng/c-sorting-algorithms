#include <stdio.h>
#include "minunit.h"
int tests_run = 0;

static char* test_ex() {
  mu_assert("error msg", "condition");
  return 0;
}

static char* all_tests()
{
  mu_run_test(test_ex);
  return 0;
}

int main(int argc, char **argv) {
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  }
  else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}

