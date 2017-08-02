#include <klee/klee.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "upb/upb.h"
#include "upb/encode.h"
#include "boilerplate.h"

#define SIZE 8
#define MAX_NAME_LENGTH 128

/* Helpful Macros ************************************************************/
#define UNUSED(var) (void)(var)

/* Standard error function for 'upb_env' *************************************/

static bool default_err(void *ud, const upb_status *status)
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

/* Concrete *****************************************************************/

upb_alloc *mk_upb_alloc()
{
  // Just return the global allocator used by upb_alloc (see upb/upb.h)
  return &upb_alloc_global;
}

upb_arena *mk_upb_arena()
{
  upb_alloc *alloc = mk_upb_alloc();
  upb_alloc *block_alloc = mk_upb_alloc();

  size_t bytes_allocated = 0;
  size_t next_block_size = 0;
  size_t max_block_size = 0;

  void *block_head = malloc(SIZE);
  if (!block_head)
    malloc_fail(-1);

  void *cleanup_head = malloc(SIZE);
  if (!cleanup_head)
    malloc_fail(-1);

  void *future1 = malloc(SIZE);
  if (!future1)
    malloc_fail(-1);

  void *future2 = malloc(SIZE);
  if (!future2)
    malloc_fail(-1);

  upb_arena *arena = (upb_arena*) malloc(sizeof (upb_arena));
  arena->alloc = *alloc;
  arena->block_alloc = block_alloc;
  arena->bytes_allocated = bytes_allocated;
  arena->next_block_size = next_block_size;
  arena->max_block_size = max_block_size;
  arena->block_head = block_head;
  arena->cleanup_head = cleanup_head;
  arena->future1 = future1;
  arena->future2 = future2;
  return arena;
}

upb_env *mk_upb_env()
{
  upb_arena *arena = mk_upb_arena();
  void *error_ud = malloc(SIZE);
  if (!error_ud)
    malloc_fail(-1);
  bool ok = true;

  upb_env *env = malloc(sizeof (upb_env));
  if (!env)
    malloc_fail(-1);

  env->arena_ = *arena;
  env->error_func_ = &default_err;
  env->error_ud_ = error_ud;
  env->ok_ = ok;
  return env;
}



upb_msglayout_fieldinit_v1 *mk_upb_msglayout_fieldinit_v1()
{
  upb_msglayout_fieldinit_v1 *fields = malloc(sizeof(*fields));
  if (!fields)
    malloc_fail(-1);

  fields->number = 0;
  fields->offset = 0;
  fields->hasbit = 0;
  fields->oneof_index = 0;
  fields->submsg_index = 0;
  fields->type = 0;
  fields->label = 0;
  return fields;
}

upb_msglayout_oneofinit_v1 *mk_upb_msglayout_oneofinit_v1()
{
  upb_msglayout_oneofinit_v1 *oneofs = malloc(sizeof(*oneofs));
  if (!oneofs)
    malloc_fail(-1);

  oneofs->data_offset = 0;
  oneofs->case_offset = 0;
  return oneofs;
}

upb_msglayout_msginit_v1 *mk_upb_msglayout_msginit_v1()
{
  upb_msglayout_msginit_v1 *m = malloc(sizeof(*m));
  if (!m)
    malloc_fail(-1);

  m->submsgs = NULL;
  m->fields = mk_upb_msglayout_fieldinit_v1();
  m->oneofs = mk_upb_msglayout_oneofinit_v1();
  m->default_msg = NULL;
  m->size = 0;
  m->field_count = 0;
  m->oneof_count = 0;
  m->extendable = 0;
  m->is_proto2 = 0;
  return m;
}

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
upb_alloc *mk_symbolic_alloc(int id)
{
  char name[MAX_NAME_LENGTH];
  sprintf(name, "alloc%d", id);

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

upb_arena *mk_symbolic_arena(int id)
{
  char name[MAX_NAME_LENGTH];
  sprintf(name, "arena%d", id);
  
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

upb_env *mk_symbolic_env(int id)
{
  char name[MAX_NAME_LENGTH];
  sprintf(name, "env%d", id);

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

upb_msglayout_fieldinit_v1 *mk_symbolic_fields(int id)
{
  char name[MAX_NAME_LENGTH];
  sprintf(name, "fields%d", id);

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

upb_msglayout_oneofinit_v1 *mk_symbolic_oneofs(int id)
{
  char name[MAX_NAME_LENGTH];
  sprintf(name, "oneofs%d", id);

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

/* upb_msglayout_msginit_v1 */

upb_msglayout_msginit_v1 *mk_symbolic_msgs(int id)
{
  char name[32];
  sprintf(name, "m%d", id);

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
  klee_assume(m1->default_msg == m2->default_msg);
  klee_assume(m1->size == m2->size);
  klee_assume(m1->field_count == m2->field_count);
  klee_assume(m1->oneof_count == m2->oneof_count);
  klee_assume(m1->extendable == m2->extendable);
  klee_assume(m1->is_proto2 == m2->is_proto2);
}
