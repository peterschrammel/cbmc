/// \file
/// Symbolic Execution

#ifndef CPROVER_GOTO_SYMEX_SYMEX_SHADOW_MEMORY_UTIL_H
#define CPROVER_GOTO_SYMEX_SYMEX_SHADOW_MEMORY_UTIL_H

#include "goto_symex_state.h"

#include <pointer-analysis/value_set_dereference.h>

void log_exact_match(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &resolved_expr);

void log_value_set_match(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &expr);

void log_value_set_match(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &matched_base,
  const value_set_dereferencet::valuet &dereference,
  const exprt &expr);

void log_value_set_match(
  const namespacet &ns,
  const messaget &log,
  const exprt &address,
  const exprt &expr);

void log_cond(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &expr);

void log_value_set(
  const namespacet &ns,
  const messaget &log,
  const value_setst::valuest &value_set);

void log_try_shadow_address(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address);

void log_set_field(
  const namespacet &ns,
  const messaget &log,
  const irep_idt &field_name,
  const exprt &expr,
  const exprt &value);

void log_get_field(
  const namespacet &ns,
  const messaget &log,
  const irep_idt &field_name,
  const exprt &expr);

exprt deref_expr(const exprt &expr);

irep_idt get_field_name(const exprt &string_expr);

typet remove_array_type_l2(const typet &type);

void remove_pointer_object(exprt &expr);

bool filter_by_value_set(
  const value_setst::valuest &value_set,
  const exprt &address);

#endif // CPROVER_GOTO_SYMEX_SYMEX_SHADOW_MEMORY_UTIL_H
