#include "function_parameter_builder.h"

#include <sstream>

#include <util/substitute.h>

#include <test-c-gen/expr2cleanc.h>

/*******************************************************************\
Function:  function_parameter_buildert::function_parameter_buildert
Inputs:
 entry - The parameter to fill
 e2c - The C converter class to use
Purpose: To create relevant parts for assigning a specific value as a
         function parameter
 \*******************************************************************/
function_parameter_buildert::function_parameter_buildert(
    const input_entryt &entry, expr2cleanct &e2c)
{
  std::ostringstream var_assignment_builder;

  std::ostringstream var_name_builder;
  var_name_builder << "arg_";
  var_name_builder << entry.first;

  variable_name = var_name_builder.str();
  substitute(variable_name, "::", "_");

  std::string type = e2c.convert_with_identifier(entry.second.type(),
                                                 variable_name);
  var_assignment_builder << type;

  var_assignment_builder << " = ";

  std::string struct_init = e2c.convert(entry.second);

  var_assignment_builder << struct_init;

  var_assignment_builder << ";";

  variable_decleration_line = var_assignment_builder.str();
}

/*******************************************************************\
Function:  function_parameter_buildert::get_parameter_decleration
Outputs: The decleration line of the parameter
Purpose: Get the decleration line of the parameter where its value is
         assigned
 \*******************************************************************/
std::string function_parameter_buildert::get_parameter_decleration() const
{
  return variable_decleration_line;
}

/*******************************************************************\
Function:  function_parameter_buildert::get_parameter_variable_name
Outputs: The variable name
Purpose: To get the name being used in the decleration so can be used as
         parameter for the function call
 \*******************************************************************/
std::string function_parameter_buildert::get_parameter_variable_name() const
{
  return variable_name;
}

