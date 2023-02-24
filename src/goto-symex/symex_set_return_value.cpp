/*******************************************************************\

Module: Symbolic Execution of ANSI-C

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Symbolic Execution of ANSI-C

#include "goto_symex.h"

void goto_symext::symex_set_return_value(
  statet &state,
  const exprt &return_value)
{
  // we must be inside a function that was called
  PRECONDITION(!state.call_stack().empty());

  framet &frame = state.call_stack().top();
  if(frame.return_value_symbol.has_value())
  {
    const auto lhs = state.l2_rename_rvalues(clean_expr(frame.return_value_symbol.value(), state, true), ns);
    symex_field_local_init(state, to_ssa_expr(lhs));
    symex_assign(state, lhs, return_value, true);
  }
}
