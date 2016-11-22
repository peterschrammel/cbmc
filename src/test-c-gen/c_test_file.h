/*******************************************************************

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef CPROVER_C_TEST_FILE_H
#define CPROVER_C_TEST_FILE_H

#include <util/irep.h>

class c_test_filet
{
public:
  c_test_filet()
    : current_file(), current_indentation(0), indentation_character("  ")
  {}

  void emit_standard_includes();
  void emit_file_include(const irep_idt &file_name);
  void emit_main_method();

  void add_line_at_current_indentation(std::string line);
  void add_line_at_root_indentation(std::string line);
  void add_line_at_indentation(std::string line, int level);
  void add_empty_line();

  void add_function(const irep_idt &function_name,
    const std::vector<std::string> function_inputs,
    const class function_return_buildert &function_return);

  void add_opening_brace(int level);
  void add_closing_brace(int level);

  void end_main_method();

  std::string get_file() const;

private:
  std::string indentation(int level) const;

  std::string current_file;
  int current_indentation;
  const std::string indentation_character;
};

#endif // CPROVER_C_TEST_FILE_H
