#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

/* Helpful Macros ************************************************************/
#define UNUSED(var) (void)(var)

/* Standard error function for 'upb_env' *************************************/

bool default_err(void *ud, const upb_status *status)
{
  UNUSED(ud);
  UNUSED(status);
  return false; // don't attempt to recover the error
}


void malloc_fail(int status)
{
  fprintf(stderr, "Malloc failure!");
  exit(status);
}
