#include <stdbool.h>

typedef struct
{
  bool b;
  int x;
} example_s;

void fun(example_s struct_param)
{
  if(!struct_param.b)
  {
  }
  if(struct_param.x > 0)
  {
    struct_param.b = false;
  }

  if(struct_param.b)
  {
  }
}
