#include <assert.h>
#include <stdlib.h>

int main()
{
  signed char x = nondet_int();
  signed char y = nondet_int();
  __CPROVER_assume(x < y);
  __CPROVER_assume(-100 <= x && x <= 100);
  __CPROVER_assume(-100 <= y && y <= 100);
  while(x < y)
  {
    if(nondet_int())
      x--;
    else
      y++;
  }
  assert(0);
  return 0;
}
