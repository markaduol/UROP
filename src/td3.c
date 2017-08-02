#include <klee/klee.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "upb/upb.h"
#include "upb/encode.h"
#include "upb/structs.int.h"

#define SIZE 8

// defined in upb/encode.c
typedef struct {
  upb_env *env;
  char *buf, *ptr, *limit;
} upb_encstate;

static void malloc_failure(int status)
{
  fprintf(stderr, "Malloc failure");
  exit(status);
}

static void mk_symbolic_fields(upb_msglayout_fieldinit_v1 *fields, int id)
{
  char name_prefix[32];
  sprintf(name_prefix, "fields%d", id);

  klee_make_symbolic(fields, sizeof(*fields), name_prefix);
  printf("Size of fields: %lu\nSize of *fields: %lu\n ", sizeof(fields), sizeof(*fields));
}

static void mk_symbolic_oneofs(upb_msglayout_oneofinit_v1 *oneofs, int id)
{
  char name_prefix[32];
  sprintf(name_prefix, "oneofs%d", id);

  klee_make_symbolic(oneofs, sizeof(*oneofs), name_prefix);
}


static void mk_symbolic_m(upb_msglayout_msginit_v1 *m, int id)
{
  char name_prefix[32];
  sprintf(name_prefix, "m%d", id);

  klee_make_symbolic(m, sizeof(*m), name_prefix);
}

static void mk_assume_fields(upb_msglayout_fieldinit_v1 *fields1, upb_msglayout_fieldinit_v1 *fields2)
{
  klee_assume(fields1->number == fields2->number);
  klee_assume(fields1->offset == fields2->offset);
  klee_assume(fields1->hasbit == fields2->hasbit);
  klee_assume(fields1->oneof_index == fields2->oneof_index);
  klee_assume(fields1->type == fields2->type);
  klee_assume(fields1->label == fields2->label);
}

static void mk_assume_oneofs(upb_msglayout_oneofinit_v1 *oneofs1, upb_msglayout_oneofinit_v1 *oneofs2)
{
  klee_assume(oneofs1->data_offset == oneofs2->data_offset);
  klee_assume(oneofs1->case_offset == oneofs2->case_offset);
}

static void mk_assume_m(upb_msglayout_msginit_v1 *m1, upb_msglayout_msginit_v1 *m2)
{
  klee_assume(m1->default_msg == m2->default_msg);
  klee_assume(m1->size == m2->size);
  klee_assume(m1->field_count == m2->field_count);
  klee_assume(m1->oneof_count == m2->oneof_count);
  klee_assume(m1->extendable == m2->extendable);
  klee_assume(m1->is_proto2 == m2->is_proto2);
}

int main(int argc, char *argv[])
{
  upb_encstate *e1 = malloc(sizeof(*e1));
  if (!e1)
    malloc_failure(-1);

  // TODO set e->env pointer (of type upb_env)
  e1->buf = malloc(sizeof(char));
  e1->ptr = malloc(sizeof(char));
  e1->limit = malloc(sizeof(char));
  klee_make_symbolic(e1->buf, sizeof(char), "e1->buf");
  klee_make_symbolic(e1->ptr, sizeof(char), "e1->ptr");
  klee_make_symbolic(e1->limit, sizeof(char), "e1->limit");

  upb_encstate *e2 = malloc(sizeof(*e2));
  if (!e2)
    malloc_failure(-1);

  e2->buf = malloc(sizeof(char));
  e2->ptr = malloc(sizeof(char));
  e2->limit = malloc(sizeof(char));
  klee_make_symbolic(e2->buf, sizeof(char), "e2->buf");
  klee_make_symbolic(e2->ptr, sizeof(char), "e2->ptr");
  klee_make_symbolic(e2->limit, sizeof(char), "e2->limit");

  char msg1[SIZE];
  char msg2[SIZE];

  klee_make_symbolic(msg1, SIZE, "msg1");
  klee_make_symbolic(msg2, SIZE, "msg2");
  klee_assume(msg1[SIZE-1] == '\0');
  klee_assume(msg2[SIZE-1] == '\0');
  int i = 0;
  for (i = 0; i < SIZE - 1; i++)
  {
    klee_assume(msg1[i] == msg2[i]);
  }

  // For v1 of upb_encode_message
  size_t size = 10;

  /* fields1 */
  upb_msglayout_fieldinit_v1 *fields1 = malloc(sizeof(*fields1));
  if (!fields1)
    malloc_failure(-1);
  mk_symbolic_fields(fields1, 1);


  /* fields2 */
  upb_msglayout_fieldinit_v1 *fields2 = malloc(sizeof(*fields2));
  if (!fields2)
    malloc_failure(-1);
  mk_symbolic_fields(fields2, 2);
  mk_assume_fields(fields1, fields2);

  /* oneofs1 */
  upb_msglayout_oneofinit_v1 *oneofs1 = malloc(sizeof(*oneofs1));
  if (!oneofs1)
    malloc_failure(-1);
  mk_symbolic_oneofs(oneofs1, 1);


  /* oneofs2 */
  upb_msglayout_oneofinit_v1 *oneofs2 = malloc(sizeof(*oneofs2));
  if (!oneofs2)
    malloc_failure(-1);
  mk_symbolic_oneofs(oneofs2, 2);
  mk_assume_oneofs(oneofs1, oneofs2);


  // TODO set m->submsgs pointer (of type upb_msglayout_msginit_v1 *const*)
  /* m1 */
  upb_msglayout_msginit_v1 *m1 = malloc(sizeof(*m1));
  if (!m1)
    malloc_failure(-1);
  m1->fields = fields1;
  m1->oneofs = oneofs1;
  mk_symbolic_m(m1, 1);

  /* m2 */
  upb_msglayout_msginit_v1 *m2 = malloc(sizeof(*m2));
  if (!m2)
    malloc_failure(-1);
  m2->fields = fields2;
  m2->oneofs = oneofs2;
  mk_symbolic_m(m2, 2);
  mk_assume_m(m1, m2);

  /*TODO assume fields in structs equal */
  klee_assert(upb_encode_message(e1, msg1, m1) == upb_encode_message_renamed(e2, msg2, m2, &size));

  return 0;
}
