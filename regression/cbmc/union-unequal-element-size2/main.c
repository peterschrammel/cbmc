#include <assert.h>

union UNIONNAME
{
  struct
  {
    char y1;
    char y2;
    short y3;
    short y4;
  } x2;
  char x3[3];
};

int main()
{
  union UNIONNAME u;
  u.x2.y1 = 0x01;
  u.x2.y2 = 0x01;
  u.x2.y3 = 0x0101;
  u.x2.y4 = 0x0101;

  assert(u.x3[0]);
  assert(u.x3[1]);
  assert(u.x3[2]);

  assert(*((char *)&u.x2.y1));
  assert(*((char *)&u.x2.y2));
  assert(*((char *)&u.x2.y3));
  char s1 = *(((char *)&u.x2.y3) + 1);
  assert(s1);
  char s2 = *((char *)&u.x2.y4);
  assert(s2);
  char s3 = *(((char *)&u.x2.y4) + 1);
  assert(s3);
  return 0;
}
