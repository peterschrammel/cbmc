/*******************************************************************

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef CPROVER_TEST_C_GEN_C_TEST_FILE_H
#define CPROVER_TEST_C_GEN_C_TEST_FILE_H

#include <util/irep.h>
#include <string>
#include <vector>
#include <utility>

class c_test_filet
{
public:
  c_test_filet()
    : current_file(), current_indentation(0), indentation_character("  ")
  {}

  void add_line_at_current_indentation(std::string line);
  void add_line_at_root_indentation(std::string line);
  void add_line_at_indentation(std::string line, int level);
  void add_empty_line();

  void add_function_call(const irep_idt &function_name,
    const std::vector<std::string> function_inputs,
    const class function_return_buildert &function_return);

  typedef std::vector<std::pair<std::string, std::string>>
    function_parameter_listt;

  void add_function(const std::string &return_type,
    const std::string function_name,
    const function_parameter_listt &paramters);

  void end_function();

  void add_opening_brace(int level);
  void add_closing_brace(int level);

  std::string get_file() const;

private:
  std::string indentation(int level) const;

  std::string current_file;
  int current_indentation;
  const std::string indentation_character;
};

#endif // CPROVER_TEST_C_GEN_C_TEST_FILE_H
