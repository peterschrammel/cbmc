#include <stdlib.h>

union UNIONNAME
{
  int x1;
  struct {
    char y1;
    int y2;
    short y3;
  } x2;
  char x3[3];
};

int main()
{
  // field declarations
  __CPROVER_field_decl_local("field2", (__CPROVER_bitvector[6])0);

  /***********************
   * Union with or semantics
   ***********************/

  union UNIONNAME u;

  assert(__CPROVER_get_field(&u, "field2") == 0);
  assert(__CPROVER_get_field(&(u.x1), "field2") == 0);
  assert(__CPROVER_get_field(&(u.x2), "field2") == 0);
  assert(__CPROVER_get_field(&(u.x2.y1), "field2") == 0);
  assert(__CPROVER_get_field(&(u.x2.y2), "field2") == 0);
  assert(__CPROVER_get_field(&(u.x2.y3), "field2") == 0);
  //assert(__CPROVER_get_field(u.x3, "field2") == 0);
  assert(__CPROVER_get_field(&(u.x3[0]), "field2") == 0);
  assert(__CPROVER_get_field(&(u.x3[1]), "field2") == 0);
  assert(__CPROVER_get_field(&(u.x3[2]), "field2") == 0);

  __CPROVER_set_field(&(u.x1), "field2", 1);
  assert(__CPROVER_get_field(&u, "field2") == 1);
  assert(__CPROVER_get_field(&(u.x1), "field2") == 1);
  assert(__CPROVER_get_field(&(u.x2), "field2") == 1);
  assert(__CPROVER_get_field(&(u.x2.y1), "field2") == 1);
  assert(__CPROVER_get_field(&(u.x2.y2), "field2") == 1);
  assert(__CPROVER_get_field(&(u.x2.y3), "field2") == 0);
  //assert(__CPROVER_get_field(u.x3, "field2") == 1);
  assert(__CPROVER_get_field(&(u.x3[0]), "field2") == 1);
  assert(__CPROVER_get_field(&(u.x3[1]), "field2") == 1);
  assert(__CPROVER_get_field(&(u.x3[2]), "field2") == 1);

  __CPROVER_set_field(&(u.x2.y1), "field2", 2);
  assert(__CPROVER_get_field(&u, "field2") == 3);
  assert(__CPROVER_get_field(&(u.x1), "field2") == 3);
  assert(__CPROVER_get_field(&(u.x2), "field2") == 3);
  assert(__CPROVER_get_field(&(u.x2.y1), "field2") == 3);
  assert(__CPROVER_get_field(&(u.x2.y2), "field2") == 1);
  assert(__CPROVER_get_field(&(u.x2.y3), "field2") == 0);
  //assert(__CPROVER_get_field(u.x3, "field2") == 3);
  assert(__CPROVER_get_field(&(u.x3[0]), "field2") == 2);
  assert(__CPROVER_get_field(&(u.x3[1]), "field2") == 1);
  assert(__CPROVER_get_field(&(u.x3[2]), "field2") == 1);

  __CPROVER_set_field(&(u.x2.y2), "field2", 4);
  assert(__CPROVER_get_field(&u, "field2") == 7);
  assert(__CPROVER_get_field(&(u.x1), "field2") == 7);
  assert(__CPROVER_get_field(&(u.x2), "field2") == 7);
  assert(__CPROVER_get_field(&(u.x2.y1), "field2") == 3);
  assert(__CPROVER_get_field(&(u.x2.y2), "field2") == 5);
  assert(__CPROVER_get_field(&(u.x2.y3), "field2") == 0);
  //assert(__CPROVER_get_field(u.x3, "field2") == 7);
  assert(__CPROVER_get_field(&(u.x3[0]), "field2") == 3);
  assert(__CPROVER_get_field(&(u.x3[1]), "field2") == 5);
  assert(__CPROVER_get_field(&(u.x3[2]), "field2") == 5);

  __CPROVER_set_field(&(u.x2.y3), "field2", 8);
  assert(__CPROVER_get_field(&u, "field2") == 15);
  assert(__CPROVER_get_field(&(u.x1), "field2") == 7);
  assert(__CPROVER_get_field(&(u.x2), "field2") == 15);
  assert(__CPROVER_get_field(&(u.x2.y1), "field2") == 3);
  assert(__CPROVER_get_field(&(u.x2.y2), "field2") == 5);
  assert(__CPROVER_get_field(&(u.x2.y3), "field2") == 8);
  //assert(__CPROVER_get_field(u.x3, "field2") == 7);
  assert(__CPROVER_get_field(&(u.x3[0]), "field2") == 3);
  assert(__CPROVER_get_field(&(u.x3[1]), "field2") == 5);
  assert(__CPROVER_get_field(&(u.x3[2]), "field2") == 5);
}
