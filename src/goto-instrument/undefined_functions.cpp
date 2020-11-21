/*******************************************************************\

Module: Handling of functions without body

Author: Michael Tautschnig

Date: July 2016

\*******************************************************************/

/// \file
/// Handling of functions without body

#include "undefined_functions.h"

#include <algorithm>
#include <iostream>

#include <util/cprover_prefix.h>
#include <util/invariant.h>
#include <util/prefix.h>

#include <goto-programs/goto_model.h>

void list_undefined_functions(
  const goto_modelt &goto_model,
  std::ostream &os)
{
  const namespacet ns(goto_model.symbol_table);

  forall_goto_functions(it, goto_model.goto_functions)
    if(
      !has_prefix(id2string(it->first), id2string(CPROVER_PREFIX)) &&
      !ns.lookup(it->first).is_macro &&
      !it->second.body_available())
    {
      os << it->first << '\n';
    }
}

void undefined_function_abort_path(
    goto_modelt &goto_model, const std::list<std::string> &ignore_functions,
    std::ostream &out) {
  std::set<std::string> replaced;
  std::set<std::string> ignored;

  Forall_goto_functions(it, goto_model.goto_functions) {
    Forall_goto_program_instructions(iit, it->second.body)
    {
      goto_programt::instructiont &ins=*iit;

      if(!ins.is_function_call())
        continue;

      const code_function_callt &call=to_code_function_call(ins.code);

      if(call.function().id()!=ID_symbol)
        continue;

      const irep_idt function=
        to_symbol_expr(call.function()).get_identifier();

      goto_functionst::function_mapt::const_iterator entry=
        goto_model.goto_functions.function_map.find(function);
      DATA_INVARIANT(
        entry!=goto_model.goto_functions.function_map.end(),
        "called function must be in function_map");

      if(
        has_prefix(id2string(function), id2string(CPROVER_PREFIX)) ||
        entry->second.body_available())
      {
        continue;
      }

      if (std::find(ignore_functions.begin(), ignore_functions.end(),
                    id2string(function)) != ignore_functions.end()) {
        ignored.insert(id2string(function));
        continue;
      }

      replaced.insert(id2string(function));

      ins = goto_programt::make_assumption(false_exprt(), ins.source_location);
      ins.source_location.set_comment(
        "'" + id2string(function) + "' is undefined");
    }
  }

  out << "Replaced by assume(false):\n";
  for (const auto &function : replaced) {
    out << "  " << function << '\n';
  }
  out << "Not replaced:\n";
  for (const auto &function : ignored) {
    out << "  " << function << '\n';
  }
}

void undefined_function_abort_path(goto_modelt &goto_model) {
  undefined_function_abort_path(goto_model, {}, std::cout);
}
