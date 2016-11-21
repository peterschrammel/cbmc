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
Function: c_test_case_generatort::get_test_function_name
Inputs:
 st - The symbol table of the trace
 gf - The goto code the trace is over
 test_idx - The index of the test
Outputs: The name of the test based on the function being tested and the test
         index.
Purpose: To name a given test.
\*******************************************************************/
const std::string c_test_case_generatort::get_test_function_name(
    const symbol_tablet &st, const goto_functionst &gf, size_t test_idx)
{
  const irep_idt called_function_name=get_entry_function_id(gf);

  const std::string pretty_function_name=as_string(st.lookup(called_function_name).pretty_name);
  const std::string sanitised_name=sanitize_function_name(pretty_function_name);

  // In the java version we hash the function name and append it, but I don't
  // see a reason for this
  // It also pads the functions with 0's, again, I'm not sure why

  std::ostringstream test_name;

  test_name << sanitised_name << "_" << test_idx;

  return test_name.str();
}

/*******************************************************************\
Function: c_test_case_generatort::generate_tests
Inputs:
 options - The command line options (used by the interpreter)
 st - The symbol table of the trace
 gf - The goto functions the trace is over
 trace - The trace to be reproduced by this test
 test_idx - The index of this test
 goals - The goals this test will cover
Outputs:  An executable C test harness
Purpose: To generate hte C test harness for a specific trace with a
         with a specific test generation function
\*******************************************************************/
std::string c_test_case_generatort::generate_tests(const optionst &options,
                                                   const symbol_tablet &symbol_table,
                                                   const goto_functionst &goto_functions,
                                                   const goto_tracet &trace,
                                                   const size_t test_idx,
                                                   const std::vector<std::string> &goals_reached)
{
  status() << "Producing test " << test_idx << eom;

  interpretert::input_varst input_vars;

  // Get the function inputs from an interpreter
  interpretert interpreter(symbol_table, goto_functions, this, options);

  interpretert::list_input_varst function_inputs;
  interpretert::side_effects_differencet side_effects;

  input_vars=interpreter.load_counter_example_inputs(trace, function_inputs,
                                                       side_effects);

  // Get the file the entry function is in to include it
  const exprt &entry_func=interpretert::get_entry_function(goto_functions);
  const irep_idt &file_name=entry_func.source_location().get_file();

  std::string test_contents=generate_c_test_case_from_inputs(
        symbol_table, entry_func, get_entry_function_id(goto_functions),
        input_vars, file_name);

  return test_contents;
}

/*******************************************************************\
Function: generate_c_test_case_from_inputs
Inputs:
 st - The symbol table
 func_call_expr - The entry point for the users code
 function_id - The ID of the entry point function
 input_vars - The inputs required for the function to recreate the race
 file_name - The name of the file we are testing
Outputs: An executable C test harness
Purpose: To generate a test harness to reproduce a specific trace
\*******************************************************************/
std::string c_test_case_generatort::generate_c_test_case_from_inputs(
    const symbol_tablet &symbol_table,
    const exprt & func_call_expr,
    const irep_idt &function_id,
    const interpretert::input_varst &input_vars,
    const irep_idt &file_name)
{
  c_test_filet test_file;
  test_file.emit_standard_includes();
  test_file.emit_file_include(file_name);
  test_file.emit_main_method();

  test_file.add_line_at_current_indentation("printf(\"Running tests...\\n\");");

  namespacet ns(symbol_table);
  expr2cleanct e2c(ns);

  interpretert::input_varst function_inputs=
      filter_inputs_to_function_parameters(input_vars, func_call_expr);

  std::vector<std::string> input_entries;
  for(const interpretert::input_entryt &entry : function_inputs)
  {
    function_parameter_buildert function_param(entry, e2c, symbol_table);

    test_file.add_line_at_current_indentation(
      function_param.get_parameter_declaration());

    input_entries.push_back(function_param.get_parameter_variable_name());
  }

  function_return_buildert return_builder(input_vars, function_id, e2c);

  if(return_builder.get_function_has_return())
  {
    test_file.add_line_at_current_indentation(
      return_builder.get_return_declaration());
  }

  test_file.add_function(function_id, input_entries, return_builder);

  if(return_builder.get_function_has_return())
  {
    const std::vector<std::string> &assertion_lines=
      return_builder.get_assertion_lines();

    for(const std::string &assert_line : assertion_lines)
    {
      test_file.add_line_at_current_indentation(assert_line);
    }
  }

  test_file.end_main_method();

  return test_file.get_file();
}

/*******************************************************************\
Function: c_test_case_generatort::get_entry_function_id
Inputs:
 gf - The program under consideration
Outputs: The ID of the entry function where the users code starts
Purpose: To find the ID of the first user function called
\*******************************************************************/
const irep_idt c_test_case_generatort::get_entry_function_id(const goto_functionst &gf)
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
  std::string isolated_function_name=called_function_name.substr(0, bracket_offset);

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
interpretert::input_varst c_test_case_generatort::filter_inputs_to_function_parameters(
    const interpretert::input_varst &all_inputs, const exprt &func_expr)
{
  const code_typet::parameterst &params=
      to_code_type(func_expr.type()).parameters();

  interpretert::input_varst filtered_inputs;

  typedef code_typet::parametert parametert;
  for(const parametert &parameter : params)
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
