#include "remove_printf.h"
#include <goto-programs/goto_model.h>

static bool is_printf_or_cseq_message(goto_programt::targett target)
{
  if(!target->is_function_call())
    return false;

  const code_function_callt &code_function_call =
    to_code_function_call(target->code);
   const exprt &function = code_function_call.function();

  if(function.id() != ID_symbol)
    return false;

  const irep_idt &identifier = to_symbol_expr(function).get_identifier();

  return identifier == "__CSEQ_message" || identifier == "printf";
}

void remove_printf(goto_modelt &goto_model)
{
  Forall_goto_functions(f_it, goto_model.goto_functions)
  {
    goto_programt &goto_program = f_it->second.body;
    Forall_goto_program_instructions(target, goto_program)
    {
      if (is_printf_or_cseq_message(target))
      {
        target->turn_into_skip();
      }
    }
  }
}
