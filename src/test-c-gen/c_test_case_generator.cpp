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


void c_test_case_generatort::run_bmc_generate_test(optionst &options,
                                                  const symbol_tablet &st,
                                                  const goto_functionst &gf,
                                                  bmct &bmc)
{
  const safety_checkert::resultt bmc_run_result = bmc.run(gf);
  // TODO: Deal with response to bmct::run
}

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

  return generator(st);
}

const irep_idt c_test_case_generatort::get_entry_function_id(const goto_functionst &gf)
{
  typedef goto_functionst::function_mapt function_mapt;
  const function_mapt &fm = gf.function_map;

  const irep_idt goto_start_id(goto_functionst::entry_point());

  const function_mapt::const_iterator start_function = fm.find(goto_start_id);

  // Check we found the start function
  assert(start_function != fm.end());

  // Iterate over the instructions to find the function call
  typedef goto_programt::instructionst instructionst;
  const instructionst &start_instructions = start_function->second.body.instructions;

  typedef goto_programt::instructiont instructiont;
  typedef goto_programt::instructionst::const_reverse_iterator rev_instruction_itert;

  rev_instruction_itert last_function_call = std::find_if(
        start_instructions.rbegin(), start_instructions.rend(),
        [&](const instructiont &instruction ) {
    return instruction.code.get_statement() == ID_function_call;
  });

  assert(last_function_call != start_instructions.rend());

  const code_function_callt &func_call=to_code_function_call(last_function_call->code);

  return get_calling_function_name(func_call);
}

const irep_idt c_test_case_generatort::get_calling_function_name(
    const code_function_callt &function_call_instruction)
{
  const exprt &func_expression = function_call_instruction.function();
  return to_symbol_expr(func_expression).get_identifier();
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
