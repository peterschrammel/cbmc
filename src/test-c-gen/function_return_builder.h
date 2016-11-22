/*******************************************************************

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef CPROVER_TEST_C_GEN_FUNCTION_RETURN_BUILDER_H
#define CPROVER_TEST_C_GEN_FUNCTION_RETURN_BUILDER_H

#include <goto-programs/interpreter_class.h>
#include <string>
#include <vector>

class function_return_buildert
{
public:
  function_return_buildert(const interpretert::input_varst &all_inputs,
    const irep_idt &function_id,
    class expr2cleanct &e2c_converter);

  bool get_function_has_return() const;
  std::string get_return_declaration() const;
  std::string get_return_variable_name() const;

  interpretert::input_entryt get_function_return_parameter() const;

private:
  void set_return_variable_name(const irep_idt &function_id);

  class expr2cleanct &e2c;
  bool has_return;
  interpretert::input_entryt return_entry;
  std::string return_var_name;

  std::vector<std::string> assertions;
};

#endif // CPROVER_TEST_C_GEN_FUNCTION_RETURN_BUILDERT_H
