#include <test-c-gen/c_simple_test_case_generator.h>
#include <test-c-gen/c_test_file.h>

c_simple_test_case_generatort::c_simple_test_case_generatort(
  message_handlert &_message_handler,
  const optionst &options,
  const symbol_tablet &symbol_table,
  const goto_functionst &goto_functions,
  const std::vector<testt> &tests,
  bool using_test_main)
  : c_test_case_generatort(_message_handler, options, symbol_table,
      goto_functions, tests),
    using_test_main(using_test_main)
{

}

void c_simple_test_case_generatort::add_includes(c_test_filet &test_file)
{
  test_file.add_line_at_root_indentation("#include <assert.h>");
  test_file.add_line_at_root_indentation("#include <stdio.h>");

  // We only require stdlib if we are making a main method that isn't called
  // main (in this case we need to call things like exit(0) when the function
  // is done.
  if(using_test_main)
  {
    test_file.add_line_at_root_indentation("#include <stdlib.h>");
  }
  test_file.add_empty_line();

  // Add the include for the file after the initial includes
  c_test_case_generatort::add_includes(test_file);
}
