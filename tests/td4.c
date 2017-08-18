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
  struct upb_msglayout *m1 = mk_symbolic_upb_msglayout("m1");
  struct upb_msglayout *m2 = mk_symbolic_upb_msglayout("m2");
  mk_assume_msglayout(m1, m2);
  
  assert(sizeof(*m1) == sizeof(*m2));

  upb_msg *res1 = upb_msg_new(m1, &upb_alloc_global);
  upb_msg *res2 = upb_msg_new_renamed(m2, &upb_alloc_global);
  klee_assert(res1 == res2);


  free(m1);
  free(m2);

  return 0;
}
