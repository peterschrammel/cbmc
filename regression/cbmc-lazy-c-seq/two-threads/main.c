#include <pthread.h>
#include <stdio.h>
#include <assert.h>

extern void __VERIFIER_atomic_begin(void);
extern void __VERIFIER_atomic_end(void);

int x;

void *t1(void *arg) {
  int a;
  a = x + 1;
  x = a;
}

void *t2(void *arg) {
  int b;
  b = x;
  x = 42;
}

int main(void) {
  pthread_t id1, id2;

  pthread_create(&id1, NULL, t1, NULL);
  pthread_create(&id2, NULL, t2, NULL);

  pthread_join(id1, NULL);
  pthread_join(id2, NULL);

  assert(x == 42 || x == 43);
}
