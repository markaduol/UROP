#include <klee/klee.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "upb/upb.h"
#include "upb/encode.h"
#include "upb/msg.h"
#include "concrete.h"
#include "symbolic.h"
#include "utils.h"

#define SIZE 8
#define MAX_NAME_LENGTH 128

/* Symbolic **************************************************************
 *
 * We concretize some values of the structs in the 'mk_assume' functions.
 * This is in order to minimize the search space for klee, and allow for 
 * more targeted experiments.
 *
 * */

/*
 * The address held by 'func' is made symbolic, not the actual function
 * being pointed to.
 *
 * struct upb_alloc {
 *   upb_alloc_func *func;
 * };
 *
 * */


upb_alloc *mk_symbolic_alloc(char *name)
{
  upb_alloc *alloc = malloc(sizeof(upb_alloc));
  if (!alloc)
    malloc_fail(-1);

  klee_make_symbolic(alloc, sizeof(upb_alloc), name);
  return alloc;
}

void mk_assume_alloc(upb_alloc *alloc1, upb_alloc *alloc2)
{
  klee_assume(alloc1->func == alloc2->func);
}

/* upb_arena */

upb_arena *mk_symbolic_arena(char *name)
{
  upb_arena *arena = malloc(sizeof(upb_arena));
  if (!arena)
    malloc_fail(-1);

  klee_make_symbolic(arena, sizeof(upb_arena), name);
  return arena;
}

void mk_assume_arena(upb_arena *arena1, upb_arena *arena2)
{
  arena1->alloc = *mk_upb_alloc();
  arena2->alloc = arena1->alloc;
  arena1->block_alloc = mk_upb_alloc();
  arena2->block_alloc = arena1->block_alloc;

  klee_assume(arena1->bytes_allocated == arena2->bytes_allocated);
  klee_assume(arena1->next_block_size == arena2->next_block_size);
  klee_assume(arena1->max_block_size == arena2->max_block_size);
  klee_assume(arena1->block_head == arena2->block_head);
  klee_assume(arena1->cleanup_head == arena2->cleanup_head);
  klee_assume(arena1->future1 == arena2->future1);
  klee_assume(arena1->future2 == arena2->future2);
}

/* upb_env */

upb_env *mk_symbolic_env(char *name)
{
  upb_env *env = malloc(sizeof(upb_env));
  if (!env)
    malloc_fail(-1);

  klee_make_symbolic(env, sizeof(upb_env), name);
  return env;
}

void mk_assume_env(upb_env *env1, upb_env *env2)
{
  env1->arena_ = *mk_upb_arena();
  env2->arena_ = env1->arena_;
  env1->error_func_ = &default_err;
  env2->error_func_ = env1->error_func_;

  klee_assume(env1->error_ud_ == env2->error_ud_);
  klee_assume(env1->ok_ == env2->ok_);
}

/* upb_msglayout_fieldinit_v1 */

upb_msglayout_fieldinit_v1 *mk_symbolic_fields(char *name)
{
  upb_msglayout_fieldinit_v1 *fields = malloc(sizeof(*fields));
  if (!fields)
    malloc_fail(-1);

  klee_make_symbolic(fields, sizeof(*fields), name);
  return fields;
}

void mk_assume_fields(
    upb_msglayout_fieldinit_v1 *fields1,
    upb_msglayout_fieldinit_v1 *fields2
    )
{
  klee_assume(fields1->number == fields2->number);
  klee_assume(fields1->offset == fields2->offset);
  klee_assume(fields1->hasbit == fields2->hasbit);
  klee_assume(fields1->oneof_index == fields2->oneof_index);
  klee_assume(fields1->type == fields2->type);
  klee_assume(fields1->label == fields2->label);
}

/* upb_msglayout_oneofsinit_v1 */

upb_msglayout_oneofinit_v1 *mk_symbolic_oneofs(char *name)
{
  upb_msglayout_oneofinit_v1 *oneofs = malloc(sizeof(*oneofs));
  if (!oneofs)
    malloc_fail(-1);

  klee_make_symbolic(oneofs, sizeof(*oneofs), name);
  return oneofs;
}

void mk_assume_oneofs(
    upb_msglayout_oneofinit_v1 *oneofs1,
    upb_msglayout_oneofinit_v1 *oneofs2)
{
  klee_assume(oneofs1->data_offset == oneofs2->data_offset);
  klee_assume(oneofs1->case_offset == oneofs2->case_offset);
}

/* upb_msglayout */

struct upb_msglayout *mk_symbolic_upb_msglayout(char *name)
{
  struct upb_msglayout *m = malloc(sizeof(struct upb_msglayout));
  if (!m)
    malloc_fail(-1);

  klee_make_symbolic(m, sizeof(*m), name);
  return m;
}

void mk_assume_msglayout(struct upb_msglayout *m1, struct upb_msglayout *m2)
{
  mk_assume_msgs(&m1->data, &m2->data);
}

/* upb_msglayout_msginit_v1 */

upb_msglayout_msginit_v1 *mk_symbolic_msgs(char *name)
{
  upb_msglayout_msginit_v1 *m = malloc(sizeof(upb_msglayout_msginit_v1));
  if (!m)
    malloc_fail(-1);
  klee_make_symbolic(m, sizeof(*m), name);
  return m;
}


void mk_assume_msgs(
    upb_msglayout_msginit_v1 *m1, 
    upb_msglayout_msginit_v1 *m2)
{
  // Concrete
  m1->fields = mk_upb_msglayout_fieldinit_v1();
  m2->fields = m1->fields;
  m1->oneofs = mk_upb_msglayout_oneofinit_v1();
  m2->oneofs = m1->oneofs;
  //klee_assume(m1->default_msg == m2->default_msg);
  m1->default_msg = malloc(sizeof(SIZE));
  if (!m1->default_msg)
    malloc_fail(-1);
  m2->default_msg = m1->default_msg;
  // Symbolic
  klee_assume(m1->submsgs == m2->submsgs);
  klee_assume(m1->size == m2->size);
  klee_assume(m1->field_count == m2->field_count);
  klee_assume(m1->oneof_count == m2->oneof_count);
  klee_assume(m1->extendable == m2->extendable);
  klee_assume(m1->is_proto2 == m2->is_proto2);
}
