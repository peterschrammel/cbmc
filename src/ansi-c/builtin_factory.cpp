/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include "builtin_factory.h"
#include "ansi_c_internal_additions.h"

#include "ansi_c_parser.h"
#include "ansi_c_typecheck.h"

#include <cstring>
#include <ostream>
#include <sstream>

const char windows_headers[]=
  "int __noop();\n"
  "int __assume(int);\n";

//! Advance to the next line
static const char *next_line(const char *line)
{
  const char *end=strchr(line, '\n');

  if(end==NULL)
    return strchr(line, 0);
  else
    return end+1;
}

//! Look for given pattern in given string.
//! Add line with pattern to 'out' if found.
//! \return 'true' if found
static bool find_pattern(
  const std::string &pattern,
  const char *header_file,
  std::ostream &out)
{
  // read line-by-line
  const char *line, *line_end;

  for(line=header_file; *line!=0; line=line_end)
  {
    line_end=next_line(line);

    // skip spaces
    while(*line==' ')
      line++;

    if(line[0]=='/' && line[1]=='/') // comment
      continue;

    for(const char *p=line; p<line_end; p++)
    {
      if(strncmp(p, pattern.c_str(), pattern.size())==0)
      {
        // copy the entire line to out
        for(const char *s=line; s!=line_end; s++)
          out << *s;

        return true; // done, found
      }
    }
  }

  return false;
}

static bool convert(
  const irep_idt &identifier,
  const std::ostringstream &s,
  symbol_tablet &symbol_table,
  message_handlert &message_handler)
{
  std::istringstream in(s.str());

  ansi_c_parser.clear();
  ansi_c_parser.set_file(ID_built_in);
  ansi_c_parser.in=&in;
  ansi_c_parser.set_message_handler(message_handler);
  ansi_c_parser.for_has_scope=config.ansi_c.for_has_scope;
  ansi_c_parser.cpp98=false; // it's not C++
  ansi_c_parser.cpp11=false; // it's not C++
  ansi_c_parser.mode=config.ansi_c.mode;

  ansi_c_scanner_init();

  if(ansi_c_parser.parse())
    return true;

  symbol_tablet new_symbol_table;

  // very recursive!
  if(ansi_c_typecheck(
    ansi_c_parser.parse_tree,
    new_symbol_table,
    "", // module
    message_handler))
  {
    return true;
  }

  // we should now have a new symbol
  symbol_tablet::symbolst::const_iterator s_it=
    new_symbol_table.symbols.find(identifier);

  if(s_it==new_symbol_table.symbols.end())
  {
    messaget message(message_handler);
    message.error() << "failed to produce built-in symbol `"
                    << identifier << '\'' << messaget::eom;
    return true;
  }

  // copy the new symbol
  symbol_table.add(s_it->second);

  return false;
}

//! Check whether given identifier is a compiler built-in.
//! If so, add declaration to symbol table.
//! \return 'true' on error
bool builtin_factory(
  const irep_idt &identifier,
  symbol_tablet &symbol_table,
  message_handlert &mh)
{
  // we search for "space" "identifier" "("
  const std::string pattern=' '+id2string(identifier)+'(';

  std::ostringstream s;

  std::string code;
  ansi_c_internal_additions(code);
  s << code;
  
  if(find_pattern(pattern, gcc_builtin_headers_generic, s))
    return convert(identifier, s, symbol_table, mh);

  if(find_pattern(pattern, gcc_builtin_headers_mem_string, s))
    return convert(identifier, s, symbol_table, mh);

  if(find_pattern(pattern, gcc_builtin_headers_omp, s))
    return convert(identifier, s, symbol_table, mh);

  if(find_pattern(pattern, gcc_builtin_headers_tm, s))
    return convert(identifier, s, symbol_table, mh);

  if(find_pattern(pattern, gcc_builtin_headers_ubsan, s))
    return convert(identifier, s, symbol_table, mh);

  if(find_pattern(pattern, gcc_builtin_headers_ia32, s))
    return convert(identifier, s, symbol_table, mh);

  if(find_pattern(pattern, gcc_builtin_headers_ia32_2, s))
    return convert(identifier, s, symbol_table, mh);

  if(find_pattern(pattern, gcc_builtin_headers_ia32_3, s))
    return convert(identifier, s, symbol_table, mh);

  if(find_pattern(pattern, gcc_builtin_headers_ia32_4, s))
    return convert(identifier, s, symbol_table, mh);

  return true;
}
