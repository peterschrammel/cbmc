#include <assert.h>
#include <stdlib.h>

struct struct1
{
  int x1;
  int x2;
};

struct struct1 y;

double z;

short a[2];

int test_struct()
{
  assert(__CPROVER_get_field(&y, "field1") == 0);
  assert(__CPROVER_get_field(&y.x1, "field1") == 0);
  assert(__CPROVER_get_field(&y.x2, "field1") == 0);
  __CPROVER_set_field(&y, "field1", 3);
  assert(__CPROVER_get_field(&y, "field1") == 3);
  assert(__CPROVER_get_field(&y.x1, "field1") == 3);
  assert(__CPROVER_get_field(&y.x2, "field1") == 3);
  __CPROVER_set_field(&y.x2, "field1", 5);
  assert(__CPROVER_get_field(&y, "field1") == 5);
  assert(__CPROVER_get_field(&y.x1, "field1") == 5);
  assert(__CPROVER_get_field(&y.x2, "field1") == 5);
}

int test_scalar()
{
  assert(__CPROVER_get_field(&z, "field1") == 0);
  __CPROVER_set_field(&z, "field1", 3);
  assert(__CPROVER_get_field(&z, "field1") == 3);
}

int test_array()
{
  assert(__CPROVER_get_field(a, "field1") == 0);
  assert(__CPROVER_get_field(&a[0], "field1") == 0);
  assert(__CPROVER_get_field(&a[1], "field1") == 0);
  __CPROVER_set_field(a, "field1", 3);
  assert(__CPROVER_get_field(a, "field1") == 3);
  assert(__CPROVER_get_field(&a[0], "field1") == 3);
  assert(__CPROVER_get_field(&a[1], "field1") == 3);
  __CPROVER_set_field(&a[1], "field1", 5);
  assert(__CPROVER_get_field(a, "field1") == 5);
  assert(__CPROVER_get_field(&a[0], "field1") == 5);
  assert(__CPROVER_get_field(&a[1], "field1") == 5);
}

int main()
{
  __CPROVER_field_decl_global("field1", (int)0);
  // test_struct();
  test_array();
  test_scalar();
}
