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

  x = 1;

  if(x < 0)
    assert(x);
}
