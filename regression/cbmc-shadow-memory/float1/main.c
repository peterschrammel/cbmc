#include <assert.h>

int main()
{
  // field declarations
  __CPROVER_field_decl_local("field1", (unsigned __CPROVER_bitvector[7])0);

  unsigned __CPROVER_bitvector[7] v;
  float x;
  assert(!__CPROVER_get_field(&x, "field1"));

  __CPROVER_set_field(&x, "field1", v);
  assert(__CPROVER_get_field(&x, "field1") == v);

  return 0;
}
