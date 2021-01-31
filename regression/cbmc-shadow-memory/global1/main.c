#include <assert.h>
#include <stdlib.h>

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
struct STRUCTNAME n[3];

int foo()
{
  // field declarations
  __CPROVER_field_decl_global("field1", (int)0);
  __CPROVER_field_decl_global("field2", (__CPROVER_bitvector[6])0);

  /***********************
   * Scalars and pointers to scalars
   ***********************/

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

  /***********************
   * Arrays and pointers into arrays
   ***********************/

  z = &(A[4]);

  assert(__CPROVER_get_field(z, "field1") == 0);
  assert(__CPROVER_get_field(z, "field2") == 0);

  __CPROVER_set_field(&(A[3]), "field1", 13);
  __CPROVER_set_field(&(A[3]), "field2", 14);
  __CPROVER_set_field(z, "field1", 15);
  __CPROVER_set_field(z, "field2", 16);

  z = A;

  assert(__CPROVER_get_field(z + 3, "field1") == 13);
  assert(__CPROVER_get_field(z + 3, "field2") == 14);
  assert(__CPROVER_get_field(z + 4, "field1") == 15);
  assert(__CPROVER_get_field(z + 4, "field2") == 16);

  int i;
  __CPROVER_assume(0 <= i && i < 5);
  __CPROVER_set_field(&(A[i]), "field1", 42);
  assert(__CPROVER_get_field(&(A[i]), "field1") == 42);

  z = &(A[i]);
  __CPROVER_set_field(z, "field1", 43);
  assert(__CPROVER_get_field(z, "field1") == 43);

  /***********************
   * Dynamically allocated arrays
   ***********************/

  z = malloc(10 * sizeof(int));

  assert(__CPROVER_get_field(z, "field1") == 0);
  assert(__CPROVER_get_field(z, "field2") == 0);

  __CPROVER_set_field(&(z[3]), "field1", 13);
  __CPROVER_set_field(&(z[3]), "field2", 14);
  __CPROVER_set_field(z + 4, "field1", 15);
  __CPROVER_set_field(z + 4, "field2", 16);

  z += 3;

  assert(__CPROVER_get_field(z, "field1") == 13);
  assert(__CPROVER_get_field(z, "field2") == 14);
  assert(__CPROVER_get_field(&(z[1]), "field1") == 15);
  assert(__CPROVER_get_field(&(z[1]), "field2") == 16);

  z -= 3;

  int j;
  __CPROVER_assume(0 <= j && j < 10);
  __CPROVER_set_field(&(z[j]), "field1", 42);
  assert(__CPROVER_get_field(&(z[j]), "field1") == 42);

  z = &(z[j]);
  __CPROVER_set_field(z, "field1", 43);
  assert(__CPROVER_get_field(z, "field1") == 43);

  /***********************
   * Structs and pointers into structs
   ***********************/

  p = &m;

  assert(__CPROVER_get_field(&(p->x1), "field1") == 0);
  assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 0);

  __CPROVER_set_field(&((*p).x1), "field1", 2);
  __CPROVER_set_field(&((*p).B1[1]), "field2", 2);

  assert(__CPROVER_get_field(&(p->x1), "field1") == 2);
  assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 2);
  assert(__CPROVER_get_field(&(p->B1[2]), "field1") == 0);

  int *q = &(m.B1[2]);
  assert(__CPROVER_get_field(q, "field1") == 0);
  __CPROVER_set_field(q, "field1", 7);
  assert(__CPROVER_get_field(q, "field1") == 7);

  int l;
  __CPROVER_assume(0 <= l && l < 3);
  __CPROVER_set_field(&(m.B1[l]), "field1", 44);
  assert(__CPROVER_get_field(&(m.B1[l]), "field1") == 44);

  z = &(m.B1[l]);
  __CPROVER_set_field(z, "field1", 45);
  assert(__CPROVER_get_field(z, "field1") == 45);

  /***********************
   * Dynamically allocated structs
   ***********************/

  p = malloc(sizeof(struct STRUCTNAME));

  assert(__CPROVER_get_field(&(p->x1), "field1") == 0);
  assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 0);

  __CPROVER_set_field(&((*p).x1), "field1", 2);
  __CPROVER_set_field(&((*p).B1[1]), "field2", 2);

  assert(__CPROVER_get_field(&(p->x1), "field1") == 2);
  assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 2);
  assert(__CPROVER_get_field(&(p->B1[2]), "field1") == 0);

  q = &(p->B1[2]);
  assert(__CPROVER_get_field(q, "field1") == 0);
  __CPROVER_set_field(q, "field1", 7);
  assert(__CPROVER_get_field(q, "field1") == 7);

  int k;
  __CPROVER_assume(0 <= k && k < 3);
  __CPROVER_set_field(&(p->B1[k]), "field1", 44);
  assert(__CPROVER_get_field(&(p->B1[k]), "field1") == 44);

  z = &(p->B1[k]);
  __CPROVER_set_field(z, "field1", 45);
  assert(__CPROVER_get_field(z, "field1") == 45);

  /***********************
   * Arrays of structs and pointers into them
   ***********************/

  assert(__CPROVER_get_field(&(n[1].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(n[1].B1[1]), "field2") == 0);

  p = &(n[2]);

  __CPROVER_set_field(&(n[1].x1), "field1", 1);
  __CPROVER_set_field(&(p->x1), "field1", 2);
  assert(__CPROVER_get_field(&(n[1].x1), "field1") == 1);
  assert(__CPROVER_get_field(&(p->x1), "field1") == 2);

  __CPROVER_set_field(&(n[1].B1[1]), "field2", 3);
  __CPROVER_set_field(&(p->B1[1]), "field2", 4);
  assert(__CPROVER_get_field(&(n[1].B1[1]), "field2") == 3);
  assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 4);

  q = &(n[1].x1);
  assert(__CPROVER_get_field(q, "field1") == 1);
  __CPROVER_set_field(q, "field1", 5);
  assert(__CPROVER_get_field(q, "field1") == 5);

  q = &(n[1].B1[1]);
  assert(__CPROVER_get_field(q, "field2") == 3);
  __CPROVER_set_field(q, "field2", 6);
  assert(__CPROVER_get_field(q, "field2") == 6);

  int x;
  __CPROVER_assume(0 <= x && x < 3);
  __CPROVER_set_field(&(n[k].B1[x]), "field1", 46);
  assert(__CPROVER_get_field(&(n[k].B1[x]), "field1") == 46);

  z = &(n[k].B1[x]);
  __CPROVER_set_field(z, "field1", 47);
  assert(__CPROVER_get_field(z, "field1") == 47);

  /***********************
   * Dynamically allocated arrays of structs
   ***********************/

  struct STRUCTNAME *u = malloc(3*sizeof(struct STRUCTNAME));

  assert(__CPROVER_get_field(&(u[1].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(u[1].B1[1]), "field2") == 0);

  p = &(u[2]);

  __CPROVER_set_field(&(u[1].x1), "field1", 1);
  __CPROVER_set_field(&(p->x1), "field1", 2);
  assert(__CPROVER_get_field(&(u[1].x1), "field1") == 1);
  assert(__CPROVER_get_field(&(p->x1), "field1") == 2);

  __CPROVER_set_field(&(u[1].B1[1]), "field2", 3);
  __CPROVER_set_field(&(p->B1[1]), "field2", 4);
  assert(__CPROVER_get_field(&(u[1].B1[1]), "field2") == 3);
  assert(__CPROVER_get_field(&(p->B1[1]), "field2") == 4);

  q = &(u[1].x1);
  assert(__CPROVER_get_field(q, "field1") == 1);
  __CPROVER_set_field(q, "field1", 5);
  assert(__CPROVER_get_field(q, "field1") == 5);

  q = &(u[1].B1[1]);
  assert(__CPROVER_get_field(q, "field2") == 3);
  __CPROVER_set_field(q, "field2", 6);
  assert(__CPROVER_get_field(q, "field2") == 6);

  int t;
  __CPROVER_assume(0 <= t && t < 3);
  __CPROVER_set_field(&(u[k].B1[t]), "field1", 46);
  assert(__CPROVER_get_field(&(u[k].B1[t]), "field1") == 46);

  z = &(u[k].B1[t]);
  __CPROVER_set_field(z, "field1", 47);
  assert(__CPROVER_get_field(z, "field1") == 47);
}

int main()
{
  foo();
}
