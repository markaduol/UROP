#include <klee/klee.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "upb/table.int.h"

#define SIZE 8

static void malloc_failure(int status)
{
  fprintf(stderr, "Malloc failure");
  exit(status);
}

int main(int argc, char *argv[])
{
  uint64_t x1, x2;
  klee_make_symbolic(&x1, sizeof x1, "x1");
  klee_make_symbolic(&x2, sizeof x2, "x2");
  klee_assume(x1 == x2);
  klee_assert(log2ceil(x1) == log2ceil_renamed(x2));

  return 0;
}
