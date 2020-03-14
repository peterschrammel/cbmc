/*******************************************************************\

Module: Symex __CPROVER_types_equal 

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Symex __CPROVER_types_equal

#include "goto_symex.h"

#include <langapi/language_util.h>
#include <util/arith_tools.h>
#include <util/invariant.h>

void goto_symext::symex_types_equal(
  const namespacet &ns,
  goto_symex_statet &state,
  const code_function_callt &code_function_call)
{
  INVARIANT(
    code_function_call.arguments().size() == 2,
    CPROVER_PREFIX "types_equal requires 2 arguments");
  const exprt &expr1 = code_function_call.arguments()[0];
  const exprt &expr2 = code_function_call.arguments()[1];

  const bool result = expr1.type() == expr2.type();

  log.debug() << CPROVER_PREFIX << "types_equal("
              << from_type(ns, "", expr1.type()) << ", "
              << from_type(ns, "", expr2.type())
              << ") == " << (result ? "true" : "false") << messaget::eom;

  log.debug() << expr1.type().pretty() << messaget::eom;
  log.debug() << expr2.type().pretty() << messaget::eom;

  symbol_exprt lhs(
    CPROVER_PREFIX "types_equal#return_value", signedbv_typet(32));
  symex_assign(
    state, code_assignt(lhs, from_integer(result ? 1 : 0, signedbv_typet(32))));
}
