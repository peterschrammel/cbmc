#include <stdlib.h>

int __VERIFIER_nondet_int(void);

int main()
{
  int *p = malloc(sizeof(int));
  if(__VERIFIER_nondet_int() > 10)
    p = NULL;
  *p = 1;
  free(p);
  return 0;
}
