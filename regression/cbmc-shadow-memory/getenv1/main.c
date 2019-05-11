#include <stdlib.h>

void main()
{
  __CPROVER_field_decl_local("dr_write", (_Bool) 0);
  char * env = getenv("PATH");
  assert(__CPROVER_get_field(env, "dr_write") == 0);
}
