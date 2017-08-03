#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "upb/upb.h"
#include "upb/encode.h"
#include "upb/structs.int.h"

#include "concrete.h"
#include "utils.h"

#define SIZE 8

int main(int argc, char *argv[])
{
  printf("Press ENTER key to continue\n");
  getchar();
  
  char *msg = malloc(SIZE);
  if (!msg)
    malloc_fail(-1);

  int i;
  for (i = 0; i < SIZE; i++)
    msg[i] = '\0';

  // upb_msglayout_msginit_v1
  upb_msglayout_msginit_v1 *m = mk_upb_msglayout_msginit_v1();
  m->field_count = 0x0101;

  // upb_env
  upb_env *env = mk_upb_env();

  size_t size = 128; 

  // assert
  const void *l = (const void*) msg;
  char *res1 = upb_encode(l, m, env, &size);
  printf("upb_encode [ret value]:%s\n", res1);
  char *res2 = upb_encode_renamed(l, m, env, &size);
  printf("upb_encode_renamed [ret value]:%s\n", res2);

  return 0;
}
