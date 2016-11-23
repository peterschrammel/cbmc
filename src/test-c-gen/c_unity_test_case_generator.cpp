/*******************************************************************\

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#include <test-c-gen/c_unity_test_case_generator.h>
#include <test-c-gen/c_test_file.h>
#include <test-c-gen/expr2cleanc.h>
#include <string>
#include <vector>

c_unity_test_case_generatort::c_unity_test_case_generatort(
  message_handlert &_message_handler,
  const optionst &options,
  const symbol_tablet &symbol_table,
  const goto_functionst &goto_functions,
  const std::vector<testt> &tests,
  bool using_test_main)
  : c_test_case_generatort(_message_handler, options, symbol_table,
      goto_functions, tests, using_test_main)
{
}

/*******************************************************************\
Function: c_unity_test_case_generatort::add_includes
Inputs:
 test_file - The C file to add the includes to
Purpose: To add the includes required for a unity test harness
\*******************************************************************/
void c_unity_test_case_generatort::add_includes(c_test_filet &test_file)
{
  test_file.add_line_at_root_indentation("#include \"unity.h\"");
  test_file.add_empty_line();

  // Add the include for the file after the initial includes
  c_test_case_generatort::add_includes(test_file);
}

/*******************************************************************\
Function: c_unity_test_case_generatort::add_main_method
Inputs:
 test_file - The C file to add the includes to
 tests - The tests that need to be called
Purpose: To create the main method that will call all the tests
         included inside this file
\*******************************************************************/
void c_unity_test_case_generatort::add_main_method(c_test_filet &test_file,
  const std::vector<testt> &tests)
{
  // Create main method
  test_file.add_function("int", "test_main",
    {{"int", "argc"}, {"char*", "argv"}});

  test_file.add_line_at_current_indentation("UNITY_BEGIN();");

  for(const testt &test : tests)
  {
    std::ostringstream test_function_call_builder;
    test_function_call_builder << "RUN_TEST(";
    test_function_call_builder << test.test_function_name;
    test_function_call_builder << ");";
    test_file.add_line_at_current_indentation(test_function_call_builder.str());
  }

  test_file.add_empty_line();

  // Shutdown the application correctly
  test_file.add_line_at_current_indentation("int result = UNITY_END();");
  end_main("result", test_file);
}

/*******************************************************************\
Function: c_unity_test_case_generatort::add_unity_assert
Inputs:
 test_file - The C test file being created
 correct_expression - The expression representing the value expected
 return_value_var - The name of the variable (including relevant nesting)
Purpose: Add an assertion for a unity expression
 \*******************************************************************/
void c_unity_test_case_generatort::add_simple_assert(c_test_filet &test_file,
  const exprt &correct_expression,
  std::string return_value_var)
{
  std::ostringstream assert_builder;

  std::string expected_return_value=e2c->convert(correct_expression);
  const typet &type=correct_expression.type();
  if(type.id()==ID_signedbv)
  {
    assert_builder << "TEST_ASSERT_EQUAL_INT(";
    assert_builder << expected_return_value << ", ";
    assert_builder << return_value_var << ");";
  }
  else if(type.id()==ID_unsignedbv)
  {
    assert_builder << "TEST_ASSERT_EQUAL_UINT(";
    assert_builder << expected_return_value << ", ";
    assert_builder << return_value_var << ");";
  }
  else if(type.id()==ID_floatbv)
  {
    // TODO(tkiley): I'm not sure if this is a good way to do this, or if there
    // is a better way. Clearly the test generated will depend on the platform
    // the test is generated rather than the platform the code is run on.
    // One way would be to generate this code in the test but doesn't feel great
    size_t size=type.get_unsigned_int(ID_width) / 8;

    if(size>=sizeof(double))
    {
      assert_builder << "TEST_ASSERT_EQUAL_DOUBLE(";
    }
    else
    {
      assert_builder << "TEST_ASSERT_EQUAL_FLOAT(";
    }

    assert_builder << expected_return_value << ", ";
    assert_builder << return_value_var << ");";
  }
  else if(type.id() == ID_bool)
  {
    if(correct_expression.is_true())
    {
      assert_builder << "TEST_ASSERT_TRUE(";
    }
    else
    {
      assert_builder << "TEST_ASSERT_FALSE(";
    }
    assert_builder << return_value_var;
    assert_builder << ");";
  }
  else if(type.id()==ID_c_bool)
  {
    // TODO(tkiley): Could this logic be moved into the same place as
    // exprt::is_true()?
    constant_exprt const_correct=to_constant_expr(correct_expression);
    const irep_idt &value=const_correct.get_value();
    mp_integer int_value=binary2integer(id2string(value), false);
    if(int_value!=0)
    {
      assert_builder << "TEST_ASSERT_TRUE(";
    }
    else
    {
      assert_builder << "TEST_ASSERT_FALSE(";
    }
    assert_builder << return_value_var;
    assert_builder << ");";
  }
  else
  {
    warning() << "Could not identify special type for ";
    warning() << type.id() << eom;

    assert_builder << "TEST_ASSERT(";
    assert_builder << return_value_var;
    assert_builder << " == ";
    assert_builder << expected_return_value;
    assert_builder << ");";
  }

  test_file.add_line_at_current_indentation(assert_builder.str());
}
