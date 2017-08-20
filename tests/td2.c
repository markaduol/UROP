#include <klee/klee.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "upb/table.int.h"

#include "utils.h"
#include "concrete.h"
#include "symbolic.h"

#define SIZE 8

/* Test Driver: char *upb_strdup2(const char *s, size_t len, upb_alloc *a) */

int main(int argc, char *argv[])
{
  char *s = malloc(SIZE);
  if (!s)
    malloc_fail(-1);

  klee_make_symbolic(s, SIZE, "s");
  klee_assume(s[SIZE-1] == '\0');

  size_t len;
  klee_make_symbolic(&len, sizeof(size_t), "len");

  char *res1 = upb_strdup2(s, len, &upb_alloc_global);
  char *res2 = upb_strdup2_renamed(s, len, &upb_alloc_global);
  klee_assert(res1 == res2);

  free(s);

  return 0;
}
