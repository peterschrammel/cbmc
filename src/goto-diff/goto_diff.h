/*******************************************************************\

Module: GOTO-DIFF Base Class

Author: Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_GOTO_DIFF_BASE_H
#define CPROVER_GOTO_DIFF_BASE_H

#include <goto-programs/goto_model.h>

#include <ostream>

class goto_difft
{
 public:
  virtual bool operator()(
    const goto_modelt &goto_model1,
    const goto_modelt &goto_model2)=0;

  virtual std::ostream &output_functions(std::ostream &out);

 protected:
  std::set<irep_idt> new_functions;  
  std::set<irep_idt> modified_functions;  
  std::set<irep_idt> deleted_functions;  

  virtual std::ostream &output_function(std::ostream &out, 
					const irep_idt &function_name);


};

#endif
