/*******************************************************************\

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#include <test-c-gen/c_test_file.h>

#include <sstream>

#include <test-c-gen/function_return_builder.h>

/*******************************************************************\
Function: c_test_filet::emit_main_method
Purpose: Start creating the main method
\*******************************************************************/
void c_test_filet::emit_main_method()
{
  add_function("int", "main", {{"int", "argv"}, {"char*", "argv[]"}});
}

/*******************************************************************\
Function: c_test_filet::end_main_method
Purpose: Put the closing brace on the main method
\*******************************************************************/
void c_test_filet::end_main_method()
{
  add_line_at_current_indentation("exit(0);");
  add_line_at_current_indentation("return 0;");
  end_function();
}

/*******************************************************************\
Function: c_test_filet::get_file
Outputs: The full contents of the file
\*******************************************************************/
std::string c_test_filet::get_file() const
{
  return current_file;
}

/*******************************************************************\
Function: c_test_filet::add_opening_brace
Inputs:
 level - The indentation level of the brace to add
Purpose: To add an opening brace and increase indentation
\*******************************************************************/
void c_test_filet::add_opening_brace(int level)
{
  assert(level==current_indentation);
  add_line_at_current_indentation("{");
  ++current_indentation;
}

/*******************************************************************\
Function: c_test_filet::add_closing_brace
Inputs:
 level - The identation level the brace is closing (i.e. the same level
         as the matching opening brace
Purpose: To put the closing brace on an indented block
\*******************************************************************/
void c_test_filet::add_closing_brace(int level)
{
  assert(level==current_indentation - 1);
  --current_indentation;
  add_line_at_current_indentation("}");
}

/*******************************************************************\
Function: c_test_filet::add_line_at_current_indentation
Inputs:
 line - The line to add to the file
Purpose: To add a specific line at the correct indentation
\*******************************************************************/
void c_test_filet::add_line_at_current_indentation(std::string line)
{
  add_line_at_indentation(line, current_indentation);
}

/*******************************************************************\
Function: c_test_filet::add_line_at_root_indentation
Inputs:
 line - The line to add to the file
Purpose: To add a specific line to the file at the root indentation
\*******************************************************************/
void c_test_filet::add_line_at_root_indentation(std::string line)
{
  add_line_at_indentation(line, 0);
}

/*******************************************************************\
Function: c_test_filet::add_line_at_indentation
Inputs:
 line - The line to add to the file
 level - The indentation level to add it at
Purpose: To add a specific line at a specific indentation level
\*******************************************************************/
void c_test_filet::add_line_at_indentation(std::string line, int level)
{
  current_file+=indentation(level)+line+"\n";
}

/*******************************************************************\
Function: c_test_filet::add_empty_line
Purpose: Add an empty line
\*******************************************************************/
void c_test_filet::add_empty_line()
{
  current_file+="\n";
}

/*******************************************************************\
Function: c_test_filet::add_function_call
Inputs:
 function_name - The name of the function to call
 function_inputs - A list to use as the arguments to the function
 function_return - The return builder which, if the function returns,
                   handles the return value assignment
Purpose: Add a function call to the file and potentially assign its
         result to a variable
\*******************************************************************/
void c_test_filet::add_function_call(const irep_idt &function_name,
  const std::vector<std::string> function_inputs,
  const function_return_buildert &function_return)
{
  std::ostringstream function_call_builder;
  if(function_return.get_function_has_return()>0)
  {
    function_call_builder << function_return.get_return_variable_name();
    function_call_builder << " = ";
  }

  function_call_builder << function_name;
  function_call_builder << "(";

  typedef std::vector<std::string>::const_iterator const_input_iterator;
  const_input_iterator last=--function_inputs.cend();

  for(const std::string &entry : function_inputs)
  {

    function_call_builder << entry;
    if(entry!=*last)
    {
      function_call_builder << ", ";
    }
  }

  function_call_builder << ");";

  add_line_at_current_indentation(function_call_builder.str());
}

/*******************************************************************\
Function: c_test_filet::add_function
Inputs:
 return_type - The string of the type the function returns
 function_name - The name of the function
 parameters - A list of key-value pairs of types and parameter names
Purpose: To create a function declartion and opening brace for a function
\*******************************************************************/
void c_test_filet::add_function(const std::string &return_type,
  const std::string function_name,
  const c_test_filet::function_parameter_listt &parameters)
{
  // Only support functions at the root
  assert(current_indentation==0);

  std::ostringstream function_decl_builder;
  function_decl_builder << return_type;
  function_decl_builder << " " << function_name;
  function_decl_builder << "(";

  bool first=true;
  for(const auto &param : parameters)
  {
    if(!first)
    {
      function_decl_builder << ", ";
    }
    else
    {
      first=false;
    }

    function_decl_builder << param.first << " " << param.second;
  }

  function_decl_builder << ")";
  add_line_at_current_indentation(function_decl_builder.str());
  add_opening_brace(0);
}

/*******************************************************************\
Function: c_test_filet::end_function
Purpose: To put the closing brace on a given function, checking braces
         match.
\*******************************************************************/
void c_test_filet::end_function()
{
  add_closing_brace(0);
  assert(current_indentation==0);
  add_empty_line();
}

/*******************************************************************\
Function: c_test_filet::indentation
Inputs:
 level - The level of indentation
Outputs: A string of whitespace characters that would indent text to a
         specific level
Purpose: To create indentation for a specific level
\*******************************************************************/
std::string c_test_filet::indentation(int level) const
{
  std::ostringstream indentation_string;
  for(int i=0; i<level; ++i)
  {
    indentation_string << indentation_character;
  }

  return indentation_string.str();
}
