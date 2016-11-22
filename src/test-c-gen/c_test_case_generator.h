/*******************************************************************\

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef CPROVER_TEST_C_GEN_C_TEST_CASE_GENERATOR_H
#define CPROVER_TEST_C_GEN_C_TEST_CASE_GENERATOR_H

#include <functional>
#include <string>

#include <util/message.h>

#include <goto-programs/interpreter_class.h>

// TODO: Since this is also used by Java test generation it should
// be moved into its own file
struct testt
{
  goto_tracet goto_trace;
  std::vector<irep_idt> covered_goals;
  std::string source_code;
  std::string test_function_name;
};

class c_test_case_generatort : public messaget
{
public:
  c_test_case_generatort(message_handlert &_message_handler,
    const class optionst &options,
    const class symbol_tablet &symbol_table,
    const class goto_functionst &goto_functions,
    const std::vector<testt> &tests);

  void operator()();

  void generate_test(const testt &test, class c_test_filet &test_file);

  const std::string get_test_function_name(size_t test_idx);

protected:
  virtual void add_includes(c_test_filet &test_file);

private:
  void generate_c_test_case_from_inputs(const exprt & func_call_expr,
    const irep_idt &function_id,
    const interpretert::input_varst &input_vars, const std::string &test_name,
    class c_test_filet &test_file);

  const irep_idt get_entry_function_id(const class goto_functionst& gf);

  const irep_idt get_calling_function_name(const class exprt &func_expr);

  std::string sanitize_function_name(const std::string called_function_name);

  interpretert::input_varst filter_inputs_to_function_parameters(
    const interpretert::input_varst &all_inputs, const exprt &func_expr);

  const class optionst &options;
  const class symbol_tablet &symbol_table;
  const class goto_functionst &goto_functions;
  std::vector<testt> tests;
};

#endif // CPROVER_TEST_C_GEN_C_TEST_CASE_GENERATOR_H
