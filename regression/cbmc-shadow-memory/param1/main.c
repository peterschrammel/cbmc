#include <stdlib.h>

struct STRUCTNAME
{
  int x1;
  int B1[3];
};

void f_int_val(int x)
{
  assert(__CPROVER_get_field(&x, "field1") == 0);
  __CPROVER_set_field(&x, "field1", 1);
  assert(__CPROVER_get_field(&x, "field1") == 1);
}

void f_intptr_ptr0(int *x)
{
  assert(__CPROVER_get_field(x, "field1") == 0);
  __CPROVER_set_field(x, "field1", 1);
  assert(__CPROVER_get_field(x, "field1") == 1);
}

void f_intptr_ptr1(int *x)
{
  assert(__CPROVER_get_field(x, "field1") == 1);
  __CPROVER_set_field(x, "field1", 2);
  assert(__CPROVER_get_field(x, "field1") == 2);
}

void f_intptr_val(int *x)
{
  assert(__CPROVER_get_field(&x, "field1") == 0);
  __CPROVER_set_field(&x, "field1", 3);
  assert(__CPROVER_get_field(&x, "field1") == 3);
}

void f_intarray_ptr0(int x[])
{
  assert(__CPROVER_get_field(&(x[0]), "field1") == 0);
  __CPROVER_set_field(&(x[0]), "field1", 1);
  assert(__CPROVER_get_field(&(x[0]), "field1") == 1);
}

void f_intarray_ptr1(int x[])
{
  assert(__CPROVER_get_field(&(x[0]), "field1") == 1);
  __CPROVER_set_field(&(x[0]), "field1", 2);
  assert(__CPROVER_get_field(&(x[0]), "field1") == 2);
}

void f_struct_val(struct STRUCTNAME x)
{
  assert(__CPROVER_get_field(&(x.B1[2]), "field1") == 0);
  __CPROVER_set_field(&(x.B1[2]), "field1", 5);
  assert(__CPROVER_get_field(&(x.B1[2]), "field1") == 5);
}

void f_structptr_ptr0(struct STRUCTNAME *x)
{
  assert(__CPROVER_get_field(&(x->B1[2]), "field1") == 0);
  __CPROVER_set_field(&(x->B1[2]), "field1", 1);
  assert(__CPROVER_get_field(&(x->B1[2]), "field1") == 1);
}

void f_structptr_ptr1(struct STRUCTNAME *x)
{
  assert(__CPROVER_get_field(&(x->B1[2]), "field1") == 1);
  __CPROVER_set_field(&(x->B1[2]), "field1", 2);
  assert(__CPROVER_get_field(&(x->B1[2]), "field1") == 2);
}

void f_structptr_val(struct STRUCTNAME *x)
{
  assert(__CPROVER_get_field(&x, "field1") == 0);
  __CPROVER_set_field(&x, "field1", 7);
  assert(__CPROVER_get_field(&x, "field1") == 7);
}

void f_int_local(int rec, int value)
{
  int x;
  assert(__CPROVER_get_field(&x, "field1") == 0);
  __CPROVER_set_field(&x, "field1", value);
  assert(__CPROVER_get_field(&x, "field1") == value);
  if(rec) {
    f_int_local(0, 2);
    assert(__CPROVER_get_field(&x, "field1") == value);
  }
}


int main()
{
  __CPROVER_field_decl_local("field1", (int)0);
  int x;
  f_int_val(x);
  f_int_val(x);
  f_intptr_ptr0(&x);
  f_intptr_ptr1(&x);
  f_intptr_val(&x);
  f_intptr_val(&x);
  int y[1];
  f_intarray_ptr0(y);
  f_intarray_ptr1(y);
  struct STRUCTNAME z;
  f_struct_val(z);
  f_struct_val(z);
  f_structptr_ptr0(&z);
  f_structptr_ptr1(&z);
  f_structptr_val(&z);
  f_structptr_val(&z);
  f_int_local(1, 1);
}
