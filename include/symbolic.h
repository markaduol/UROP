#ifndef BOILERPLATE_H
#define BOILERPLATE_H

/* Symbolic */
struct upb_msglayout *mk_symbolic_upb_msglayout(char *name);

upb_alloc *mk_symbolic_alloc(int id);

upb_arena *mk_symbolic_arena(int id);

upb_env *mk_symbolic_env(int id);

upb_msglayout_fieldinit_v1 *mk_symbolic_fields(int id);

upb_msglayout_oneofinit_v1 *mk_symbolic_oneofs(int id);

upb_msglayout_msginit_v1 *mk_symbolic_msgs(int id);

void mk_assume_msglayout(struct upb_msglayout *m1, struct upb_msglayout *m2);

void mk_assume_alloc(upb_alloc*, upb_alloc*);

void mk_assume_arena(upb_arena*, upb_arena*);

void mk_assume_env(upb_env*, upb_env*);

void mk_assume_fields(upb_msglayout_fieldinit_v1*, upb_msglayout_fieldinit_v1*);

void mk_assume_oneofs(upb_msglayout_oneofinit_v1*, upb_msglayout_oneofinit_v1*);

void mk_assume_msgs(upb_msglayout_msginit_v1*, upb_msglayout_msginit_v1*);

#endif
