#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "concrete.h"
#include "utils.h"
#include "upb/upb.h"
#include "upb/encode.h"
#include "upb/structs.int.h"

#define SIZE 8

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
