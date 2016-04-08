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

std::ostream &goto_difft::output_functions(std::ostream &out) const
 {
   switch(ui)
   {
     case ui_message_handlert::PLAIN:
     {
       out << "new functions: \n";
      for(irep_id_sett::const_iterator it = new_functions.begin();
 	  it != new_functions.end(); ++it)
      {
	const goto_programt &program = 
	  goto_model2.goto_functions.function_map.at(*it).body;
	out << "  " 
	    << program.instructions.begin()->source_location.get_file()
	    << ": " << *it << "\n";
      }
 
       out << "modified functions: \n";
      for(irep_id_sett::const_iterator it = modified_functions.begin();
 	  it != modified_functions.end(); ++it)
      {
	const goto_programt &program = 
	  goto_model2.goto_functions.function_map.at(*it).body;
	out << "  " 
	    << program.instructions.begin()->source_location.get_file()
	    << ": " << *it << "\n";
      }
 
       out << "deleted functions: \n";
      for(irep_id_sett::const_iterator it = deleted_functions.begin();
 	  it != deleted_functions.end(); ++it)
      {
	const goto_programt &program = 
	  goto_model2.goto_functions.function_map.at(*it).body;
	out << "  " 
	    << program.instructions.begin()->source_location.get_file()
	    << ": " << *it << "\n";
      }
       break;
     }
     case ui_message_handlert::JSON_UI:
     {
       {
	json_arrayt result;
	convert_function_group(result, new_functions);
	out << ",\n\"NewFunctions\": " << result;
       }
       {
	json_arrayt result;
	convert_function_group(result, modified_functions);
	out << ",\n\"ModifiedFunctions\": " << result;
       }
       {
	json_arrayt result;
	convert_function_group(result, deleted_functions);
	out << ",\n\"DeletedFunctions\": " << result;
       }
       break;
    }
    default:
      assert(false);
  }
  return out;
}

/*******************************************************************\

Function: goto_difft::convert_function_group

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void goto_difft::convert_function_group(json_arrayt &result, 
			const irep_id_sett &function_group) const
{
  for(irep_id_sett::const_iterator it = function_group.begin();
      it != function_group.end(); ++it)
  {
    convert_function(result.push_back(jsont()).make_object(), *it);
  }
}

/*******************************************************************\

Function: goto_difft::convert_functions

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void goto_difft::convert_function(json_objectt &result, 
		  const irep_idt &function_name) const
{
  const goto_programt &program = 
    goto_model2.goto_functions.function_map.at(function_name).body;
  result["File"] = json_stringt(
    id2string(program.instructions.begin()->source_location.get_file()));
  result["Function"] = json_stringt(id2string(function_name));
}
