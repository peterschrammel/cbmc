#include <assert.h>
#include <stdio.h>

int main()
{
  __CPROVER_field_decl_local("field1", (_Bool)0);
  __CPROVER_field_decl_global("field1", (_Bool)0);

  const char *a = "abc";
  FILE *f;
  fprintf(f, "bla %s", a);

  __CPROVER_set_field(a, "field1", 1);
  assert(__CPROVER_get_field(a, "field1") == 1);
}
