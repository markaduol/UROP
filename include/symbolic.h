#ifndef BOILERPLATE_H
#define BOILERPLATE_H

/* Symbolic */
struct upb_msglayout *mk_symbolic_upb_msglayout(char *name);

upb_alloc *mk_symbolic_alloc(char *name);

upb_arena *mk_symbolic_arena(char *name);

upb_env *mk_symbolic_env(char *name);

upb_msglayout_fieldinit_v1 *mk_symbolic_fields(char *name);

upb_msglayout_oneofinit_v1 *mk_symbolic_oneofs(char *name);

upb_msglayout_msginit_v1 *mk_symbolic_msgs(char *name);

void mk_assume_msglayout(struct upb_msglayout *m1, struct upb_msglayout *m2);

void mk_assume_alloc(upb_alloc*, upb_alloc*);

void mk_assume_arena(upb_arena*, upb_arena*);

void mk_assume_env(upb_env*, upb_env*);

void mk_assume_fields(upb_msglayout_fieldinit_v1*, upb_msglayout_fieldinit_v1*);

void mk_assume_oneofs(upb_msglayout_oneofinit_v1*, upb_msglayout_oneofinit_v1*);

void mk_assume_msgs(upb_msglayout_msginit_v1*, upb_msglayout_msginit_v1*);

#endif
