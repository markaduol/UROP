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
  char *s1 = malloc(SIZE);
  if (!s1)
  {
    free(s1);
    malloc_failure(-1);
  }
  klee_make_symbolic(s1, SIZE, "s1");
  klee_assume(s1[SIZE-1] == '\0');

  char *s2 = malloc(SIZE);
  if (!s2)
  {
    free(s2);
    malloc_failure(-1);
  }
  klee_make_symbolic(s2, SIZE, "s2");
  klee_assume(s2[SIZE-1] == '\0');

  int i = 0;
  for (i = 0; i < SIZE-1; i++)
    klee_assume(s1[i] == s2[i]);

  char *res1 = upb_strdup2(s1, SIZE, &upb_alloc_global);
  char *res2 = upb_strdup2_renamed(s2, SIZE, &upb_alloc_global);
  const unsigned char *l = (void*) res1;
  const unsigned char *r = (void*) res2;
  for (; *l && *r; l++, r++)
  {
    klee_assert(*l == *r);
  }

  free(s1);
  free(s2);

  return 0;
}