#include <test-c-gen/expr2cleanc.h>

std::string expr2cleanct::convert_struct(const exprt &src, unsigned &precedence)
{
  // Generate the normal struct code except we exlude padding members
  return expr2ct::convert_struct(src, precedence, false);
}
