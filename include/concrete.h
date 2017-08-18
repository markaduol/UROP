#ifndef CONCRETE_H
#define CONCRETE_H

#include "upb/upb.h"
#include "upb/encode.h"
#include "upb/decode.h"
#include "upb/structs.int.h"
#include "upb/msg.h"

// From upb/msg.c (known valid commits: 1aafd41, ae30b4a - latest commit as of 18/08/17)
struct upb_msglayout {
    struct upb_msglayout_msginit_v1 data;
};


upb_alloc *mk_upb_alloc();

upb_arena *mk_upb_arena();

upb_env *mk_upb_env();

upb_msglayout_fieldinit_v1 *mk_upb_msglayout_fieldinit_v1();

upb_msglayout_oneofinit_v1 *mk_upb_msglayout_oneofinit_v1();

upb_msglayout_msginit_v1 *mk_upb_msglayout_msginit_v1();

#endif
