#include "c_test_source_factory.h"

#include <util/irep.h>
#include <util/symbol_table.h>

#include <sstream>

#include <test-c-gen/expr2cleanc.h>

#include <test-c-gen/c_test_file.h>
#include <test-c-gen/function_parameter_builder.h>
#include <test-c-gen/function_return_builder.h>


/*std::string generate_c_test_case_from_inputs(
    const symbol_tablet &st, const irep_idt &func_id,
    bool enters_main, inputst inputs,
    const interpretert::list_input_varst& opaque_function_returns,
    const interpretert::input_var_functionst& input_defn_functions,
    const interpretert::dynamic_typest& dynamic_types,
    const std::string &test_func_name,
    const interpretert::side_effects_differencet &valuesDifference,
    const exprt &assertExpr, bool emitAssert,
    bool disable_mocks,
    bool disable_verify_mocks,
    const optionst::value_listt& mock_classes,
    const optionst::value_listt& no_mock_classes,
    const std::vector<std::string>& goals_reached,
    const std::string& expect_exception)
{
  return "int main(int argc, char* argv) { return 0; }";
}*/

namespace
{
  inputst filter_inputs_to_function_parameters(const inputst &all_inputs,
                                               const exprt &func_expr)
  {
    const code_typet::parameterst &params=
                      to_code_type(func_expr.type()).parameters();

    typedef code_typet::parametert parametert;

    inputst filtered_inputs;

    for(const parametert &parameter : params)
    {
      const irep_idt &parameter_identifer = parameter.get_identifier();
      typedef inputst::const_iterator const_inputs_itert;
      const_inputs_itert param_iter = all_inputs.find(parameter_identifer);

      // The input vars should include all parameters
      assert(param_iter != all_inputs.cend());

      filtered_inputs.insert(input_entryt(parameter_identifer,
                                          param_iter->second));
    }

    return filtered_inputs;
  }

  input_entryt get_return_inputs(const inputst &all_inputs, bool &out_has_return)
  {
    for(const input_entryt &input : all_inputs)
    {
      // TODO: this isn't a great way to find returns
      if(input.first == "return'")
      {
        out_has_return = true;
        return input;
      }
    }

    out_has_return = false;
    return input_entryt(ID_empty, exprt());
  }
}


std::string generate_c_test_case_from_inputs(const symbol_tablet &st,
                                             const exprt & func_call_expr,
                                             const irep_idt &function_id,
                                             const interpretert::input_varst &input_vars,
                                             const irep_idt &file_name)
{
  c_test_filet test_file;
  test_file.emit_standard_includes();
  test_file.emit_file_include(file_name);
  test_file.emit_main_method();

  test_file.add_line_at_current_indentation("printf(\"Running tests...\\n\");");

  namespacet ns(st);
  expr2cleanct e2c(ns);

  inputst function_inputs = filter_inputs_to_function_parameters(input_vars,
                                                                 func_call_expr);

  std::vector<std::string> input_entries;
  for(const input_entryt &entry : function_inputs)
  {
    function_parameter_buildert function_param(entry, e2c);

    test_file.add_line_at_current_indentation(
          function_param.get_parameter_decleration());

    input_entries.push_back(function_param.get_parameter_variable_name());
  }

  function_return_buildert return_builder(input_vars, function_id, e2c);

  if(return_builder.get_function_has_return())
  {
    test_file.add_line_at_current_indentation(
          return_builder.get_return_decleration());
  }

  test_file.add_function(function_id, input_entries, return_builder);

  if(return_builder.get_function_has_return())
  {
    const std::vector<std::string> &assertion_lines =
        return_builder.get_assertion_lines();

    for(const std::string &assert_line : assertion_lines)
    {
      test_file.add_line_at_current_indentation(assert_line);
    }
  }

  test_file.end_main_method();

  return test_file.get_file();
}
