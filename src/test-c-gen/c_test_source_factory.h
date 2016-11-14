#ifndef C_TEST_SOURCE_FACTORY_H_
#define C_TEST_SOURCE_FACTORY_H_

#include <goto-programs/interpreter_class.h>

/**
 * @brief
 *
 * @details
 */
typedef interpretert::input_varst inputst;

/**
 * @brief
 *
 * @details
 *
 * @param st
 * @param func_id
 * @param inputs
 *
 * @return
 */
/*std::string generate_c_test_case_from_inputs(
    const class symbol_tablet &st,
    const class irep_idt &func_id,
    bool main_is_entered,
    class inputst inputs,
    const class interpretert::list_input_varst& opaque_function_returns,
    const class interpretert::input_var_functionst& input_defn_functions,
    const class interpretert::dynamic_typest& dynamic_types,
    const std::string &,
    const class interpretert::side_effects_differencet &,
    const class exprt &,
    bool emitAssert,
    bool disable_mocks,
    bool disable_verify_mocks,
    const class optionst::value_listt& mock_classes,
    const class optionst::value_listt& no_mock_classes,
    const std::vector<std::string>& goals,
    const std::string& expect_exception);*/

/**
 * @brief Produce the C code
 *
 * @details
 *
 * @param st The symbol table of the goto program
 *
 * @return The C code?
 */
std::string generate_c_test_case_from_inputs(
    const class symbol_tablet &st, const exprt &func_call_expr,
    const irep_idt &function_id, const interpretert::input_varst &input_vars,
    const irep_idt &file_name);


#endif // C_TEST_SOURCE_FACTORY_H_
