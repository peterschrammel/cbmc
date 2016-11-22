/*******************************************************************\

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#include <test-c-gen/c_test_case_generator.h>

#include <algorithm>

#include <util/options.h>
#include <util/symbol_table.h>
#include <util/substitute.h>
#include <util/irep.h>

#include <goto-programs/goto_functions.h>
#include <goto-programs/goto_program_template.h>

#include <cbmc/bmc.h>

#include <test-c-gen/expr2cleanc.h>

#include <test-c-gen/c_test_file.h>
#include <test-c-gen/function_parameter_builder.h>
#include <test-c-gen/function_return_builder.h>

#include <goto-programs/interpreter_class.h>

/*******************************************************************\
Function: c_test_case_generatort::c_test_case_generatort
Inputs:
  options - Command line options passed to the interpreter
  symbol_table - The symbol table for the GOTO program
  goto_functions - The GOTO program
  tests - The tests (e.g. traces) to create
Purpose: To generate all the tests described in this generator.
\*******************************************************************/
c_test_case_generatort::c_test_case_generatort(
  message_handlert &_message_handler,
  const optionst &options,
  const symbol_tablet &symbol_table,
  const goto_functionst &goto_functions,
  const std::vector<testt> &tests)
  : messaget(_message_handler),
    options(options),
    symbol_table(symbol_table),
    goto_functions(goto_functions),
    tests(tests),
    ns(symbol_table),
    e2c(new expr2cleanct(ns))
{
}

/*******************************************************************\
Function: c_test_case_generatort::operator()
Purpose: To generate all the tests described in this generator.
\*******************************************************************/
void c_test_case_generatort::operator()()
{
  c_test_filet test_file;

  // Add standard includes
  add_includes(test_file);

  size_t test_index=0;

  for(testt &test : tests)
  {
    // Create method for each test
    test.test_function_name=get_test_function_name(test_index);

    generate_test(test, test_file);

    ++test_index;
  }

  add_main_method(test_file, tests);

  status() << test_file.get_file() << eom;
}


/*******************************************************************\
Function: c_test_case_generatort::get_test_function_name
Inputs:
 test_idx - The index of the test
Outputs: The name of the test based on the function being tested and the test
         index.
Purpose: To name a given test.
\*******************************************************************/
const std::string c_test_case_generatort::get_test_function_name(
  size_t test_idx)
{
  const irep_idt called_function_name=get_entry_function_id(goto_functions);
  const symbolt &function_symbol=symbol_table.lookup(called_function_name);

  const std::string pretty_function_name=as_string(function_symbol.pretty_name);
  const std::string sanitised_name=sanitize_function_name(pretty_function_name);

  std::ostringstream test_name;
  test_name << sanitised_name << "_";
  test_name << std::setfill('0') << std::setw(3) << test_idx;

  return test_name.str();
}

/*******************************************************************\
Function: c_test_case_generatort::add_includes
Inputs:
 test_file - The C file to add the includes to
Purpose: To generate hte C test harness for a specific trace with a
         with a specific test generation function
\*******************************************************************/
void c_test_case_generatort::add_includes(c_test_filet &test_file)
{
  // Get the file the entry function is in to include it
  const exprt &entry_func=interpretert::get_entry_function(goto_functions);
  const irep_idt &file_name=entry_func.source_location().get_file();

  std::ostringstream include_line_builder;
  include_line_builder << "#include \"";
  include_line_builder << file_name;
  include_line_builder << "\"";
  test_file.add_line_at_root_indentation(include_line_builder.str());
}

/*******************************************************************\
Function: c_test_case_generatort::generate_test
Inputs:
 trace - The trace to be reproduced by this test
 test_idx - The index of this test
Purpose: To generate hte C test harness for a specific trace with a
         with a specific test generation function
\*******************************************************************/
void c_test_case_generatort::generate_test(const testt &test,
  c_test_filet &test_file)
{
  status() << "Producing test " << test.test_function_name << eom;

  interpretert::input_varst input_vars;

  // Get the function inputs from an interpreter
  interpretert interpreter(symbol_table, goto_functions, this, options);

  interpretert::list_input_varst function_inputs;
  interpretert::side_effects_differencet side_effects;

  const exprt &entry_func=interpretert::get_entry_function(goto_functions);

  input_vars=interpreter.load_counter_example_inputs(test.goto_trace,
    function_inputs,
    side_effects);

  generate_c_test_case_from_inputs(
        entry_func, get_entry_function_id(goto_functions),
        input_vars, test.test_function_name, test_file);
}

/*******************************************************************\
Function: generate_c_test_case_from_inputs
Inputs:
 func_call_expr - The entry point for the users code
 function_id - The ID of the entry point function
 input_vars - The inputs required for the function to recreate the race
 file_name - The name of the file we are testing
Purpose: To generate a test harness to reproduce a specific trace
\*******************************************************************/
void c_test_case_generatort::generate_c_test_case_from_inputs(
  const exprt & func_call_expr,
  const irep_idt &function_id,
  const interpretert::input_varst &input_vars,
  const std::string &test_name,
  c_test_filet &test_file)
{
  test_file.add_function("void", test_name,
    c_test_filet::function_parameter_listt());

  interpretert::input_varst function_inputs=
    filter_inputs_to_function_parameters(input_vars, func_call_expr);

  std::vector<std::string> input_entries;
  if(function_inputs.size() > 0)
  {
    test_file.add_line_at_current_indentation(
      "// Initalize function arguments");

    for(const interpretert::input_entryt &entry : function_inputs)
    {
      function_parameter_buildert function_param(entry, *e2c, symbol_table);

      test_file.add_line_at_current_indentation(
        function_param.get_parameter_declaration());

      input_entries.push_back(function_param.get_parameter_variable_name());
    }
  }

  function_return_buildert return_builder(input_vars, function_id, *e2c);

  if(return_builder.get_function_has_return())
  {
    test_file.add_line_at_current_indentation(
      return_builder.get_return_declaration());
  }

  test_file.add_function_call(function_id, input_entries, return_builder);

  if(return_builder.get_function_has_return())
  {
    const interpretert::input_entryt &return_entry=
      get_function_return_parameter(input_vars);

    add_asserts(test_file, return_entry.second,
      return_builder.get_return_variable_name());
  }

  test_file.end_function();
}

