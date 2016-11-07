#include <test-c-gen/c_test_case_generator.h>

#include <algorithm>

#include <util/options.h>
#include <util/symbol_table.h>
#include <util/substitute.h>
#include <util/irep.h>

#include <goto-programs/goto_functions.h>
#include <goto-programs/goto_program_template.h>

#include <cbmc/bmc.h>

#include "c_test_source_factory.h"

#include <goto-programs/interpreter_class.h>

std::string c_test_case_generatort::generate_tests(const optionst &options,
                                                   const symbol_tablet &st,
                                                   const goto_functionst &gf,
                                                   const goto_tracet &trace,
                                                   const size_t test_idx,
                                                   const std::vector<std::string> &goals)
{
  test_case_generatort generator = generate_c_test_case_from_inputs;
  return generate_tests_with_generator(options, st, gf, trace, generator,
                                test_idx, goals);
}

const std::string c_test_case_generatort::get_test_function_name(
    const symbol_tablet &st, const goto_functionst &gf, size_t test_idx)
{
  const irep_idt called_function_name = get_entry_function_id(gf);

  const std::string pretty_function_name = as_string(st.lookup(called_function_name).pretty_name);
  const std::string sanitised_name = sanitise_function_name(pretty_function_name);

  // In the java version we hash the function name and append it, but I don't
  // see a reason for this
  // It also pads the functions with 0's, again, I'm not sure why

  std::ostringstream test_name;

  test_name << sanitised_name << "_" << test_idx;

  return test_name.str();
}

std::string c_test_case_generatort::generate_tests_with_generator(const optionst &options,
                                                                  const symbol_tablet &st,
                                                                  const goto_functionst &gf,
                                                                  const goto_tracet &trace,
                                                                  const test_case_generatort generator,
                                                                  size_t test_idx,
                                                                  std::vector<std::string> goals_reached)
{
  status() << "Producing test " << test_idx << eom;

  interpretert::input_varst input_vars;

  for(const auto & step : trace.steps)
  {
    if(step.is_input())
    {
      typedef std::pair<irep_idt, exprt> input_entryt;
      input_vars.insert(input_entryt(step.io_id, step.io_args.front()));

      /*namespacet ns(st);
      std::string value = from_expr(ns, "", step.io_args.front());*/

      //status() << value << eom;
    }
  }








  // Get the file the entry function is in to include it
  const exprt &entry_func = interpretert::get_entry_function(gf);
  const irep_idt &file_name = entry_func.source_location().get_file();

  return generator(st, get_entry_function_id(gf), input_vars, file_name);
}

const irep_idt c_test_case_generatort::get_entry_function_id(const goto_functionst &gf)
{
  const exprt &func_expr = interpretert::get_entry_function(gf);
  return get_calling_function_name(func_expr);
}

const irep_idt c_test_case_generatort::get_calling_function_name(
    const exprt &func_expr)
{
  return to_symbol_expr(func_expr).get_identifier();
}

std::string c_test_case_generatort::sanitise_function_name(
    const std::string called_function_name)
{
  const size_t bracket_offset = called_function_name.find('(');
  std::string isolated_function_name = called_function_name.substr(0, bracket_offset);

  // Remove '<', '>', '.' and replace with _ to get valid name
  substitute(isolated_function_name, ".", "_");
  substitute(isolated_function_name, ">", "_");
  substitute(isolated_function_name, "<", "_");

  return isolated_function_name;
}
