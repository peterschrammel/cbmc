/*******************************************************************\

Module: GOTO-DIFF Base Class

Author: Peter Schrammel

\*******************************************************************/

#include "goto_diff.h"


/*******************************************************************\

Function: goto_difft::output_functions

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::ostream &goto_difft::output_functions(std::ostream &out)
{
  out << "new functions: \n";
  for(std::set<irep_idt>::const_iterator it = new_functions.begin();
      it != new_functions.end(); ++it)
    output_function(out, *it);

  out << "modified functions: \n";
  for(std::set<irep_idt>::const_iterator it = modified_functions.begin();
      it != modified_functions.end(); ++it)
    output_function(out, *it);

  out << "deleted functions: \n";
  for(std::set<irep_idt>::const_iterator it = deleted_functions.begin();
      it != deleted_functions.end(); ++it)
    output_function(out, *it);

  return out;
}

/*******************************************************************\

Function: goto_difft::output_functions

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::ostream &goto_difft::output_function(std::ostream &out, 
					  const irep_idt &function_name)
{
  out << "  " << function_name << "\n";

  return out;
}
