#include <test-c-gen/expr2cleanc.h>

std::string expr2cleanct::convert_struct(const exprt &src, unsigned &precedence)
{
  // Generate the normal struct code except we exlude padding members
  return expr2ct::convert_struct(src, precedence, false);
}

std::string expr2cleanct::convert_struct_type(const typet &src,
                                              const std::string &qualifer_str,
                                              const std::string &declarator_str)
{
  // Disable including the body of the struct when getting the type
  return expr2ct::convert_struct_type(src, qualifer_str, declarator_str,
                                      false, false);
}
