#include <pthread.h>
#include <stdio.h>

/*   thread 1 modifies vars[i] at round i 
     thread 2 sets addresses[i]=&vars[i] at round i+1
                   and output[i]=*addresses[i] at round i+1
     locks are used to enforce round switches
     SAFE in SC  (CBMC gives spurious counterexample for NLOCKS=2)
     UNSAFE in TSO
*/
#define NLOCKS 2
int vs[NLOCKS], outs[NLOCKS], *addrs[NLOCKS], x = -1;
_Bool lcks1[NLOCKS + 1], lcks2[NLOCKS + 1];
//_Bool lcks[NLOCKS+1];
int *vars = vs, *outputs = outs, **addresses = addrs;
_Bool *locks1 = lcks1, *locks2 = lcks2;
//_Bool *locks=lcks;

void *thr1()
{
  int i;
  locks1[0] = 1;
  for(i = 0; i <= NLOCKS; i++)
  {
    if(locks1[i])
      locks2[i] = 1;
  }
  return;
}

void *thr2()
{
  int i; //,j;
  for(i = 0; i <= NLOCKS;
      i++) //in the error trace iteration i should match round number
    if(locks2[i])
    {
      vars[i] = i;
      if(i > 0)
        vars[i - 1] = -1;
    }
  return;
}

void *thr3()
{
  int i; //,j,k;
  for(i = 0; i <= NLOCKS; i++)
  {
    if(locks2[i])
    {
      if(i < NLOCKS)
        locks1[i + 1] = 1;
      if(i > 0)
      {
        addresses[i - 1] =
          &vars[i - 1]; // store address where we copy the value for output
        outputs[i - 1] = *addresses[i - 1];
      }
    }
  }

  if(locks2[NLOCKS])
  {
    for(i = 0; i < NLOCKS; i++)
      if(outputs[i] != i)
        return;
    assert(0);
  }
  return;
}

int main()
{
  int i;
  pthread_t t1, t2, t3;

  for(i = 0; i < NLOCKS + 1; i++)
  {
    addresses[i] = &x;
    vars[i] = -1;
  }
  outputs[0] = -1;

  pthread_create(&t1, 0, thr1, 0);
  pthread_create(&t2, 0, thr2, 0);
  pthread_create(&t3, 0, thr3, 0);
  pthread_join(t1, 0);
  pthread_join(t2, 0);
  pthread_join(t3, 0);
  return 0;
}
