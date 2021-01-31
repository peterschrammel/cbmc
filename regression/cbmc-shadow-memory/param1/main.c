#include <stdlib.h>

struct STRUCTNAME
{
  int x1;
  int B1[3];
};

void f_int(int x)
{
  assert(__CPROVER_get_field(&x, "field1") == 0);
  __CPROVER_set_field(&x, "field1", 3);
  assert(__CPROVER_get_field(&x, "field1") == 3);
}

void f_intptr(int *x)
{
  assert(__CPROVER_get_field(&x, "field1") == 0);
  __CPROVER_set_field(&x, "field1", 3);
  assert(__CPROVER_get_field(&x, "field1") == 3);
}

void f_intarray(int x[])
{
  assert(__CPROVER_get_field(&x, "field1") == 0);
  __CPROVER_set_field(&x, "field1", 3);
  assert(__CPROVER_get_field(&x, "field1") == 3);
}

void f_struct(struct STRUCTNAME x)
{
  assert(__CPROVER_get_field(&x, "field1") == 0);
  __CPROVER_set_field(&x, "field1", 3);
  assert(__CPROVER_get_field(&x, "field1") == 3);
}

void f_structptr(struct STRUCTNAME *x)
{
  assert(__CPROVER_get_field(&x, "field1") == 0);
  __CPROVER_set_field(&x, "field1", 3);
  assert(__CPROVER_get_field(&x, "field1") == 3);
}


int main()
{
  __CPROVER_field_decl_local("field1", (int)0);
  int x;
  f_int(x);
  f_intptr(&x);
  int y[1];
  f_intarray(y);
  struct STRUCTNAME z;
  f_struct(z);
  f_structptr(&z);
}
