#include "symex_shadow_memory_util.h"

#include <langapi/language_util.h>

#include <util/arith_tools.h>
#include <util/bitvector_expr.h>
#include <util/c_types.h>
#include <util/pointer_expr.h>
#include <util/expr_initializer.h>

void log_exact_match(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &resolved_expr)
{
#ifdef DEBUG_SM
  log.conditional_output(
    log.debug(),
    [ns, shadowed_address, resolved_expr](messaget::mstreamt &mstream) {
      mstream << "Shadow memory: exact match: "
              << from_expr(ns, "", shadowed_address.address)
              << " == " << from_expr(ns, "", resolved_expr) << messaget::eom;
    });
#endif
}

void log_value_set_match(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &expr)
{
#ifdef DEBUG_SM
  log.conditional_output(
    log.debug(),
    [ns, shadowed_address, expr](messaget::mstreamt &mstream) {
      mstream << "Shadow memory: value set match: "
              << from_expr(ns, "", shadowed_address.address)
              << " == " << from_expr(ns, "", expr) << messaget::eom;
    });
#endif
}

void log_value_set_match(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &matched_base_address,
  const value_set_dereferencet::valuet &dereference,
  const exprt &expr,
  const value_set_dereferencet::valuet &shadow_dereference)
{
#ifdef DEBUG_SM
  log.conditional_output(
    log.debug(),
    [ns, shadowed_address, expr, dereference, matched_base_address, shadow_dereference](
      messaget::mstreamt &mstream) {
      mstream << "Shadow memory: value set match: " << messaget::eom;
      mstream << "Shadow memory:   base: " << from_expr(ns, "", shadowed_address.address)
              << " <-- " << from_expr(ns, "", matched_base_address) << messaget::eom;
      mstream << "Shadow memory:   cell: " << from_expr(ns, "", dereference.pointer) << " <-- "
              << from_expr(ns, "", expr) << messaget::eom;
      mstream << "Shadow memory:   shadow ptr: "
              << from_expr(ns, "", shadow_dereference.pointer) << messaget::eom;
      mstream << "Shadow memory:   shadow val: "
              << from_expr(ns, "", shadow_dereference.value) << messaget::eom;
    });
#endif
}

void log_value_set_match(
  const namespacet &ns,
  const messaget &log,
  const exprt &address,
  const exprt &expr)
{
#ifdef DEBUG_SM
  log.conditional_output(
    log.debug(), [ns, address, expr](messaget::mstreamt &mstream) {
      mstream << "Shadow memory: value set match: " << from_expr(ns, "", address)
              << " <-- " << from_expr(ns, "", expr) << messaget::eom;
    });
#endif
}

void log_cond(
  const namespacet &ns,
  const messaget &log,
  const char *cond_text,
  const exprt &cond)
{
#ifdef DEBUG_SM
  log.conditional_output(
    log.debug(), [ns, cond_text, cond](messaget::mstreamt &mstream) {
      mstream << "Shadow memory: " << cond_text << ": " << from_expr(ns, "", cond)
              << messaget::eom;
    });
#endif
}

void log_value_set(
  const namespacet &ns,
  const messaget &log,
  const std::vector<exprt> &value_set)
{
#ifdef DEBUG_SM
  log.conditional_output(
  log.debug(), [ns, value_set](messaget::mstreamt &mstream) {
    for(const auto &e : value_set)
    {
      mstream << "Shadow memory: value set: " << from_expr(ns, "", e) << messaget::eom;
    }
  });
#endif
}

void log_try_shadow_address(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address)
{
#ifdef DEBUG_SM
  log.conditional_output(
    log.debug(), [ns, shadowed_address](messaget::mstreamt &mstream) {
      mstream << "Shadow memory: trying shadowed address: "
              << from_expr(ns, "", shadowed_address.address)
              << messaget::eom;
    });
#endif
}

