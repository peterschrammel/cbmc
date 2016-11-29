/*******************************************************************\

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#include <test-c-gen/c_simple_test_case_generator.h>
#include <test-c-gen/c_test_file.h>
#include <test-c-gen/expr2cleanc.h>
#include <string>
#include <vector>

c_simple_test_case_generatort::c_simple_test_case_generatort(
  message_handlert &_message_handler,
  const optionst &options,
  const symbol_tablet &symbol_table,
  const goto_functionst &goto_functions,
  const testt &test,
  size_t test_index,
  bool using_test_main)
  : c_test_case_generatort(_message_handler, options, symbol_table,
      goto_functions, test, test_index, using_test_main)
{
}

/*******************************************************************\
Function: c_simple_test_case_generatort::add_includes
Inputs:
 test_file - The C file to add the includes to
Purpose: To add the includes required for a simple test harness
\*******************************************************************/
void c_simple_test_case_generatort::add_includes(c_test_filet &test_file)
{
  test_file.add_line_at_root_indentation("#include <assert.h>");
  test_file.add_line_at_root_indentation("#include <stdio.h>");
  test_file.add_empty_line();

  // Add the include for the file after the initial includes
  c_test_case_generatort::add_includes(test_file);
}

/*******************************************************************\
Function: c_simple_test_case_generatort::add_main_method
Inputs:
 test_file - The C file to add the includes to
 tests - The tests that need to be called
Purpose: To create the main method that will call all the tests
         included inside this file
\*******************************************************************/
void c_simple_test_case_generatort::add_main_method(c_test_filet &test_file,
  const testt &test)
{
  start_main(test_file);

  std::ostringstream test_function_call_builder;
  test_function_call_builder << test.test_function_name;
  test_function_call_builder << "();";
  test_file.add_line_at_current_indentation(test_function_call_builder.str());

  test_file.add_empty_line();

  end_main("0", test_file);
}

/*******************************************************************\
Function: c_simple_test_case_generatort::add_simple_assert
Inputs:
 test_file - The C test file being created
 correct_expression - The expression representing the value expected
 return_value_var - The name of the variable (including relevant nesting)
Purpose: Add an assertion for a simple expression
 \*******************************************************************/
void c_simple_test_case_generatort::add_simple_assert(c_test_filet &test_file,
  const exprt &correct_expression,
  std::string return_value_var)
{
  std::ostringstream assert_builder;

  assert_builder << "assert(";
  assert_builder << return_value_var;
  assert_builder << " == ";

  std::string expected_return_value=e2c->convert(correct_expression);

  assert_builder << expected_return_value;
  assert_builder << ");";

  test_file.add_line_at_current_indentation(assert_builder.str());
}
