#include <assert.h>

int __cs_nondet_x;
int y;

int main(int argc, char **argv)
{
  assert(__cs_nondet_x == 1);
  assert(y == 0);
  return y;
}
