#ifndef CONCRETE_H
#define CONCRETE_H

#include "upb/upb.h"
#include "upb/encode.h"
#include "upb/structs.int.h"

upb_alloc *mk_upb_alloc();

upb_arena *mk_upb_arena();

upb_env *mk_upb_env();

upb_msglayout_fieldinit_v1 *mk_upb_msglayout_fieldinit_v1();

upb_msglayout_oneofinit_v1 *mk_upb_msglayout_oneofinit_v1();

upb_msglayout_msginit_v1 *mk_upb_msglayout_msginit_v1();

#endif
