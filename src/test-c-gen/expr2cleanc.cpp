#include <test-c-gen/expr2cleanc.h>

/*******************************************************************\
Function: expr2cleanct::convert_struct
Inputs:
 src - The struct expression being converted
 precedence
Outputs: C assignment of a struct
Purpose: To produce C code for assigning a struct
         The clean version removes padding variables.
 \*******************************************************************/
std::string expr2cleanct::convert_struct(const exprt &src, unsigned &precedence)
{
  // Generate the normal struct code except we exlude padding members
  return expr2ct::convert_struct(src, precedence, false);
}

/*******************************************************************\
Function: expr2cleanct::convert_struct_type
Inputs:
 src - The struct type being converted
 qualifer_str - Type qualifiers
 declarator_str - Type declarators
Outputs: C type decleration for struct
Purpose: To produce a C type decleration for a given struct
         The clean version removes padding and redefining the
         struct in line.
 \*******************************************************************/
std::string expr2cleanct::convert_struct_type(const typet &src,
                                              const std::string &qualifer_str,
                                              const std::string &declarator_str)
{
  // Disable including the body of the struct when getting the type
  return expr2ct::convert_struct_type(src, qualifer_str, declarator_str,
                                      false, false);
}

/*******************************************************************\
Function: expr2cleanct::convert_constant_bool
Inputs:
 boolean_value - The boolean value to convert
Outputs: C code for representing a true or false value
Purpose: Output C code for a boolean literal.
         Clean version uses lower-case true/false as in stdbool.h
 \*******************************************************************/
std::string expr2cleanct::convert_constant_bool(bool boolean_value)
{
  // This requires #include <stdbool.h>
  if(boolean_value)
  {
    return "true";
  }
  else
  {
    return "false";
  }
}
