#include <pthread.h>
#include <stdio.h>
#include <assert.h>

extern void __VERIFIER_atomic_begin(void);
extern void __VERIFIER_atomic_end(void);

int shared;

void *t1(void *arg) {
  int value_t1;
  value_t1 = 5;

  __VERIFIER_atomic_begin();
  value_t1 += shared;
  __VERIFIER_atomic_end();
}

void *t2(void *arg) {
  int value_t2;
  value_t2 = 3;

  __VERIFIER_atomic_begin();
  shared += value_t2;
  __VERIFIER_atomic_end();
}

int main(void) {
  pthread_t id1, id2;

  __VERIFIER_atomic_begin();
  shared = 10;
  __VERIFIER_atomic_end();

  pthread_create(&id1, NULL, t1, NULL);
  pthread_create(&id2, NULL, t2, NULL);

  pthread_join(id1, NULL);
  pthread_join(id2, NULL);

  assert(0);
}
