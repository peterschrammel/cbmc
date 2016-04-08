/*******************************************************************\

Module: GOTO-DIFF Base Class

Author: Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_GOTO_DIFF_BASE_H
#define CPROVER_GOTO_DIFF_BASE_H

#include <goto-programs/goto_model.h>
#include <langapi/language_ui.h>
#include <util/message.h>

#include <ostream>

class goto_difft : public messaget
{
 public:
  goto_difft()
    :
    ui(ui_message_handlert::PLAIN)
    {}

  virtual bool operator()(
    const goto_modelt &goto_model1,
    const goto_modelt &goto_model2) 
    { assert(false); }

  void set_ui(language_uit::uit _ui) { ui=_ui; }

  virtual std::ostream &output_functions(std::ostream &out);

 protected:
  language_uit::uit ui;

  std::set<irep_idt> new_functions;  
  std::set<irep_idt> modified_functions;  
  std::set<irep_idt> deleted_functions;  

  virtual std::ostream &output_function(std::ostream &out, 
					const irep_idt &function_name);


};

#endif
