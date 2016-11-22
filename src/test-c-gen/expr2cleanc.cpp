/*******************************************************************\

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#include <test-c-gen/expr2cleanc.h>

/*******************************************************************\
Function: expr2cleanct::convert_with_identifier
Inputs:
 src - The type to convert
 identifier - The identifier to use as the type
Outputs: A C decleration of the given type with the right identifier.
Purpose: To convert a type in to ANSI-C but with the identifier in place
         This is useful for array types where the identifier is inside
         the type
\*******************************************************************/
std::string expr2cleanct::convert_with_identifier(const typet &src,
  const std::string &identifier)
{
  return convert_rec(src, c_qualifierst(), identifier);
}

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
Function: expr2cleanct::convert_array_type
Inputs:
 src - The array type being converted
 qualifiers - Type qualifiers
 declarator_str - Type declarators
Outputs: C type decleration for an array
Purpose: To produce a C type decleration for a given array
         The clean version removes specifying the size of the array
 \*******************************************************************/
std::string expr2cleanct::convert_array_type(const typet &src,
const c_qualifierst &qualifiers,
  const std::string &declarator_str)
{
  return expr2ct::convert_array_type(src, qualifiers, declarator_str, false);
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
  return boolean_value?"1":"0";
}
