/*******************************************************************

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef C_TEST_SOURCE_FACTORY_H
#define C_TEST_SOURCE_FACTORY_H

#include <test-c-gen/input_entry.h>

std::string generate_c_test_case_from_inputs(
    const class symbol_tablet &symbol_table, const exprt &func_call_expr,
    const irep_idt &function_id, const interpretert::input_varst &input_vars,
    const irep_idt &file_name);


#endif // C_TEST_SOURCE_FACTORY_H_
