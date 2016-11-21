#include "function_return_builder.h"

#include <test-c-gen/expr2cleanc.h>

/*******************************************************************\
Function: function_return_buildert::function_return_buildert
Inputs:
 all_inputs - All the inputs from the interpreter
 function_id - The ID of the function being called
 e2c_converter - The expr2c converter being used to generate C code
Purpose: A builder class for dealing with function returns
 \*******************************************************************/
function_return_buildert::function_return_buildert(const interpretert::input_varst &all_inputs,
                                                   const irep_idt &function_id,
                                                   expr2cleanct &e2c_converter)
  :e2c(e2c_converter)
{
  has_return=false;

  for(const interpretert::input_entryt &input : all_inputs)
  {
    // TODO: this isn't a great way to find returns
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
Function: function_return_buildert::add_assertions_for_expression
Inputs:
 correct_expression - The expression representing the value expected
 ret_value_var - The name of the variable (including relevant nesting)
Pupose: Add assertions to the assertions list for the return value
 \*******************************************************************/
void function_return_buildert::add_assertions_for_expression(const exprt &correct_expression,
                                                             std::string ret_value_var)
{
  assert(has_return);

  const typet &type=correct_expression.type();

  if(type.id()==ID_struct)
  {
    add_assertions_for_struct_expression(correct_expression, ret_value_var);
  }
  else if(type.id()==ID_pointer)
  {
    // TODO - this should check pointers dereferenced == something sensible?
    add_assertions_for_simple_expression(correct_expression, ret_value_var);
  }
  else
  {
    add_assertions_for_simple_expression(correct_expression, ret_value_var);
  }
}

/*******************************************************************\
Function: function_return_buildert::add_assertions_for_struct_expression
Inputs:
 correct_expression - The expression representing the value expected
 ret_value_var - The name of the variable (including relevant nesting)
Purpose: Look through all the components of the struct and add assertions for
         each of them.
 \*******************************************************************/
void function_return_buildert::add_assertions_for_struct_expression(
    const exprt &correct_expression, std::string ret_value_var)
{
  const struct_typet &struct_type=to_struct_type(correct_expression.type());

  exprt::operandst::const_iterator o_it=correct_expression.operands().begin();

  typedef struct_union_typet::componentt componentt;
  for(const componentt &component : struct_type.components())
  {
    // Skip padding parameters
    if(component.get_is_padding())
    {
      ++o_it;
      continue;
    }

    const irep_idt &component_name=component.get_name();

    std::ostringstream struct_component_name_builder;
    struct_component_name_builder << ret_value_var << "." << component_name;

    add_assertions_for_expression(*o_it, struct_component_name_builder.str());
    ++o_it;
  }
}

/*******************************************************************\
Function: function_return_buildert::add_assertions_for_simple_expression
Inputs:
 correct_expression - The expression representing the value expected
 return_value_var - The name of the variable (including relevant nesting)
Purpose: Add an assertion for a simple expression
 \*******************************************************************/
void function_return_buildert::add_assertions_for_simple_expression(
    const exprt &correct_expression, std::string return_value_var)
{
  std::ostringstream assert_builder;

  assert_builder << "assert(";
  assert_builder << return_value_var;
  assert_builder << " == ";

  std::string expected_return_value=e2c.convert(correct_expression);

  assert_builder << expected_return_value;
  assert_builder << ");";

  assertions.push_back(assert_builder.str());
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

/*******************************************************************\
Function: function_return_buildert::get_assertion_lines
Outputs: The asserts on the return type
Purpose: Produce all the assertions (one per line) for the return
 \*******************************************************************/
std::vector<std::string> function_return_buildert::get_assertion_lines()
{
  // if the function doesn't return anything, these other methods shouldn't
  // be called
  assert(has_return);

  assertions.clear();
  add_assertions_for_expression(return_entry.second, return_var_name);

  return assertions;
}
