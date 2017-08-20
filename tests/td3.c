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

/* Test Driver: char* upb_encode(const void *msg, const upb_msglayout_msginit_v1 *l, 
 *                               upb_env *env, size_t *size)  */

int main(int argc, char *argv[])
{
  /* msg (symbolic) */
  char *msg = malloc(SIZE);
  if (!msg)
    malloc_fail(-1);
  klee_make_symbolic(msg, SIZE, "msg");

  /* upb_msglayout_msginit_v1 (symbolic) */
  upb_msglayout_msginit_v1 *m = mk_symbolic_msgs("m");

  /* upb_env (concrete) */
  upb_env *env = mk_symbolic_env("env");

  /* size (concrete) */
  size_t size = 128; 

  // assert
  char *res1 = upb_encode(msg, m, env, &size);
  char *res2 = upb_encode_renamed(msg, m, env, &size);
  klee_assert(res1 == res2);

  free(msg);
  return 0;
}
