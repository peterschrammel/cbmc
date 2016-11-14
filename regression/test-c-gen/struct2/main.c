#include <stdbool.h>

struct nested_struct
{
  float y;
};

struct example_s
{
  bool b;
  int x;
  struct nested_struct internal;
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

  if(struct_param.internal.y > 0.5f)
  {
  }
}
