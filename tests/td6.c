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

/* Test Driver: size_t upb_msg_sizeof(const upb_msglayout *l) */

int main()
{
  struct upb_msglayout *l = mk_symbolic_upb_msglayout("l");

  size_t res1 = upb_msg_sizeof(l);
  size_t res2 = upb_msg_sizeof_renamed(l);
  klee_assert(res1 == res2);

  free(l);

  return 0;
}
