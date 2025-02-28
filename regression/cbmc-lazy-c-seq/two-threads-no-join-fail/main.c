#include <assert.h>
#include <pthread.h>
#include <stdio.h>

extern void __VERIFIER_atomic_begin(void);
extern void __VERIFIER_atomic_end(void);

int x = 42;

void *t1(void *arg)
{
  int a;
  a = x + 1;
  x = a;
}

void *t2(void *arg)
{
  int b;
  b = x;
  x = 42;
}

int main(void)
{
  pthread_t id1, id2;
  //x=x;

  pthread_create(&id1, NULL, t1, NULL);
  pthread_create(&id2, NULL, t2, NULL);

  //pthread_join(id1, NULL);
  //pthread_join(id2, NULL);
  //x=x;

  assert(x == 42 || x == 43);
}
