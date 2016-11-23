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

struct example_s fun()
{
  struct nested_struct my_nest = { .y = 3.14159f };
  struct example_s return_var = { .b = false, .x = 4, .internal = my_nest };
  return return_var;
}
