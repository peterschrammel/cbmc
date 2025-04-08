#include <pthread.h>

int x;

void *t1(void *arg)
{
  ;
}

int main(void)
{
  pthread_t id1;

  pthread_create(&id1, NULL, t1, NULL);
  pthread_join(id1, NULL);

  x = 1;

  assert(x <= 0);
}
