/*******************************************************************\

Module: Syntactic GOTO-DIFF

Author: Peter Schrammel

\*******************************************************************/

#include "syntactic_diff.h"

/*******************************************************************\

Function: syntactic_difft::operator()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool syntactic_difft::operator()(
    const goto_modelt &goto_model1,
    const goto_modelt &goto_model2)
{
  forall_goto_functions(it, goto_model1.goto_functions)
  {
    goto_functionst::function_mapt::const_iterator f_it = 
      goto_model2.goto_functions.function_map.find(it->first);
    if(f_it == goto_model2.goto_functions.function_map.end())
    {
      deleted_functions.insert(it->first);
      continue;
    }

    if(it->second.body.instructions.size() != 
       f_it->second.body.instructions.size())
    {
      modified_functions.insert(it->first);
      continue;
    }

    goto_programt::instructionst::const_iterator 
      i_it1=it->second.body.instructions.begin();
    for(goto_programt::instructionst::const_iterator 
	  i_it2=f_it->second.body.instructions.begin(); 
	i_it1!=it->second.body.instructions.end() && 
	  i_it2!=f_it->second.body.instructions.end(); 
	++i_it1, ++i_it2)
    {
      if(i_it1->code != i_it2->code &&
  	 i_it1->function != i_it2->function &&
	 i_it1->type != i_it2->type &&
         i_it1->guard != i_it2->guard &&
         i_it1->targets != i_it2->targets)
      {
	modified_functions.insert(it->first);
	break;
      }
    }

  }
  forall_goto_functions(it, goto_model2.goto_functions)
  {
    if(goto_model1.goto_functions.function_map.find(it->first) == 
       goto_model1.goto_functions.function_map.end())
      new_functions.insert(it->first);
  }
  
  return !(new_functions.empty() && 
	   modified_functions.empty() && 
	   deleted_functions.empty());
}


