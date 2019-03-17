#include <stdlib.h>

struct STRUCTNAME
{
  int x1;
  int B1[3];
};

int main()
{
  // field declarations
  __CPROVER_field_decl_local("field1", (int)0);
  __CPROVER_field_decl_local("field2", (__CPROVER_bitvector[6])0);

  int y = 10;

  __VERIFIER_assert(__CPROVER_get_field(&y, "field1") == 0);
  __VERIFIER_assert(__CPROVER_get_field(&y, "field2") == 0);

  int *z;

  __VERIFIER_assert(__CPROVER_get_field(&z, "field1") == 0);
  __VERIFIER_assert(__CPROVER_get_field(&z, "field2") == 0);

  __CPROVER_set_field(&y, "field1", 3);
  __CPROVER_set_field(&y, "field2", 4);
  __CPROVER_set_field(&z, "field1", 5);
  __CPROVER_set_field(&z, "field2", 6);

  z = &y;

  __VERIFIER_assert(__CPROVER_get_field(z, "field1") == 3);
  __VERIFIER_assert(__CPROVER_get_field(z, "field2") == 4);

  __VERIFIER_assert(__CPROVER_get_field(&z, "field1") == 5);
  __VERIFIER_assert(__CPROVER_get_field(&z, "field2") == 6);

  __VERIFIER_assert(
    __CPROVER_get_field(z, "field1") == __CPROVER_get_field(&y, "field1"));
  __VERIFIER_assert(
    __CPROVER_get_field(z, "field2") == __CPROVER_get_field(&y, "field2"));

  int **w;

  w = &z;

  __VERIFIER_assert(__CPROVER_get_field(&w, "field1") == 0);
  __VERIFIER_assert(__CPROVER_get_field(&w, "field2") == 0);

  __VERIFIER_assert(__CPROVER_get_field(w, "field1") == 5);
  __VERIFIER_assert(__CPROVER_get_field(w, "field2") == 6);

  __VERIFIER_assert(__CPROVER_get_field(*w, "field1") == 3);
  __VERIFIER_assert(__CPROVER_get_field(*w, "field2") == 4);

  int A[5];

  z = &(A[10]);

  __VERIFIER_assert(__CPROVER_get_field(z, "field1") == 0);
  __VERIFIER_assert(__CPROVER_get_field(z, "field2") == 0);

  __CPROVER_set_field(&(A[3]), "field1", 13);
  __CPROVER_set_field(&(A[3]), "field2", 14);

  z = A;

  __VERIFIER_assert(__CPROVER_get_field(z + 3, "field1") == 13);
  __VERIFIER_assert(__CPROVER_get_field(z + 3, "field2") == 14);

  z = malloc(20 * sizeof(int));

  __CPROVER_set_field(&(w[3]), "field1", y);
  __VERIFIER_assert(__CPROVER_get_field(z + 3, "field1") == 10);
  __VERIFIER_assert(__CPROVER_get_field(z + 3, "field2") == 0);

  struct STRUCTNAME m, *p;

  p = &m;

  __VERIFIER_assert(__CPROVER_get_field(&(p->x1), "field1") == 0);
  __VERIFIER_assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 0);

  __CPROVER_set_field(&((*p).x1), "field1", 2);
  __CPROVER_set_field(&((*p).B1[1]), "field2", 2);

  __VERIFIER_assert(__CPROVER_get_field(&(p->x1), "field1") == 2);
  __VERIFIER_assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 2);
  __VERIFIER_assert(__CPROVER_get_field(&(p->B1[2]), "field1") == 0);
}
