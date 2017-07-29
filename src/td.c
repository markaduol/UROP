#include <klee/klee.h>

#define SIZE 7

int main(int argc, char *argv[])
{
  char s1[SIZE];
  char s2[SIZE];
  klee_make_symbolic(s1, sizeof s1, "s1");
  klee_make_symbolic(s2, sizeof s2, "s2");
  klee_assume(s1 == s2);
  klee_assert(strlen(s1) == strlen_renamed(s2));
}