void log_set_field(
  const namespacet &ns,
  const messaget &log,
  const irep_idt &field_name,
  const exprt &expr,
  const exprt &value)
{
  log.conditional_output(
    log.debug(), [field_name, ns, expr, value](messaget::mstreamt &mstream) {
      mstream << "Shadow memory: set_field: " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << " to " << from_expr(ns, "", value)
              << messaget::eom;
    });
}

void log_get_field(
  const namespacet &ns,
  const messaget &log,
  const irep_idt &field_name,
  const exprt &expr)
{
  log.conditional_output(
    log.debug(), [ns, field_name, expr](messaget::mstreamt &mstream) {
      mstream << "Shadow memory: get_field: " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << messaget::eom;
    });
}

exprt deref_expr(const exprt &expr)
{
  if(expr.id() == ID_address_of)
  {
    return to_address_of_expr(expr).object();
  }

  return dereference_exprt(expr);
}

irep_idt get_field_name(const exprt &string_expr)
{
  if(string_expr.id() == ID_typecast)
    return get_field_name(to_typecast_expr(string_expr).op());
  else if(string_expr.id() == ID_address_of)
    return get_field_name(to_address_of_expr(string_expr).object());
  else if(string_expr.id() == ID_index)
    return get_field_name(to_index_expr(string_expr).array());
  else if(string_expr.id() == ID_string_constant)
  {
    return string_expr.get(ID_value);
  }
  else
    UNREACHABLE;
}

void fix_array_with_expr_size_in_type(exprt &expr)
{
  if(expr.id() == ID_with)
  {
    with_exprt &with_expr = to_with_expr(expr);
    if(with_expr.type() != with_expr.old().type())
    {
      with_expr.type() = with_expr.old().type();
    }
    return;
  }
  Forall_operands(it, expr)
      fix_array_with_expr_size_in_type(*it);
}

typet remove_array_type_l2(const typet &type)
{
  if(to_array_type(type).size().id() != ID_symbol)
    return type;

  array_typet array_type = to_array_type(type); // copy

  ssa_exprt &size = to_ssa_expr(array_type.size());
  size.remove_level_2();

  return std::move(array_type);
}

void remove_pointer_object(exprt &expr)
{
  if(expr.id() == ID_pointer_object)
  {
    expr = to_unary_expr(expr).op();
    return;
  }
  Forall_operands(it, expr)
    remove_pointer_object(*it);
  // pointer_object has size_type(): revert to original type after removal
  if(expr.id() == ID_if)
  {
    const if_exprt &if_expr = to_if_expr(expr);
    expr.type() = if_expr.true_case().type();
  }
  else if(expr.id() == ID_with)
  {
    const with_exprt &with_expr = to_with_expr(expr);
    expr.type() = with_expr.old().type();
  }
}

bool filter_by_value_set(
  const std::vector<exprt>  &value_set,
  const exprt &address)
{
  //log.debug() << "address: " << address.pretty() << messaget::eom;

  if(
    address.id() == ID_constant && address.type().id() == ID_pointer &&
    to_constant_expr(address).is_zero())
  {
    for(const auto &e : value_set)
    {
      if(e.id() != ID_object_descriptor)
        continue;

      const exprt &expr = to_object_descriptor_expr(e).object();
      if(expr.id() == ID_null_object)
        return true;
    }
    return false;
  }

// too restrictive
#if 0
  INVARIANT(
    address.id() == ID_address_of, "address of shadowed object expected");

  exprt expr2 = to_address_of_expr(address).object();
  if (expr2.id() == ID_index) {
    expr2 = to_index_expr(expr2).array();
  }
  INVARIANT(expr2.id() == ID_symbol, "symbol of shadowed object expected");
#endif

  for(const auto &e : value_set)
  {
    //log.debug() << "object: " << e.pretty() << messaget::eom;
    if(e.id() == ID_unknown)
      return true;

    if(e.id() != ID_object_descriptor)
      continue;

      // too restrictive
#if 0
    exprt expr1 = to_object_descriptor_expr(e).object();
    if (expr1.id() == ID_index) {
      expr1 = to_index_expr(expr1).array();
    }
    if(expr1.id() != ID_symbol)
      continue;

    if(to_symbol_expr(expr1).get_identifier() ==
       to_symbol_expr(expr2).get_identifier())
    {
      return true;
    }
    if(expr1.type() == expr2.type())
    {
      return true;
    }
#endif
    return true;
  }
  return false;
}

