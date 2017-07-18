#include <klee/klee.h>
#include <stdarg.h>
#include <stdio.h>

#include "util/strutil.h"

int main()
{
  re2::StringPiece* src_1, src_2;

  klee_make_symbolic(src_1, sizeof src_1, "src_1");
  klee_make_symbolic(src_2, sizeof src_2, "src_2");
  klee_assume(src_1 == src_2);
  
  string res_1 = re2::CEscape(&src_1);
  string res_2 = re2::CEscape(&src_2);
  klee_assert(res_1 == res_2);
}
