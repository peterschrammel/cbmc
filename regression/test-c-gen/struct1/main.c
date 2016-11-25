#include <stdbool.h>

struct example_s
{
  bool b;
  int x;
};

void fun(struct example_s struct_param)
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
