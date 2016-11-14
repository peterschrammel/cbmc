/*******************************************************************

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef FUNCTION_PARAMETER_BUILDER_H
#define FUNCTION_PARAMETER_BUILDER_H

#include <test-c-gen/input_entry.h>

class function_parameter_buildert
{
public:
  function_parameter_buildert(const input_entryt &entry,
                              class expr2cleanct &e2c);

  std::string get_parameter_decleration() const;
  std::string get_parameter_variable_name() const;

private:
  std::string variable_name;
  std::string variable_decleration_line;

};

#endif // FUNCTION_PARAMETER_BUILDERT_H
