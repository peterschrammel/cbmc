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

typedef std::function<std::string(
    const symbol_tablet &, // the symbol table for the trace
    const exprt &, // the expression of the function call
    const irep_idt &, // the id of the function
    const interpretert::input_varst &, // the input values for the function
    const irep_idt & // The file name the users function is in
    )> test_case_generatort;

class c_test_case_generatort : public messaget
{
public:
  c_test_case_generatort(message_handlert & _message_handler):
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
  std::string generate_tests_with_generator(const class optionst &options,
                                            const class symbol_tablet &symbol_table,
                                            const class goto_functionst &goto_functions,
                                            const class goto_tracet &trace,
                                            const test_case_generatort generator,
                                            size_t test_idx,
                                            std::vector<std::string> goals_reached);

  const irep_idt get_entry_function_id(const class goto_functionst& gf);

  const irep_idt get_calling_function_name(const class exprt &func_expr);

  std::string sanitize_function_name(const std::string called_function_name);
};

#endif
