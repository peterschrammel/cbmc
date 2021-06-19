#include <stdlib.h>
#include <string.h>

struct INNERSTRUCT
{
  int x1;
  int x2[2];
};

struct STRUCT
{
  struct INNERSTRUCT x[3];
  int y;
};

int main()
{
  // field declarations
  __CPROVER_field_decl_local("field1", (__CPROVER_bitvector[6])0);

  struct STRUCT s1;
  struct STRUCT s2;
//  struct STRUCT s3;

  __CPROVER_set_field(&(s1.x[0].x1), "field1", 1);
  __CPROVER_set_field(&(s1.x[1].x2[1]), "field1", 2);
/*  __CPROVER_set_field(&(s1.x[2].x2[0]), "field1", 3);
  __CPROVER_set_field(&(s1.y), "field1", 4);
  assert(__CPROVER_get_field(&(s1.x[0].x1), "field1") == 1);
  assert(__CPROVER_get_field(&(s1.x[0].x2[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(s1.x[0].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s1.x[1].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s1.x[1].x2[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(s1.x[1].x2[1]), "field1") == 2);
  assert(__CPROVER_get_field(&(s1.x[2].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s1.x[2].x2[0]), "field1") == 3);
  assert(__CPROVER_get_field(&(s1.x[2].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s1.y), "field1") == 4);*/
/*
  s2.x[1] = s1.x[2];
  assert(__CPROVER_get_field(&(s2.x[0].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[0].x2[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[0].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[1].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[1].x2[0]), "field1") == 3);
  assert(__CPROVER_get_field(&(s2.x[1].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[2].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[2].x2[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[2].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.y), "field1") == 0);
*/
  s2 = s1;
  assert(__CPROVER_get_field(&(s2.x[0].x1), "field1") == 1);
/*  assert(__CPROVER_get_field(&(s2.x[0].x2[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[0].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[1].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[1].x2[0]), "field1") == 0);*/
  assert(__CPROVER_get_field(&(s2.x[1].x2[1]), "field1") == 2);
/*  assert(__CPROVER_get_field(&(s2.x[2].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.x[2].x2[0]), "field1") == 3);
  assert(__CPROVER_get_field(&(s2.x[2].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s2.y), "field1") == 4);*/
/*
  memcpy(&(s3.x[1]), &(s1.x[2]), sizeof(struct INNERSTRUCT));
  assert(__CPROVER_get_field(&(s3.x[0].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[0].x2[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[0].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[1].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[1].x2[0]), "field1") == 3);
  assert(__CPROVER_get_field(&(s3.x[1].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[2].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[2].x2[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[2].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.y), "field1") == 0);

  memcpy(&s3, &s1, sizeof(struct STRUCT));
  assert(__CPROVER_get_field(&(s3.x[0].x1), "field1") == 1);
  assert(__CPROVER_get_field(&(s3.x[0].x2[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[0].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[1].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[1].x2[0]), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[1].x2[1]), "field1") == 2);
  assert(__CPROVER_get_field(&(s3.x[2].x1), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.x[2].x2[0]), "field1") == 3);
  assert(__CPROVER_get_field(&(s3.x[2].x2[1]), "field1") == 0);
  assert(__CPROVER_get_field(&(s3.y), "field1") == 4);
  */
}
