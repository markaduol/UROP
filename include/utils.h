#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#include "upb/upb.h"
#include "upb/encode.h"
#include "upb/structs.int.h"

bool default_err(void *ud, const upb_status *status);

void malloc_fail(int status);

#endif
