/*******************************************************************\

Module: Symbolic Execution

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Symbolic Execution

#include "goto_symex.h"

#include <util/exception_utils.h>

void goto_symext::symex_atomic_begin(statet &state)
{
  if(!state.reachable)
    return;

  if(state.atomic_section_id != 0)
    throw incorrect_goto_program_exceptiont(
      "we don't support nesting of atomic sections",
      state.source.pc->source_location());

  state.atomic_section_id=++atomic_section_counter;
  state.read_in_atomic_section.clear();
  state.written_in_atomic_section.clear();

  target.atomic_begin(
      state.guard.as_expr(),
      atomic_section_counter,
      state.source);
}

void goto_symext::symex_atomic_end(statet &state)
{
  if(!state.reachable)
    return;

  if(state.atomic_section_id == 0)
    throw incorrect_goto_program_exceptiont(
      "ATOMIC_END unmatched", state.source.pc->source_location());

  const unsigned atomic_section_id=state.atomic_section_id;
  state.atomic_section_id=0;

  target.atomic_end(
    state.guard.as_expr(),
    atomic_section_id,
    state.source);
}
