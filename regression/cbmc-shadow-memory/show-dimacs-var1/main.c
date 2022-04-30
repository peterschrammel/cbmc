#include <assert.h>

__CPROVER_bitvector[3] __cseq_distinguished_global;
int __abc;
int x;

int main(int argc, char** argv)
{
  static int y;
  int z = 0;
  if (__cseq_distinguished_global) {
    __abc = 1;
    z = 1;
  } else {
    x = 1;
    y = 1;
  }
  assert(__abc == 0 && x == 0 && y == 0 && z == 0);
  return 0;
}
