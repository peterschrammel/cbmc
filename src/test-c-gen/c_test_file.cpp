#include "c_test_file.h"

#include <sstream>

#include <test-c-gen/function_return_builder.h>

void c_test_filet::emit_standard_includes()
{
  add_line_at_root_indentation("#include <assert.h>");
  add_line_at_root_indentation("#include <stdio.h>");
  add_line_at_root_indentation("#include <stdbool.h>");
  add_empty_line();
}

void c_test_filet::emit_file_include(const irep_idt &file_name)
{
  std::ostringstream include_line_builder;
  include_line_builder << "#include \"";
  include_line_builder << file_name;
  include_line_builder << "\"";
  add_line_at_root_indentation(include_line_builder.str());
}

void c_test_filet::emit_main_method()
{
  add_line_at_current_indentation("int main(int argc, char* argv)");
  add_opening_brace(0);
}

void c_test_filet::end_main_method()
{
  add_closing_brace(0);
  assert(current_indentation == 0);
  add_empty_line();
}

std::string c_test_filet::get_file() const
{
  return current_file;
}

void c_test_filet::add_opening_brace(int level)
{
  assert(level == current_indentation);
  add_line_at_current_indentation("{");
  ++current_indentation;
}

void c_test_filet::add_closing_brace(int level)
{
  assert(level == current_indentation - 1);
  --current_indentation;
  add_line_at_current_indentation("}");
}

void c_test_filet::add_line_at_current_indentation(std::string line)
{
  add_line_at_indentation(line, current_indentation);
}

void c_test_filet::add_line_at_root_indentation(std::string line)
{
  add_line_at_indentation(line, 0);
}

void c_test_filet::add_line_at_indentation(std::string line, int level)
{
  current_file += indentation(level) + line + "\n";
}

void c_test_filet::add_empty_line()
{
  current_file += "\n";
}

void c_test_filet::add_function(const irep_idt &function_name,
                                const std::vector<std::string> function_inputs,
                                const function_return_buildert &function_return)
{
  std::ostringstream function_call_builder;
  if(function_return.get_function_has_return() > 0)
  {
    function_call_builder << function_return.get_return_variable_name();
    function_call_builder << " = ";
  }

  function_call_builder << function_name;
  function_call_builder << "(";

  typedef std::vector<std::string>::const_iterator const_input_iterator;
  const_input_iterator last = --function_inputs.cend();

  for(const std::string &entry : function_inputs)
  {

    function_call_builder << entry;
    if(entry != *last)
    {
      function_call_builder << ", ";
    }
  }

  function_call_builder << ");";

  add_line_at_current_indentation(function_call_builder.str());
}

std::string c_test_filet::indentation(int level) const
{
  std::ostringstream indentation_string;
  for(int i = 0; i < level; ++i)
  {
    indentation_string << indentation_character;
  }

  return indentation_string.str();
}
