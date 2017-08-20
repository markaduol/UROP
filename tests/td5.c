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
  struct upb_msglayout *l = mk_symbolic_upb_msglayout("l");

  void *mem = malloc(sizeof(*mem));
  if (!mem)
    malloc_fail(-1);

  klee_make_symbolic(mem, sizeof(*mem), "mem");

  upb_msg *res1 = upb_msg_init(mem, l, &upb_alloc_global);
  upb_msg *res2 = upb_msg_init_renamed(mem, l, &upb_alloc_global);
  klee_assert(res1 == res2);

  free(mem);
  free(l);

  return 0;
}
