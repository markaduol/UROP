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

/* Test Driver: upb_msg *upb_msg_init(void *mem, const upb_msglayout *l, upb_alloc *a) */

int main()
{
  struct upb_msglayout *l1 = mk_symbolic_upb_msglayout("l1");
  struct upb_msglayout *l2 = mk_symbolic_upb_msglayout("l2");
  mk_assume_msglayout(l1, l2);

  void *mem1 = malloc(sizeof(*mem1));
  if (!mem1)
    malloc_fail(-1);
  void *mem2 = malloc(sizeof(*mem2));
  if (!mem2)
    malloc_fail(-1);
  klee_make_symbolic(mem1, sizeof(*mem1), "mem1");
  klee_make_symbolic(mem2, sizeof(*mem2), "mem2");

  upb_msg *res1 = upb_msg_init(mem1, l1, &upb_alloc_global);
  upb_msg *res2 = upb_msg_init_renamed(mem2, l2, &upb_alloc_global);
  klee_assert(res1 == res2);

  free(mem1);
  free(mem2);
  free(l1);
  free(l2);

  return 0;
}
