#include <assert.h>
#include <stdlib.h>

struct struct1
{
  int x1;
  int x2;
};

int test_struct()
{
  struct struct1 *y = malloc(sizeof(struct struct1));
  assert(__CPROVER_get_field(y, "field1") == 0);
  assert(__CPROVER_get_field(&y->x1, "field1") == 0);
  assert(__CPROVER_get_field(&y->x2, "field1") == 0);
  __CPROVER_set_field(y, "field1", 3);
  assert(__CPROVER_get_field(y, "field1") == 3);
  assert(__CPROVER_get_field(&y->x1, "field1") == 3);
  assert(__CPROVER_get_field(&y->x2, "field1") == 3);
  __CPROVER_set_field(&y->x2, "field1", 5);
  assert(__CPROVER_get_field(y, "field1") == 5);
  assert(__CPROVER_get_field(&y->x1, "field1") == 5);
  assert(__CPROVER_get_field(&y->x2, "field1") == 5);
}

int test_scalar()
{
  double *z = malloc(sizeof(double));
  assert(__CPROVER_get_field(z, "field1") == 0);
  __CPROVER_set_field(z, "field1", 3);
  assert(__CPROVER_get_field(z, "field1") == 3);
}

int test_array()
{
  short *a = malloc(2*sizeof(short));
  int size;
  __CPROVER_assume(size >= 2);
  short *b = malloc(size*sizeof(long));

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

  assert(__CPROVER_get_field(b, "field1") == 0);
  assert(__CPROVER_get_field(&b[0], "field1") == 0);
  assert(__CPROVER_get_field(&b[1], "field1") == 0);
  __CPROVER_set_field(b, "field1", 3);
  assert(__CPROVER_get_field(b, "field1") == 3);
  assert(__CPROVER_get_field(&b[0], "field1") == 3);
  assert(__CPROVER_get_field(&b[1], "field1") == 3);
  __CPROVER_set_field(&b[1], "field1", 5);
  assert(__CPROVER_get_field(b, "field1") == 5);
  assert(__CPROVER_get_field(&b[0], "field1") == 5);
  assert(__CPROVER_get_field(&b[1], "field1") == 5);
}

int main()
{
  __CPROVER_field_decl_global("field1", (int)0);
  test_struct();
  test_array();
  test_scalar();
}
