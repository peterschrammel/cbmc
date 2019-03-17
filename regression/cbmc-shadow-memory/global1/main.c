
struct STRUCTNAME
{
  int x1;
  int B1[3];
};

int y;
int *z;
int **w;
struct STRUCTNAME m, *p;
int A[5];

int foo()
{
  // field declarations
  __CPROVER_field_decl_global("field1", (int)0);
  __CPROVER_field_decl_global("field2", (__CPROVER_bitvector[6])0);

  y = 10;

  assert(__CPROVER_get_field(&y, "field1") == 0);
  assert(__CPROVER_get_field(&y, "field2") == 0);

  assert(__CPROVER_get_field(&z, "field1") == 0);
  assert(__CPROVER_get_field(&z, "field2") == 0);

  __CPROVER_set_field(&y, "field1", 3);
  __CPROVER_set_field(&y, "field2", 4);
  __CPROVER_set_field(&z, "field1", 5);
  __CPROVER_set_field(&z, "field2", 6);

  z = &y;

  assert(__CPROVER_get_field(z, "field1") == 3);
  assert(__CPROVER_get_field(z, "field2") == 4);

  assert(__CPROVER_get_field(&z, "field1") == 5);
  assert(__CPROVER_get_field(&z, "field2") == 6);

  assert(__CPROVER_get_field(z, "field1") == __CPROVER_get_field(&y, "field1"));
  assert(__CPROVER_get_field(z, "field2") == __CPROVER_get_field(&y, "field2"));

  w = &z;

  assert(__CPROVER_get_field(&w, "field1") == 0);
  assert(__CPROVER_get_field(&w, "field2") == 0);

  assert(__CPROVER_get_field(w, "field1") == 5);
  assert(__CPROVER_get_field(w, "field2") == 6);

  assert(__CPROVER_get_field(*w, "field1") == 3);
  assert(__CPROVER_get_field(*w, "field2") == 4);

  z = &(A[0]);

  assert(__CPROVER_get_field(z, "field1") == 0);
  assert(__CPROVER_get_field(z, "field2") == 0);

  __CPROVER_set_field(&(A[3]), "field1", 13);
  __CPROVER_set_field(&(A[3]), "field2", 14);

  z = A;

  assert(__CPROVER_get_field(z + 3, "field1") == 13);
  assert(__CPROVER_get_field(z + 3, "field2") == 14);

  z = malloc(10 * sizeof(int));

  __CPROVER_set_field(&(z[3]), "field1", y);
  assert(__CPROVER_get_field(z + 3, "field1") == 10);
  assert(__CPROVER_get_field(z + 3, "field2") == 0);

  p = &m;

  assert(__CPROVER_get_field(&(p->x1), "field1") == 0);
  assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 0);

  __CPROVER_set_field(&((*p).x1), "field1", 2);
  __CPROVER_set_field(&((*p).B1[1]), "field2", 2);

  assert(__CPROVER_get_field(&(p->x1), "field1") == 2);
  assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 2);
  assert(__CPROVER_get_field(&(p->B1[2]), "field1") == 0);
}

int main()
{
  foo();
}
