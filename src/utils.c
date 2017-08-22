#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void malloc_fail(int status)
{
  fprintf(stderr, "Malloc failure!");
  exit(status);
}
