#include <assert.h>

struct STRUCTNAME
{
  int f;
};

int main()
{
  {
    // constant scalar
    const int x = 0;
    __CPROVER_unconst_type(typeof(x)) y;
    y = 1;
  }
  {
    // constant pointer
    int z = 0;
    int w = 0;
    int * const x = &z;
    __CPROVER_unconst_type(typeof(x)) y;
    y = &z;
    (*y)++;
    y = &w;
  }
  {
    // pointer to constant string
    const char *x = "abc";
    __CPROVER_unconst_type(typeof(x)) y;
    y = x;
  }
  {
    // constant struct
    const struct STRUCTNAME x;
    __CPROVER_unconst_type(typeof(x)) y;
    y.f = 1;
  }
  {
    // pointer to constant scalar
    int z = 0;
    int w = 0;
    const int *x = &z;
    __CPROVER_unconst_type(typeof(x)) y;
    y = &z;
    (*y)++;
    y = &w;
  }
  {
    // constant array
    const int x[] = {1, 2, 3};
    __CPROVER_unconst_type(typeof(x)) y;
    y[0] = 1;
    y[1] = 1;
    y[2] = 1;
  }

  return 0;
}
