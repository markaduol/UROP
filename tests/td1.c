#include <klee/klee.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "upb/table.int.h"

#define SIZE 8

/* Test Driver: log2ceil(int x) */

int main(int argc, char *argv[])
{
  uint64_t x;
  klee_make_symbolic(&x, sizeof x, "x");
  klee_assert(log2ceil(x) == log2ceil_renamed(x));

  return 0;
}
