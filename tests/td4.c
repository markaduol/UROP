#include <klee/klee.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "upb/upb.h"
#include "upb/encode.h"
#include "upb/structs.int.h"
#include "upb/msg.h"

#include "utils.h"
#include "concrete.h"
#include "symbolic.h"

/* Test Driver: upb_msg *upb_msg_new(const upb_msglayout *l, upb_alloc *a) */

int main()
{
  struct upb_msglayout *l = mk_symbolic_upb_msglayout("l");

  upb_msg *res1 = upb_msg_new(l, &upb_alloc_global);
  upb_msg *res2 = upb_msg_new_renamed(l, &upb_alloc_global);
  klee_assert(res1 == res2);

  free(l);

  return 0;
}
