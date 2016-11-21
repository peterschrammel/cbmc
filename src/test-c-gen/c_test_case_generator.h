/*******************************************************************

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef C_TEST_CASE_GENERATOR_H
#define C_TEST_CASE_GENERATOR_H

#include <functional>
#include <string>

#include <util/message.h>

#include <goto-programs/interpreter_class.h>
#include <test-c-gen/input_entry.h>

class c_test_case_generatort : public messaget
{
public:
  c_test_case_generatort(message_handlert &_message_handler):
    messaget(_message_handler)
{
}

  std::string generate_tests(const class optionst &options,
                             const class symbol_tablet &symbol_table,
                             const class goto_functionst &goto_functions,
                             const class goto_tracet &trace,
                             const size_t test_idx,
                             const std::vector<std::string> &goals);

  const std::string get_test_function_name(const class symbol_tablet &st,
                                           const class goto_functionst &gf,
                                           size_t test_idx);


private:
  std::string generate_c_test_case_from_inputs(const symbol_tablet &symbol_table,
                                               const exprt & func_call_expr,
                                               const irep_idt &function_id,
                                               const interpretert::input_varst &input_vars,
                                               const irep_idt &file_name);

  const irep_idt get_entry_function_id(const class goto_functionst& gf);

  const irep_idt get_calling_function_name(const class exprt &func_expr);

  std::string sanitize_function_name(const std::string called_function_name);
  inputst filter_inputs_to_function_parameters(const inputst &all_inputs,
                                               const exprt &func_expr);
};

#endif
