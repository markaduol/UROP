#include <klee/klee.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "upb/upb.h"
#include "upb/encode.h"
#include "upb/structs.int.h"

#include "utils.h"
#include "concrete.h"
#include "symbolic.h"

#define SIZE 8

int main(int argc, char *argv[])
{
  /* msg (symbolic) */
  char *msg1 = malloc(SIZE);
  if (!msg1)
    malloc_fail(-1);

  char *msg2 = malloc(SIZE);
  if (!msg2)
    malloc_fail(-1);

  klee_make_symbolic(msg1, SIZE, "msg1");
  klee_make_symbolic(msg2, SIZE, "msg2");
  int i;
  for (i = 0; i < SIZE; i++)
  {
    klee_assume(msg1[i] == msg2[i]);
  }

  /* upb_msglayout_msginit_v1 (symbolic) */
  upb_msglayout_msginit_v1 *m1 = mk_symbolic_msgs(1);
  upb_msglayout_msginit_v1 *m2 = mk_symbolic_msgs(2);
  mk_assume_msgs(m1, m2);

  /* upb_env (concrete) */
  upb_env *env = mk_upb_env();

  /* size (concrete) */
  size_t size = 128; 

  // assert
  const void *l = (const void*) msg1;
  const void *r = (const void*) msg2;
  char *res1 = upb_encode(l, m1, env, &size);
  char *res2 = upb_encode_renamed(r, m2, env, &size);
  for (; *res1 && *res2; res1++, res2++)
  {
    klee_assert(*res1 == *res2);
  }
  // Should both be '\0' here
  klee_assert(*res1 == *res2);
  free(msg1);
  free(msg2);

  return 0;
}
