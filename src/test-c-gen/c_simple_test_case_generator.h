/*******************************************************************\

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef CPROVER_TEST_C_GEN_C_SIMPLE_TEST_CASE_GENERATOR_H
#define CPROVER_TEST_C_GEN_C_SIMPLE_TEST_CASE_GENERATOR_H

#include <test-c-gen/c_test_case_generator.h>
#include <string>
#include <vector>

class c_simple_test_case_generatort : public c_test_case_generatort
{
public:
  c_simple_test_case_generatort(message_handlert &_message_handler,
    const class optionst &options,
    const class symbol_tablet &symbol_table,
    const class goto_functionst &goto_functions,
    const testt &test,
    size_t test_index,
    bool using_test_main);

protected:
  virtual void add_includes(class c_test_filet &test_file) override;
  virtual void add_main_method(c_test_filet &test_file,
    const testt &test) override;

  virtual void add_simple_assert(class c_test_filet &test_file,
    const exprt &correct_expression,
    std::string ret_value_var) override;
};

#endif // CPROVER_TEST_C_GEN_C_SIMPLE_TEST_CASE_GENERATOR_H
