#include <stdlib.h>

void main()
{
  __CPROVER_field_decl_local("field1", (_Bool)1);
  int x;
  int *y = NULL;
  int c;
  int *z;
  __CPROVER_assume(c > 0);
  if(c)
    z = &x;
  else
    z = y;
  assert(__CPROVER_get_field(z, "field1"));
  __CPROVER_set_field(z, "field1", 0);
  assert(!__CPROVER_get_field(z, "field1"));

  assert(__CPROVER_get_field(y, "field1"));
}
