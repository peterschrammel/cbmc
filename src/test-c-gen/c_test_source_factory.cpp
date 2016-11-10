#include "c_test_source_factory.h"

#include <util/irep.h>
#include <util/substitute.h>
#include <util/symbol_table.h>

#include <sstream>

#include <test-c-gen/expr2cleanc.h>


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
  class c_test_filet
  {
public:
    c_test_filet()
      : indentation_character("  "), current_indentation(0), current_file()
    {}

  void emit_standard_includes();
  void emit_file_include(const irep_idt &file_name);
  void emit_main_method();

  void add_line_at_current_indentation(std::string line);
  void add_line_at_root_indentation(std::string line);
  void add_line_at_indentation(std::string line, int level);
  void add_empty_line();

  void add_function(const irep_idt &function_name,
                    const std::vector<std::string> function_inputs);

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
                                  const std::vector<std::string> function_inputs)
  {
    std::ostringstream function_call_builder;
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
}


std::string generate_c_test_case_from_inputs(const symbol_tablet &st,
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

  typedef std::pair<irep_idt, exprt> input_entryt;

  std::vector<std::string> input_entries;
  for(const input_entryt &entry : input_vars)
  {
    std::ostringstream var_assignment_builder;

    std::string type = e2c.convert(entry.second.type());
    var_assignment_builder << type;

    var_assignment_builder << " ";

    std::ostringstream var_name_builder;
    var_name_builder << "arg_";
    var_name_builder << entry.first;

    std::string var_name = var_name_builder.str();

    substitute(var_name, "::", "_");

    var_assignment_builder << var_name;
    var_assignment_builder << " = ";


    // TODO: Boolean's being capitalised
    // TODO: Remove random extra parameter being generated (caused by local var in funciton)
    std::string struct_init = e2c.convert(entry.second);

    var_assignment_builder << struct_init;

    var_assignment_builder << ";";

    test_file.add_line_at_current_indentation(var_assignment_builder.str());

    input_entries.push_back(var_name);
  }


  test_file.add_function(function_id, input_entries);

  test_file.end_main_method();

  return test_file.get_file();
}
