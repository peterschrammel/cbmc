#include <stdbool.h>

struct example_s
{
  bool b;
  int x;
};

struct example_s fun()
{
  struct example_s return_var = { .b = true, .x = 4 };
  return return_var;
}
