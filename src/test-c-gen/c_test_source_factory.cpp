#include "c_test_source_factory.h"

#include <util/symbol_table.h>
#include <util/irep.h>


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


std::string generate_c_test_case_from_inputs(
    const symbol_tablet &st)
{
  return "int main(int argc, char* argv) { return 0; }";
}