/*******************************************************************\
Function: c_test_case_generatort::get_entry_function_id
Inputs:
 gf - The program under consideration
Outputs: The ID of the entry function where the users code starts
Purpose: To find the ID of the first user function called
\*******************************************************************/
const irep_idt c_test_case_generatort::get_entry_function_id(
  const goto_functionst &gf)
{
  const exprt &func_expr=interpretert::get_entry_function(gf);
  return get_calling_function_name(func_expr);
}

/*******************************************************************\
Function: c_test_case_generatort::get_calling_function_name
Inputs:
 func_expr - The expression calling the entry function
Outputs: The ID of the given function
Purpose: To get the name of a specific function being called
\*******************************************************************/
const irep_idt c_test_case_generatort::get_calling_function_name(
  const exprt &func_expr)
{
  return to_symbol_expr(func_expr).get_identifier();
}

/*******************************************************************\
Function: c_test_case_generatort::sanitise_function_name
Inputs:
 called_function_name - the name of the function being sanitised
Outputs: A clean version of the name.
Purpose: Remove ., <, > from names.
\*******************************************************************/
std::string c_test_case_generatort::sanitize_function_name(
  const std::string called_function_name)
{
  const size_t bracket_offset=called_function_name.find('(');
  std::string isolated_function_name=called_function_name.
    substr(0, bracket_offset);

  // Remove '<', '>', '.' and replace with _ to get valid name
  substitute(isolated_function_name, ".", "_");
  substitute(isolated_function_name, ">", "_");
  substitute(isolated_function_name, "<", "_");

  return isolated_function_name;
}

/*******************************************************************\
 Function: filter_inputs_to_function_parameters
 Inputs:
  all_inputs - All the inputs that have come back from the interpreter
  func_expr - The function call to filter for
Outputs: The inputs which are parameters for the supplied function
Purpose: To pull out from the inputs the relevant ones for the function
         call
\*******************************************************************/
interpretert::input_varst c_test_case_generatort::
  filter_inputs_to_function_parameters(
  const interpretert::input_varst &all_inputs, const exprt &func_expr)
{
  const code_typet::parameterst &params=
      to_code_type(func_expr.type()).parameters();

  interpretert::input_varst filtered_inputs;

  for(const code_typet::parametert &parameter : params)
  {
    const irep_idt &parameter_identifer=parameter.get_identifier();
    typedef interpretert::input_varst::const_iterator const_inputs_itert;
    const_inputs_itert param_iter=all_inputs.find(parameter_identifer);

    // The input vars should include all parameters
    assert(param_iter!=all_inputs.cend());

    filtered_inputs.insert(interpretert::input_entryt(parameter_identifer,
                                        param_iter->second));
  }

  return filtered_inputs;
}

/*******************************************************************\
Function: c_test_case_generatort::get_function_return_parameter
Inputs:
 all_inputs - The variables and their values returned from the
              interpreter
Outputs: The entry representing the return value (throws if no such
         entry).
Pupose: To find the value of the return value of the function
 \*******************************************************************/
interpretert::input_entryt c_test_case_generatort::
  get_function_return_parameter(const interpretert::input_varst &all_inputs)
{
  for(const interpretert::input_entryt &input : all_inputs)
  {
    // TODO(tkiley): this isn't a great way to find returns
    if(input.first=="return'")
    {
      return input;
    }
  }

  throw "no return value when trying to get the input";
}

/*******************************************************************\
Function: c_test_case_generatort::add_asserts
Inputs:
 correct_expression - The expression representing the value expected
 ret_value_var - The name of the variable (including relevant nesting)
Pupose: Add assertions to the assertions list for the return value
 \*******************************************************************/
void c_test_case_generatort::add_asserts(c_test_filet &test_file,
  const exprt &correct_expression,
  std::string ret_value_var)
{
  const typet &type=correct_expression.type();

  if(type.id()==ID_struct)
  {
    add_struct_assert(test_file, correct_expression, ret_value_var);
  }
  else if(type.id()==ID_pointer)
  {
    // TODO(tkiley): this should check pointers dereferenced == something sensible?
    add_simple_assert(test_file, correct_expression, ret_value_var);
  }
  else
  {
    add_simple_assert(test_file, correct_expression, ret_value_var);
  }
}

/*******************************************************************\
Function: c_test_case_generatort::add_struct_assert
Inputs:
 test_file - The C file to add the includes to
 correct_expression - The expression representing the value expected
 ret_value_var - The name of the variable (including relevant nesting)
Purpose: Look through all the components of the struct and add assertions for
         each of them.
 \*******************************************************************/
void c_test_case_generatort::add_struct_assert(c_test_filet &test_file,
  const exprt &correct_expression,
  std::string ret_value_var)
{
  const struct_typet &struct_type=to_struct_type(correct_expression.type());

  exprt::operandst::const_iterator o_it=correct_expression.operands().begin();

  for(const struct_union_typet::componentt &component :
    struct_type.components())
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

    add_asserts(test_file, *o_it, struct_component_name_builder.str());
    ++o_it;
  }
}
