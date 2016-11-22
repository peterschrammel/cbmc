/*******************************************************************\

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#include <test-c-gen/function_return_builder.h>

#include <test-c-gen/expr2cleanc.h>

/*******************************************************************\
Function: function_return_buildert::function_return_buildert
Inputs:
 all_inputs - All the inputs from the interpreter
 function_id - The ID of the function being called
 e2c_converter - The expr2c converter being used to generate C code
Purpose: A builder class for dealing with function returns
 \*******************************************************************/
function_return_buildert::function_return_buildert(
  const interpretert::input_varst &all_inputs,
  const irep_idt &function_id,
  expr2cleanct &e2c_converter)
  :e2c(e2c_converter)
{
  has_return=false;

  for(const interpretert::input_entryt &input : all_inputs)
  {
    // TODO(tkiley): this isn't a great way to find returns
    // TODO(tkiley): duplicated in c_test_case_generatort
    if(input.first=="return'")
    {
      has_return=true;
      return_entry=input;
      set_return_variable_name(function_id);
      return;
    }
  }

  has_return=false;
}

/*******************************************************************\
Function: function_return_buildert::set_return_variable_name
Inputs:
 function_id - The name of the function
Purpose: Store the variable named used to store the result of the function
 \*******************************************************************/
void function_return_buildert::set_return_variable_name(
  const irep_idt &function_id)
{
  std::ostringstream ret_name_builder;
  ret_name_builder << "ret_";
  ret_name_builder << function_id;
  return_var_name=ret_name_builder.str();
}

/*******************************************************************\
Function: function_return_buildert::get_function_has_return
Outputs: Returns true if the function has a return value
Purpose: We only store and validate function returns if the
         function actually returns something.
 \*******************************************************************/
bool function_return_buildert::get_function_has_return() const
{
  return has_return;
}

/*******************************************************************\
Function: function_return_buildert::get_return_variable_name
Outputs: The variable used to store the function return
Purpose: For getting the name used to store the function return
 \*******************************************************************/
std::string function_return_buildert::get_return_variable_name() const
{
  // if the function doesn't return anything, these other methods shouldn't
  // be called
  assert(has_return);
  return return_var_name;
}

/*******************************************************************\
Function: function_return_buildert::get_return_declaration
Outputs: The declaration line
Purpose: Get the declaration line for the return variable
 \*******************************************************************/
std::string function_return_buildert::get_return_declaration() const
{
  // if the function doesn't return anything, these other methods shouldn't
  // be called
  assert(has_return);

  std::ostringstream ret_var_declaration_builder;

  std::string type=e2c.convert(return_entry.second.type());
  ret_var_declaration_builder << type;

  ret_var_declaration_builder << " ";

  ret_var_declaration_builder << get_return_variable_name();
  ret_var_declaration_builder << ";";

  return ret_var_declaration_builder.str();
}
