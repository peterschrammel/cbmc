#include <stdbool.h>


int fun(int* ptr_x)
{
  int y = 0;
  if(ptr_x)
  {
    int y = *ptr_x;
    y += 1;
  }
  else
  {
    y = 4;
  }

  return y;
}
