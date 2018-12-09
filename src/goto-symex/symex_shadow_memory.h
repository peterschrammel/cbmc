/*******************************************************************\

Module: Remove Shadow Memory Instrumentation

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Remove Shadow Memory Instrumentation

#ifndef CPROVER_GOTO_SYMEX_SYMEX_SHADOW_MEMORY_H
#define CPROVER_GOTO_SYMEX_SYMEX_SHADOW_MEMORY_H

#include <util/message.h>

#include <goto-programs/goto_model.h>

class code_function_callt;
class goto_symex_statet;
class namespacet;
class symbol_tablet;

class symex_shadow_memoryt : public messaget
{
public:
  explicit symex_shadow_memoryt(message_handlert &message_handler)
    : messaget(message_handler)
  {
  }

  void symex_field_decl(
    const namespacet &ns,
    goto_symex_statet &state,
    const code_function_callt &code_function_call);

  void symex_get_field(
    const namespacet &ns,
    goto_symex_statet &state,
    const code_function_callt &code_function_call);

  void symex_set_field(
    const namespacet &ns,
    goto_symex_statet &state,
    const code_function_callt &code_function_call);

  void symex_field_static_init();
  void symex_field_dynamic_init();

protected:
  std::map<irep_idt, typet> fields;

  // addresses must remain in sequence
  std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
    address_fields;

#if 0  
  symbol_exprt add_field(
    const namespacet &ns,
    const std::map<irep_idt, typet> &fields,
    const exprt &expr,
    symbol_tablet &symbol_table,
    const irep_idt &function_id,
    const source_locationt &source_location,
    std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
      &address_fields,
    const irep_idt &field_name);

  void initialize_rec(
    const namespacet &ns,
    const std::map<irep_idt, typet> &fields,
    const exprt &expr,
    symbol_tablet &symbol_table,
    const irep_idt &function_id,
    goto_programt::targett target,
    goto_programt &goto_program,
    std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
      &address_fields);
#endif
};

#endif // CPROVER_GOTO_SYMEX_SYMEX_SHADOW_MEMORY_H
