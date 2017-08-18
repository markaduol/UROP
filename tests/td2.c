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
  char *s1 = malloc(SIZE);
  if (!s1)
    malloc_fail(-1);

  klee_make_symbolic(s1, SIZE, "s1");
  klee_assume(s1[SIZE-1] == '\0');

  char *s2 = malloc(SIZE);
  if (!s2)
    malloc_fail(-1);

  klee_make_symbolic(s2, SIZE, "s2");
  klee_assume(s2[SIZE-1] == '\0');

  int i = 0;
  for (i = 0; i < SIZE-1; i++)
    klee_assume(s1[i] == s2[i]);

  char *res1 = upb_strdup2(s1, SIZE, &upb_alloc_global);
  char *res2 = upb_strdup2_renamed(s2, SIZE, &upb_alloc_global);
  klee_assert(res1 == res2);

  free(s1);
  free(s2);

  return 0;
}
