#include <math.h>

int fun()
{
  double rem = fmod(4.0, 3.0);
  if(rem == 0.0)
  {
    return 1;
  }
  else
  {
    return -1;
  }
}