const typet &get_field_type(
  const irep_idt& field_name,
  const goto_symex_statet &state)
{
  auto field_type_it = state.local_fields.find(field_name);
  if (field_type_it != state.local_fields.end()) {
    return field_type_it->second.type();
  }
  field_type_it = state.global_fields.find(field_name);
  CHECK_RETURN(field_type_it != state.global_fields.end());
  return field_type_it->second.type();
}

static void max_element(
    const exprt &element,
    const typet &field_type,
    exprt &max)
{
  exprt value = typecast_exprt::conditional_cast(element, field_type);
  if (max.is_nil())
  {
    max = value;
  }
  else {
    max = if_exprt(binary_predicate_exprt(value, ID_gt, max), value, max);
  }
}

static void max_over_bytes(
    const exprt &value,
    const typet &type,
    const typet &field_type,
    exprt &max)
{
  const size_t size = to_bitvector_type(type).get_width() / 8;
  max_element(value, field_type, max);
  for(size_t i = 1; i < size; ++i)
  {
    max_element(
        lshr_exprt(value, from_integer(8 * i, size_type())),
        field_type,
        max);
  }
}

static void max_elements(
    const exprt &element,
    const typet &field_type,
    const namespacet &ns,
    const messaget &log,
    const bool is_union,
    exprt &max)
{
  if(element.type().id() == ID_unsignedbv || element.type().id() == ID_signedbv)
  {
    if(is_union)
    {
      max_over_bytes(element, element.type(), field_type, max);
    }
    else
    {
      max_element(element, field_type, max);
    }
  }
  else
  {
    exprt value = compute_max_over_cells(element,
                                        field_type,
                                        ns,
                                        log,
                                        is_union);
    max_element(value, field_type, max);
  }
}

exprt compute_max_over_cells(
  const exprt &expr,
  const typet &field_type,
  const namespacet &ns,
  const messaget &log,
  const bool is_union)
{
  const typet type = ns.follow(expr.type());

  if(type.id() == ID_struct || type.id() == ID_union)
  {
    exprt max = nil_exprt();
    for(const auto &component : to_struct_union_type(type).components())
    {
      if(component.get_is_padding())
      {
        continue;
      }
      max_elements(
          member_exprt(expr, component),
          field_type,
          ns,
          log,
          is_union,
          max);
    }
    return max;
  }
  else if(type.id() == ID_array)
  {
    const array_typet &array_type = to_array_type(type);
    if(array_type.size().is_constant())
    {
      exprt max = nil_exprt();
      const mp_integer size = numeric_cast_v<mp_integer>(to_constant_expr(array_type.size()));
      for(mp_integer index = 0; index < size; ++index)
      {
        max_elements(
            index_exprt(expr, from_integer(index, index_type())),
            field_type,
            ns,
            log,
            is_union,
            max);
      }
      return max;
    }
    else
    {
      log.warning()
        << "Shadow memory: cannot compute max over variable-size array "
        << from_expr(ns, "", expr)
        << messaget::eom;
    }
  }
  return typecast_exprt::conditional_cast(expr, field_type);
}

static void or_over_bytes(
  const exprt &value,
  const typet &type,
  const typet &field_type,
  exprt::operandst &values)
{
  const size_t size = to_bitvector_type(type).get_width() / 8;
  values.push_back(typecast_exprt::conditional_cast(value, field_type));
  for(size_t i = 1; i < size; ++i)
  {
    values.push_back(
        typecast_exprt::conditional_cast(
            lshr_exprt(value, from_integer(8 * i, size_type())),
            field_type));
  }
}

