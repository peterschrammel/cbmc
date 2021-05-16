#include "symex_shadow_memory_util.h"

#include <langapi/language_util.h>

#include <util/arith_tools.h>
#include <util/c_types.h>
#include <util/pointer_expr.h>

void log_exact_match(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &resolved_expr)
{
  log.conditional_output(
    log.debug(),
    [ns, shadowed_address, resolved_expr](messaget::mstreamt &mstream) {
      mstream << "exact match: "
              << from_expr(ns, "", shadowed_address.address)
              << " == " << from_expr(ns, "", resolved_expr) << messaget::eom;
    });
}

void log_value_set_match(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &expr)
{
  log.conditional_output(
    log.debug(),
    [ns, shadowed_address, expr](messaget::mstreamt &mstream) {
      mstream << "value set match: "
              << from_expr(ns, "", shadowed_address.address)
              << " == " << from_expr(ns, "", expr) << messaget::eom;
    });
}

void log_value_set_match(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const exprt &matched_base,
  const value_set_dereferencet::valuet &dereference,
  const exprt &expr,
  const value_set_dereferencet::valuet &shadow_dereference)
{
  log.conditional_output(
    log.debug(),
    [ns, shadowed_address, expr, dereference, matched_base, shadow_dereference](
      messaget::mstreamt &mstream) {
      mstream << "value set match: " << messaget::eom;
      mstream << "  base: " << from_expr(ns, "", shadowed_address.address)
              << " <-- " << from_expr(ns, "", matched_base) << messaget::eom;
      mstream << "  cell: " << from_expr(ns, "", dereference.pointer) << " <-- "
              << from_expr(ns, "", expr) << messaget::eom;
      mstream << "  shadow ptr: "
              << from_expr(ns, "", shadow_dereference.pointer) << messaget::eom;
      mstream << "  shadow val: "
              << from_expr(ns, "", shadow_dereference.value) << messaget::eom;
    });
}

void log_value_set_match(
  const namespacet &ns,
  const messaget &log,
  const exprt &address,
  const exprt &expr)
{
  log.conditional_output(
    log.debug(), [ns, address, expr](messaget::mstreamt &mstream) {
      mstream << "value set match: " << from_expr(ns, "", address)
              << " <-- " << from_expr(ns, "", expr) << messaget::eom;
    });
}

void log_cond(
  const namespacet &ns,
  const messaget &log,
  const exprt &cond)
{
  log.conditional_output(
    log.debug(), [ns, cond](messaget::mstreamt &mstream) {
      mstream << "cond: " << from_expr(ns, "", cond)
              << messaget::eom;
    });
}

void log_value_set(
  const namespacet &ns,
  const messaget &log,
  const std::vector<exprt> &value_set)
{
  log.conditional_output(
  log.debug(), [ns, value_set](messaget::mstreamt &mstream) {
    for(const auto &e : value_set)
    {
      mstream << "value set: " << from_expr(ns, "", e) << messaget::eom;
    }
  });
}

void log_try_shadow_address(
  const namespacet &ns,
  const messaget &log,
  const goto_symex_statet::shadowed_addresst &shadowed_address)
{
  log.conditional_output(
    log.debug(), [ns, shadowed_address](messaget::mstreamt &mstream) {
      mstream << "trying shadowed address: "
              << from_expr(ns, "", shadowed_address.address)
              << messaget::eom;
    });
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
      mstream << "set_field: " << id2string(field_name) << " for "
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
      mstream << "get_field: " << id2string(field_name) << " for "
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
    return field_type_it->second;
  }
  field_type_it = state.global_fields.find(field_name);
  CHECK_RETURN(field_type_it != state.global_fields.end());
  return field_type_it->second;
}

exprt compute_max_over_cells(
  const exprt &expr,
  const typet &lhs_type,
  const namespacet &ns,
  const messaget &log)
{
  const typet type = ns.follow(expr.type());

  if(type.id() == ID_struct)
  {
    exprt max = nil_exprt();
    for(const auto &component : to_struct_type(type).components())
    {
      exprt value;
      if(component.type().id() == ID_unsignedbv || component.type().id() == ID_signedbv)
      {
        value = typecast_exprt::conditional_cast(member_exprt(expr, component), lhs_type);
      }
      else
      {
        value = compute_max_over_cells(member_exprt(expr, component),
          lhs_type,
          ns,
          log);
      }

      if (max.is_nil())
      {
        max = value;
      }
      else
      {
        max = if_exprt(binary_predicate_exprt(value, ID_gt, max), value, max);
      }
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
        exprt value;
        if(array_type.subtype().id() == ID_unsignedbv || array_type.subtype().id() == ID_signedbv)
        {
          value = typecast_exprt::conditional_cast(index_exprt(expr, from_integer(index, index_type())), lhs_type);
        }
        else
        {
          value = compute_max_over_cells(
              index_exprt(expr, from_integer(index, index_type())),
              lhs_type,
              ns,
              log);
        }

        if (max.is_nil())
        {
          max = value;
        }
        else
        {
          max = if_exprt(binary_predicate_exprt(value, ID_gt, max), value, max);
        }
      }
      return max;
    }
    else
    {
      log.warning() << "CANNOT COMPUTE MAX OVER SHADOW MEMORY FOR VARIABLE SIZE ARRAY" << messaget::eom;
    }
  }
  return typecast_exprt::conditional_cast(expr, lhs_type);
}

exprt compute_or_over_cells(
    const exprt &expr,
    const typet &lhs_type,
    const namespacet &ns,
    const messaget &log)
{
  const typet type = ns.follow(expr.type());

  if(type.id() == ID_struct)
  {
    exprt::operandst values;
    for(const auto &component : to_struct_type(type).components())
    {
      exprt value;
      if(component.type().id() == ID_unsignedbv || component.type().id() == ID_signedbv)
      {
        value = member_exprt(expr, component);
      }
      else
      {
        value = compute_or_over_cells(member_exprt(expr, component),
                                       lhs_type,
                                       ns,
                                       log);
      }

      values.push_back(typecast_exprt::conditional_cast(value, lhs_type));
    }
    return multi_ary_exprt(ID_bitor, values, lhs_type);
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
        exprt value;
        if(array_type.subtype().id() == ID_unsignedbv || array_type.subtype().id() == ID_signedbv)
        {
          value = index_exprt(expr, from_integer(index, index_type()));
        }
        else
        {
          value = compute_or_over_cells(
              index_exprt(expr, from_integer(index, index_type())),
              lhs_type,
              ns,
              log);
        }

        values.push_back(typecast_exprt::conditional_cast(value, lhs_type));
      }
      return multi_ary_exprt(ID_bitor, values, lhs_type);
    }
    else
    {
      log.warning() << "CANNOT COMPUTE OR OVER SHADOW MEMORY FOR VARIABLE SIZE ARRAY" << messaget::eom;
    }
  }
  return typecast_exprt::conditional_cast(expr, lhs_type);
}
