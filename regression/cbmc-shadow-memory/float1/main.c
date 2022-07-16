#include <assert.h>

int main()
{
  // field declarations
  __CPROVER_field_decl_local("field1", (unsigned __CPROVER_bitvector[8])0);

  float x;
  assert(!__CPROVER_get_field(&x, "field1"));

  __CPROVER_set_field(&x, "field1", 0xfeu);
  assert(__CPROVER_get_field(&x, "field1") == 0xfeu);

  return 0;
}
