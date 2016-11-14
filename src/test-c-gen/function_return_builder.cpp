#include "function_return_builder.h"

#include <test-c-gen/expr2cleanc.h>

function_return_buildert::function_return_buildert(const inputst &all_inputs,
                                                   const irep_idt &function_id,
                                                   expr2cleanct &e2c_converter)
  :e2c(e2c_converter)
{
  has_return = false;

  for(const input_entryt &input : all_inputs)
  {
    // TODO: this isn't a great way to find returns
    if(input.first == "return'")
    {
      has_return = true;
      return_entry = input;
      set_return_variable_name(function_id);
      return;
    }
  }

  has_return = false;
}

void function_return_buildert::set_return_variable_name(
    const irep_idt &function_id)
{
  std::ostringstream ret_name_builder;
  ret_name_builder << "ret_";
  ret_name_builder << function_id;
  return_var_name = ret_name_builder.str();
}

bool function_return_buildert::get_function_has_return() const
{
  return has_return;
}

std::string function_return_buildert::get_return_variable_name() const
{
  // if the function doesn't return anything, these other methods shouldn't
  // be called
  assert(has_return);
  return return_var_name;
}

std::string function_return_buildert::get_return_decleration() const
{
  // if the function doesn't return anything, these other methods shouldn't
  // be called
  assert(has_return);

  std::ostringstream ret_var_decleartion_builder;

  std::string type = e2c.convert(return_entry.second.type());
  ret_var_decleartion_builder << type;

  ret_var_decleartion_builder << " ";

  ret_var_decleartion_builder << get_return_variable_name();
  ret_var_decleartion_builder << ";";

  return ret_var_decleartion_builder.str();
}


std::string function_return_buildert::get_assertion_line() const
{
  // if the function doesn't return anything, these other methods shouldn't
  // be called
  assert(has_return);

  std::ostringstream assert_builder;
  assert_builder << "assert(";
  assert_builder << return_var_name;
  // TODO: Should we handle more complex assertions, e.g. checking
  // pointers dereferenced == something sensible?
  assert_builder << " == ";

  std::string expected_return_value = e2c.convert(return_entry.second);

  assert_builder << expected_return_value;
  assert_builder << ");";

  return assert_builder.str();
}



