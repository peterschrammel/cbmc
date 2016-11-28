/*******************************************************************

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef CPROVER_TEST_C_GEN_FUNCTION_PARAMETER_BUILDER_H
#define CPROVER_TEST_C_GEN_FUNCTION_PARAMETER_BUILDER_H

#include <goto-programs/interpreter_class.h>
#include <test-c-gen/expr2cleanc.h>
#include <string>

class function_parameter_buildert
{
public:
  function_parameter_buildert(const interpretert::input_entryt &entry,
    expr2cleanct &e2c,
    const symbol_tablet &symbol_table);

  std::string get_parameter_declaration() const;
  std::string get_parameter_variable_name() const;

private:
  std::string variable_name;
  std::string variable_declaration_line;
};

#endif // CPROVER_TEST_C_GEN_FUNCTION_PARAMETER_BUILDERT_H
