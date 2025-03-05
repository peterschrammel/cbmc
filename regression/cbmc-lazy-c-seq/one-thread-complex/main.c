#include <assert.h>
#include <pthread.h>
#include <stdio.h>

extern void __VERIFIER_atomic_begin(void);
extern void __VERIFIER_atomic_end(void);

int x = 42;

void *t1(void *arg)
{
  int c;
  c = x + 3;
  x = c;
}


int main(void)
{
  int a,b;
  pthread_t id1;

  a = x + 1;
  x = a;

  pthread_create(&id1, NULL, t1, NULL);

  b = x + 2;
  x = b;
  x = 50;

  pthread_join(id1, NULL);

  assert(x == 45 || x == 46 || x == 48 || x == 50 || x == 53);
}
