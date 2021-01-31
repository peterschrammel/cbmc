#include <stdlib.h>

int main()
{
  __CPROVER_field_decl_global("field1", (int)0);
  __CPROVER_field_decl_local("field1", (int)0);

  int A[2];
  assert(__CPROVER_get_field(&(A[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(A[1]), "field1") == 0);
  __CPROVER_set_field(&(A[1]), "field1", 1);
  assert(__CPROVER_get_field(&(A[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(A[1]), "field1") == 1);

  int s;
  __CPROVER_assume(2 <= s && s <= 100);
  int B[s];
  assert(__CPROVER_get_field(&(B[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(B[1]), "field1") == 0);
  __CPROVER_set_field(&(B[1]), "field1", 1);
  assert(__CPROVER_get_field(&(B[0]), "field1") == 1);
  assert(__CPROVER_get_field(&(B[1]), "field1") == 1);

  int *C = malloc(2 * sizeof(int));;
  assert(__CPROVER_get_field(&(C[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(C[1]), "field1") == 0);
  __CPROVER_set_field(&(C[1]), "field1", 1);
  assert(__CPROVER_get_field(&(C[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(C[1]), "field1") == 1);

  int s;
  __CPROVER_assume(2 <= s && s <= 100);
  int *D = malloc(s * sizeof(int));
  assert(__CPROVER_get_field(&(D[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(D[1]), "field1") == 0);
  __CPROVER_set_field(&(D[1]), "field1", 1);
  assert(__CPROVER_get_field(&(D[0]), "field1") == 1);
  assert(__CPROVER_get_field(&(D[1]), "field1") == 1);
}