static exprt or_values(const exprt::operandst &values, const typet &field_type)
{
  if(values.size() == 1)
  {
    return values[0];
  }
  return multi_ary_exprt(ID_bitor, values, field_type);
}

static void or_elements(
    const exprt &element,
    const typet &field_type,
    const namespacet &ns,
    const messaget &log,
    const bool is_union,
    exprt::operandst &values)
{
  if(element.type().id() == ID_unsignedbv || element.type().id() == ID_signedbv)
  {
    exprt value = element;
    if(is_union)
    {
      or_over_bytes(value, element.type(), field_type, values);
    }
    else
    {
      values.push_back(typecast_exprt::conditional_cast(value, field_type));
    }
  }
  else
  {
    exprt value = compute_or_over_cells(element,
                                        field_type,
                                        ns,
                                        log,
                                        is_union);
    values.push_back(typecast_exprt::conditional_cast(value, field_type));
  }
}

exprt compute_or_over_cells(
    const exprt &expr,
    const typet &field_type,
    const namespacet &ns,
    const messaget &log,
    const bool is_union)
{
  const typet type = ns.follow(expr.type());

  if(type.id() == ID_struct || type.id() == ID_union)
  {
    exprt::operandst values;
    for(const auto &component : to_struct_union_type(type).components())
    {
      if(component.get_is_padding())
      {
        continue;
      }
      or_elements(
          member_exprt(expr, component),
          field_type,
          ns,
          log,
          is_union,
          values);
    }
    return or_values(values, field_type);
  }
  else if(type.id() == ID_array)
  {
    const array_typet &array_type = to_array_type(type);
    if(array_type.size().is_constant())
    {
      exprt::operandst values;
      const mp_integer size = numeric_cast_v<mp_integer>(to_constant_expr(array_type.size()));
      for(mp_integer index = 0; index < size; ++index)
      {
        or_elements(
            index_exprt(expr, from_integer(index, index_type())),
            field_type,
            ns,
            log,
            is_union,
            values);
      }
      return or_values(values, field_type);
    }
    else
    {
      log.warning()
          << "Shadow memory: cannot compute or over variable-size array "
          << from_expr(ns, "", expr)
          << messaget::eom;
    }
  }
  exprt::operandst values;
  if(is_union)
  {
    or_over_bytes(expr, type, field_type, values);
  }
  else
  {
    values.push_back(typecast_exprt::conditional_cast(expr, field_type));
  }
  return or_values(values, field_type);
}

exprt duplicate_per_byte(
    const exprt &expr,
    const typet &lhs_type,
    const namespacet &ns,
    const messaget &log)
{
  if(lhs_type.id() == ID_unsignedbv || lhs_type.id() == ID_signedbv)
  {
    return duplicate_per_byte(expr, lhs_type, ns);
  }
  else if(lhs_type.id() == ID_pointer)
  {
    // We don't support union semantics for pointers.
  }
  else if(lhs_type.id() == ID_c_bool)
  {
    // nothing to do
  }
  else
  {
    log.warning()
        << "Shadow memory: cannot handle primitive union updates correctly for "
        << from_type(ns, "", lhs_type)
        << messaget::eom;
  }
  return typecast_exprt::conditional_cast(expr, lhs_type);
}

exprt remove_casts(exprt expr)
{
  while(expr.id() == ID_typecast)
  {
    expr = to_typecast_expr(expr).op();
  }
  return expr;
}

void replace_invalid_object_by_null(exprt &expr)
{
  if(expr.id() == ID_symbol && expr.type().id() == ID_pointer &&
      (id2string(to_symbol_expr(expr).get_identifier()).rfind("invalid_object") != std::string::npos ||
      id2string(to_symbol_expr(expr).get_identifier()).rfind("$object") != std::string::npos))
  {
    expr = null_pointer_exprt(to_pointer_type(expr.type()));
    return;
  }
  Forall_operands(it, expr)
  {
    replace_invalid_object_by_null(*it);
  }
}