/*******************************************************************\

Module: Symex Shadow Memory Instrumentation

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Symex Shadow Memory Instrumentation

#include "goto_symex.h"

#include <util/arith_tools.h>
#include <util/base_type.h>
#include <util/c_types.h>
#include <util/cprover_prefix.h>
#include <util/find_symbols.h>
#include <util/fresh_symbol.h>
#include <util/invariant.h>
#include <util/pointer_predicates.h>
#include <util/prefix.h>
#include <util/invariant.h>
#include <util/message.h>
#include <util/pointer_offset_size.h>
#include <util/replace_expr.h>
#include <util/source_location.h>
#include <util/std_expr.h>
#include <util/string_constant.h>

#include <langapi/language_util.h>

#include <goto-programs/goto_model.h>
#include <pointer-analysis/value_set_dereference.h>

static irep_idt get_field_name(const exprt &string_expr)
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

static typet remove_array_type_l2(const typet &type) {
  if (to_array_type(type).size().id() != ID_symbol)
    return type;

  array_typet array_type = to_array_type(type); // copy

  ssa_exprt &size = to_ssa_expr(array_type.size());
  size.remove_level_2();

  return array_type;
}

static void remove_pointer_object(exprt &expr) {
  if(expr.id() == ID_pointer_object)
  {
    expr = to_unary_expr(expr).op();
    return;
  }
  Forall_operands(it, expr)
    remove_pointer_object(*it);
  // pointer_object has size_type(): revert to original type after removal
  if (expr.id() == ID_if) {
    const if_exprt &if_expr = to_if_expr(expr);
    expr.type() = if_expr.true_case().type();
  } else if (expr.id() == ID_with) {
    const with_exprt &with_expr = to_with_expr(expr);
    expr.type() = with_expr.old().type();
  }
}

bool goto_symext::can_be_initialized_rec(
  const namespacet &ns,
  const exprt &expr)
{
  typet type = ns.follow(expr.type());
  if(type.id() == ID_array)
  {
    exprt size_expr = to_array_type(type).size();
    if(!size_expr.is_constant())
    {
      log.warning() << "constant array size expected:\n"
                    << from_expr(ns, "", size_expr) << messaget::eom;
      log.warning() << "falling back to shadow-per-object for "
                    << from_expr(ns, "", expr) << messaget::eom;
      return false;
    }
    mp_integer array_size;
    to_integer(to_constant_expr(size_expr), array_size);
    for(mp_integer index = 0; index < array_size; ++index)
    {
      if(
        !can_be_initialized_rec(
          ns,
          index_exprt(expr, from_integer(index, signed_long_int_type()))))
      {
        return false;
      }
    }
  }
  else if(type.id() == ID_struct)
  {
    for(const auto &component : to_struct_type(type).components())
    {
      if(!can_be_initialized_rec(ns, member_exprt(expr, component)))
      {
        return false;
      }
    }
  }
  return true;
}
  
void goto_symext::initialize_rec(
  const namespacet &ns,
  goto_symex_statet &state,
  const exprt &expr,
  bool per_object,
  std::map<irep_idt, typet> &fields)
{
  typet type = ns.follow(expr.type());
  if(type.id() == ID_array && !per_object)
  {
    exprt size_expr = to_array_type(type).size();
    CHECK_RETURN(size_expr.is_constant());
    mp_integer array_size;
    to_integer(to_constant_expr(size_expr), array_size);
    for(mp_integer index = 0; index < array_size; ++index)
    {
      initialize_rec(
        ns,
        state,
        index_exprt(expr, from_integer(index, signed_long_int_type())),
        per_object,
        fields);
    }
  }
  else if(type.id() == ID_struct && !per_object)
  {
    for(const auto &component : to_struct_type(type).components())
    {
      initialize_rec(ns, state, member_exprt(expr, component), per_object, fields);
    }
    return;
  }
  
  if(per_object || (type.id() != ID_array && type.id() != ID_struct))
  {
    for(const auto &field_pair : fields)
    {
      exprt address_expr = expr;
      if(type.id() == ID_array && expr.id() == ID_symbol)
      {
        address_expr.type() = remove_array_type_l2(address_expr.type());
        exprt original_expr = to_ssa_expr(expr).get_original_expr();
        original_expr.type() = remove_array_type_l2(original_expr.type());
        to_ssa_expr(address_expr).set_expression(original_expr);
      }
      if(address_expr.id() == ID_dereference)
      {
        address_expr = to_dereference_expr(address_expr).pointer();
      }
      else
      {
        address_expr = address_of_exprt(address_expr);
      }

      symbol_exprt field = add_field(
        ns, state, address_expr, field_pair.first, per_object, fields);
      code_assignt code_assign(
        field, from_integer(mp_integer(0), field.type()));
      symex_assign(state, code_assign);

      log.conditional_output(
        log.debug(),
        [field, ns, address_expr, per_object](messaget::mstreamt &mstream) {
          mstream << "initialize field " << id2string(field.get_identifier())
                  << " for " << from_expr(ns, "", address_expr)
                  << (per_object ? " (per object)" : "")
                  << messaget::eom;
        });
    }
  }
}

symbol_exprt goto_symext::add_field(
  const namespacet &ns,
  goto_symex_statet &state,
  const exprt &expr,
  const irep_idt &field_name,
  bool per_object,
  std::map<irep_idt, typet> &fields)
{
  auto &addresses = state.address_fields[field_name];
  symbolt &new_symbol = get_fresh_aux_symbol(
    fields.at(field_name),
    id2string(state.source.function_id),
    from_expr(ns, "", expr) + "." + id2string(field_name),
    state.source.pc->source_location,
    ID_C,
    state.symbol_table);

  addresses.push_back(
    goto_symex_statet::shadowed_addresst{expr, new_symbol.symbol_expr(), per_object});
  return new_symbol.symbol_expr();
}

static std::vector<exprt> get_filtered_value_set(
  const value_setst::valuest &value_set,
  const exprt &address)
{
  std::vector<exprt> result;
  INVARIANT(
    address.id() == ID_address_of, "address of shadowed object expected");

  exprt expr2 = to_address_of_expr(address).object();
  if (expr2.id() == ID_index) {
    expr2 = to_index_expr(expr2).array();
  }

  for(const auto &e : value_set)
  {
    if(e.id() != ID_object_descriptor)
      continue;

    exprt expr1 = to_object_descriptor_expr(e).object();
    if (expr1.id() == ID_index) {
      expr1 = to_index_expr(expr1).array();
    }
    if(expr1.id() != ID_symbol)
      continue;

    if(expr2.id() != ID_symbol ||
       to_symbol_expr(expr1).get_identifier() ==
       to_symbol_expr(expr2).get_identifier())
    {
      result.push_back(e);
    }
  }
  return result;
}

bool goto_symext::filter_by_value_set(
  const value_setst::valuest &value_set,
  const exprt &address)
{
  //log.debug() << "address: " << address.pretty() << messaget::eom;

  if(address.id() == ID_constant &&
     address.type().id() == ID_pointer &&
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

void goto_symext::symex_set_field(
  const namespacet &ns,
  goto_symex_statet &state,
  const code_function_callt &code_function_call)
{
  INVARIANT(
    code_function_call.arguments().size() == 3,
    CPROVER_PREFIX "set_field requires 3 arguments");
  irep_idt field_name = get_field_name(code_function_call.arguments()[1]);

  exprt expr = code_function_call.arguments()[0];
  typet expr_type = expr.type();
  DATA_INVARIANT(
    expr.type().id() == ID_pointer,
    "shadow memory requires a pointer expression");

  exprt value = code_function_call.arguments()[2];

  log.conditional_output(
    log.debug(),
    [field_name, ns, expr, value](messaget::mstreamt &mstream) {
      mstream << "set_field: " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << " to " << from_expr(ns, "", value)
              << messaget::eom;
    });
  INVARIANT(
    state.address_fields.count(field_name) == 1,
    id2string(field_name) + " should exist");
  const auto &addresses = state.address_fields.at(field_name);
  const exprt &rhs = value;
  exprt lhs = nil_exprt();
  size_t mux_size = 0;
  value_setst::valuest value_set;
  state.value_set.get_value_set(expr, value_set, ns);
  log.conditional_output(
    log.debug(),
    [ns, value_set](messaget::mstreamt &mstream) {
      for (const auto &e : value_set)
      {
        mstream << "value set: " << from_expr(ns, "", e) << messaget::eom;
      }
    });

  const null_pointer_exprt null_pointer(to_pointer_type(expr.type()));
  if(value_set.size() == 1 && filter_by_value_set(value_set, null_pointer))
  {
    log.conditional_output(
      log.debug(),
      [ns, null_pointer, expr](messaget::mstreamt &mstream) {
        mstream << "value set match: " << from_expr(ns, "", null_pointer)
                << " <-- " << from_expr(ns, "", expr) << messaget::eom;
        mstream << "ignoring set field on NULL object" << messaget::eom;
      });
    return;
  }

  for(const auto &shadowed_address : addresses)
  {
    log.conditional_output(
      log.debug(),
      [ns, shadowed_address](messaget::mstreamt &mstream) {
        mstream << "trying shadowed address: "
                << from_expr(ns, "", shadowed_address.address)
                << (shadowed_address.per_object ? " (per object)" : "")
                << messaget::eom;
      });
    if(shadowed_address.per_object)
    {
      // exact match
      if(shadowed_address.address == expr)
      {
        log.conditional_output(
          log.debug(),
          [ns, shadowed_address, expr](messaget::mstreamt &mstream) {
            mstream
              << "exact match: " << from_expr(ns, "", shadowed_address.address)
              << " == " << from_expr(ns, "", expr) << messaget::eom;
          });
        lhs = address_of_exprt(shadowed_address.shadow);
        mux_size = 1;
        break;
      }

      std::vector<exprt> filtered_value_set =
        get_filtered_value_set(value_set,shadowed_address.address);
      if(filtered_value_set.empty())
        continue;

      for(const auto &matched_object : filtered_value_set)
      {
        value_set_dereferencet::valuet dereference =
          value_set_dereferencet::build_reference_to(matched_object, expr, ns);

        const exprt &matched_base =
          address_of_exprt(to_object_descriptor_expr(matched_object).object());

        log.conditional_output(
          log.debug(),
          [ns, shadowed_address, expr, dereference, matched_base](messaget::mstreamt &mstream) {
            mstream << "value set match: " << messaget::eom;
            mstream  << "  " << from_expr(ns, "", shadowed_address.address)
                     << " <-- " << from_expr(ns, "", matched_base)
                     << messaget::eom;
            mstream  << "  " << from_expr(ns, "", dereference.pointer)
                     << " <-- " << from_expr(ns, "", expr) << messaget::eom;
          });
        exprt cond = and_exprt(
          equal_exprt(
            expr,
            typecast_exprt::conditional_cast(dereference.pointer, expr.type())),
          equal_exprt(
            shadowed_address.address,
            typecast_exprt::conditional_cast(
              matched_base, shadowed_address.address.type())));
        log.conditional_output(
          log.debug(),
          [ns, shadowed_address, cond](messaget::mstreamt &mstream) {
            mstream << "cond: " << from_expr(ns, "", cond)
                    << (shadowed_address.per_object ? " (per object)" : "")
                    << messaget::eom;
          });
        if(!cond.is_false())
        {
          mux_size++;
          if(lhs.is_nil())
          {
            lhs = address_of_exprt(shadowed_address.shadow);
          }
          else
          {
            lhs = if_exprt(cond, address_of_exprt(shadowed_address.shadow), lhs);
          }
        }
      }
    }
    else // !shadowed_address.per_object
    {
      // exact match
      if(shadowed_address.address == expr)
      {
        log.conditional_output(
          log.debug(),
          [ns, shadowed_address, expr](messaget::mstreamt &mstream) {
            mstream << "exact match: " << from_expr(ns, "", shadowed_address.address)
                    << " == " << from_expr(ns, "", expr) << messaget::eom;
          });
        lhs = address_of_exprt(shadowed_address.shadow);
        mux_size = 1;
        break;
      }

      if(!filter_by_value_set(value_set, shadowed_address.address))
        continue;

      log.conditional_output(
        log.debug(),
        [ns, shadowed_address, expr](messaget::mstreamt &mstream) {
          mstream << "value set match: " << from_expr(ns, "", shadowed_address.address)
                  << " <-- " << from_expr(ns, "", expr) << messaget::eom;
        });

      exprt cond = equal_exprt(
        shadowed_address.address,
        typecast_exprt::conditional_cast(expr, shadowed_address.address.type()));
      log.conditional_output(
        log.debug(),
        [ns, shadowed_address, cond](messaget::mstreamt &mstream) {
          mstream << "cond: " << from_expr(ns, "", cond)
                  << (shadowed_address.per_object ? " (per object)" : "")
                  << messaget::eom;
        });
      if(!cond.is_false())
      {
        mux_size++;
        if(lhs.is_nil())
        {
          lhs = address_of_exprt(shadowed_address.shadow);
        }
        else
        {
          lhs = if_exprt(cond, address_of_exprt(shadowed_address.shadow), lhs);
        }
      }
    }
  }

  if (lhs.is_not_nil()) {
    log.debug() << "mux size set_field: " << mux_size << messaget::eom;
    lhs = dereference_exprt(lhs);
    symex_assign(state, code_assignt(lhs, typecast_exprt::conditional_cast(
                                              rhs, lhs.type())));
  } else {
    log.warning() << "cannot set_field for " << from_expr(ns, "", expr)
                  << messaget::eom;
  }
}

void goto_symext::symex_get_field(
  const namespacet &ns,
  goto_symex_statet &state,
  const code_function_callt &code_function_call)
{
  INVARIANT(
    code_function_call.arguments().size() == 2,
    CPROVER_PREFIX "get_field requires 2 arguments");
  irep_idt field_name = get_field_name(code_function_call.arguments()[1]);

  exprt expr = code_function_call.arguments()[0];
  typet expr_type = expr.type();
  DATA_INVARIANT(
    expr_type.id() == ID_pointer,
    "shadow memory requires a pointer expression");

  log.conditional_output(
    log.debug(),
    [ns, field_name, expr](messaget::mstreamt &mstream) {
      mstream << "get_field: " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << messaget::eom;
    });

  INVARIANT(
    state.address_fields.count(field_name) == 1,
    id2string(field_name) + " should exist");
  const auto &addresses = state.address_fields.at(field_name);
  // Should actually be fields.at(field_name)
  symbol_exprt lhs(CPROVER_PREFIX "get_field#return_value", signedbv_typet(32));
  exprt rhs = nil_exprt();
  size_t mux_size = 0;
  value_setst::valuest value_set;
  state.value_set.get_value_set(expr, value_set, ns);
  log.conditional_output(
    log.debug(),
    [ns, value_set](messaget::mstreamt &mstream) {
      for (const auto &e : value_set)
      {
        mstream << "value set: " << from_expr(ns, "", e) << messaget::eom;
      }
    });

  const null_pointer_exprt null_pointer(to_pointer_type(expr.type()));
  if(filter_by_value_set(value_set, null_pointer))
  {
    log.conditional_output(
      log.debug(),
      [ns, null_pointer, expr](messaget::mstreamt &mstream) {
        mstream << "value set match: " << from_expr(ns, "", null_pointer)
                << " <-- " << from_expr(ns, "", expr) << messaget::eom;
      });
    rhs = if_exprt(equal_exprt(expr, null_pointer),
                   from_integer(0, lhs.type()), from_integer(-1, lhs.type()));
    mux_size = 1;
  }

  for(const auto &shadowed_address : addresses)
  {
    log.conditional_output(
      log.debug(),
      [ns, shadowed_address](messaget::mstreamt &mstream) {
        mstream
          << "trying shadowed address: " << from_expr(ns, "", shadowed_address.address)
          << (shadowed_address.per_object ? " (per object)" : "")
          << messaget::eom;
      });
    if(shadowed_address.per_object)
    {
      // exact match
      if(shadowed_address.address == expr)
      {
        log.conditional_output(
          log.debug(),
          [ns, shadowed_address, expr](messaget::mstreamt &mstream) {
            mstream
              << "exact match: " << from_expr(ns, "", shadowed_address.address)
              << " == " << from_expr(ns, "", expr) << messaget::eom;
          });
        rhs = typecast_exprt::conditional_cast(shadowed_address.shadow, lhs.type());
        mux_size = 1;
        break;
      }

      std::vector<exprt> filtered_value_set =
        get_filtered_value_set(value_set, shadowed_address.address);
      if(filtered_value_set.empty())
        continue;

      for(const auto &matched_object : filtered_value_set)
      {
        value_set_dereferencet::valuet dereference =
          value_set_dereferencet::build_reference_to(matched_object, expr, ns);

        const exprt &matched_base =
          address_of_exprt(to_object_descriptor_expr(matched_object).object());

        log.conditional_output(
          log.debug(),
          [ns, shadowed_address, expr, matched_base, dereference](messaget::mstreamt &mstream) {
            mstream << "value set match: " << messaget::eom;
            mstream << "  " << from_expr(ns, "", shadowed_address.address)
                    << " <-- " << from_expr(ns, "", matched_base)
                    << messaget::eom;
            mstream << "  " << from_expr(ns, "", dereference.pointer)
                    << " <-- " << from_expr(ns, "", expr) << messaget::eom;
          });
        exprt cond = and_exprt(
          equal_exprt(
            expr,
            typecast_exprt::conditional_cast(dereference.pointer, expr.type())),
          equal_exprt(
            shadowed_address.address,
            typecast_exprt::conditional_cast(
              matched_base, shadowed_address.address.type())));
        log.conditional_output(
          log.debug(),
          [ns, shadowed_address, cond](messaget::mstreamt &mstream) {
            mstream << "cond: " << from_expr(ns, "", cond)
                    << (shadowed_address.per_object ? " (per object)" : "")
                    << messaget::eom;
          });
        if(!cond.is_false())
        {
          mux_size++;
          if(rhs.is_nil())
          {
            rhs = if_exprt(
              cond,
              typecast_exprt::conditional_cast(
                shadowed_address.shadow, lhs.type()),
              from_integer(-1, lhs.type()));
          }
          else
          {
            rhs = if_exprt(
              cond,
              typecast_exprt::conditional_cast(
                shadowed_address.shadow, lhs.type()),
              rhs);
          }
        }
      }
    }
    else // !shadowed_address.per_object
    {
      // exact match
      if(shadowed_address.address == expr)
      {
        log.conditional_output(
          log.debug(),
          [ns, shadowed_address, expr](messaget::mstreamt &mstream) {
            mstream << "exact match: " << from_expr(ns, "", shadowed_address.address)
                    << " == " << from_expr(ns, "", expr) << messaget::eom;
          });
        rhs = typecast_exprt::conditional_cast(shadowed_address.shadow, lhs.type());
        mux_size = 1;
        break;
      }

      if(!filter_by_value_set(value_set, shadowed_address.address))
        continue;

      log.conditional_output(
        log.debug(),
        [ns, shadowed_address, expr](messaget::mstreamt &mstream) {
          mstream << "value set match: " << from_expr(ns, "", shadowed_address.address)
                  << " <-- " << from_expr(ns, "", expr) << messaget::eom;
        });
      exprt cond = equal_exprt(
        shadowed_address.address,
        typecast_exprt::conditional_cast(expr, shadowed_address.address.type()));
      log.conditional_output(
        log.debug(),
        [ns, shadowed_address, cond](messaget::mstreamt &mstream) {
          mstream << "cond: " << from_expr(ns, "", cond)
                  << (shadowed_address.per_object ? " (per object)" : "")
                  << messaget::eom;
        });
      if(!cond.is_false())
      {
        mux_size++;
        if(rhs.is_nil())
        {
          rhs = if_exprt(
            cond,
            typecast_exprt::conditional_cast(
              shadowed_address.shadow, lhs.type()),
            from_integer(-1, lhs.type()));
        }
        else
        {
          rhs = if_exprt(
            cond,
            typecast_exprt::conditional_cast(
              shadowed_address.shadow, lhs.type()),
            rhs);
        }
      }
    }
  }

  if (rhs.is_not_nil())
  {
    log.debug() << "mux size get_field: " << mux_size << messaget::eom;
    symex_assign(state, code_assignt(lhs, typecast_exprt::conditional_cast(
                                              rhs, lhs.type())));
  }
  else
  {
    log.warning() << "cannot get_field for " << from_expr(ns, "", expr)
                  << messaget::eom;
    symex_assign(state, code_assignt(lhs, from_integer(0, lhs.type())));
  }
}

void goto_symext::symex_field_static_init(const namespacet &ns,
                                          goto_symex_statet &state,
                                          const ssa_exprt &expr) {
  if(state.source.function_id != CPROVER_PREFIX "initialize")
    return;

  const irep_idt &identifier =
      to_symbol_expr(expr.get_original_expr()).get_identifier();
  if(has_prefix(id2string(identifier), CPROVER_PREFIX))
    return;

  const symbolt &symbol = ns.lookup(identifier);

  if(symbol.is_auxiliary || !symbol.is_static_lifetime)
    return;
  if(id2string(symbol.name).find("__cs_") != std::string::npos)
    return;

  const typet &type = symbol.type;
  log.debug() << "global memory " << id2string(identifier) << " of type "
              << from_type(ns, "", type) << messaget::eom;

  bool per_object = shadow_per_object ? true : !can_be_initialized_rec(ns, expr);
  initialize_rec(ns, state, expr, per_object, state.global_fields);
}

void goto_symext::symex_field_static_init_string_constant(
    const namespacet &ns, goto_symex_statet &state,
    const ssa_exprt &expr, const exprt &rhs)
{
  const irep_idt &identifier =
      to_symbol_expr(expr.get_original_expr()).get_identifier();
  if(has_prefix(id2string(identifier), CPROVER_PREFIX))
    return;

  const index_exprt &index_expr =
    to_index_expr(to_address_of_expr(rhs).object());

  const typet &type = index_expr.array().type();
  log.debug() << "global memory "
              << id2string(to_string_constant(index_expr.array()).get_value())
              << " of type " << from_type(ns, "", type) << messaget::eom;

  bool per_object =
    shadow_per_object ? true : !can_be_initialized_rec(ns, expr);
  initialize_rec(
    ns, state, index_expr.array(), per_object, state.global_fields);
}

void goto_symext::symex_field_local_init(
  const namespacet &ns,
  goto_symex_statet &state,
  const ssa_exprt &expr)
{
  const symbolt &symbol =
    ns.lookup(to_symbol_expr(expr.get_original_expr()).get_identifier());

  if(symbol.is_auxiliary)
    return;
  if(id2string(symbol.name).find("__cs_") != std::string::npos)
    return;

  const typet &type = expr.type();
  ssa_exprt expr_l1 = remove_level_2(expr);
  log.debug() << "local memory " << id2string(expr_l1.get_identifier())
              << " of type " << from_type(ns, "", type) << messaget::eom;

  bool per_object = shadow_per_object ? true : !can_be_initialized_rec(ns, expr);
  initialize_rec(ns, state, expr_l1, per_object, state.local_fields);
}

void goto_symext::symex_field_dynamic_init(
  const namespacet &ns,
  goto_symex_statet &state,
  const exprt &expr,
  const mp_integer &size,
  bool per_object)
{
  if(per_object)
  {
    log.debug() << "dynamic memory of type " << from_type(ns, "", expr.type())
                << messaget::eom;

    initialize_rec(
      ns,
      state,
      expr,
      per_object,
      state.global_fields);
  }
  else
  {
    log.debug() << "dynamic memory of type " << from_type(ns, "", expr.type())
                << " and " << size << " element(s)" << messaget::eom;
    for(mp_integer index = 0; index < size; ++index)
    {
      initialize_rec(
        ns,
        state,
        dereference_exprt(
          plus_exprt(
            address_of_exprt(expr),
            from_integer(index, signed_long_int_type()))),
        per_object,
        state.global_fields);
    }
  }
}

std::pair<std::map<irep_idt, typet>, std::map<irep_idt, typet>>
goto_symext::preprocess_field_decl(
  goto_modelt &goto_model,
  message_handlert &message_handler)
{
  std::map<irep_idt, typet> global_fields;
  std::map<irep_idt, typet> local_fields;
  namespacet ns(goto_model.symbol_table);

  // get declarations
  Forall_goto_functions(f_it, goto_model.goto_functions)
  {
    goto_programt &goto_program = f_it->second.body;
    Forall_goto_program_instructions(target, goto_program)
    {
      if(!target->is_function_call())
        continue;

      const code_function_callt &code_function_call =
        to_code_function_call(target->code);
      const exprt &function = code_function_call.function();

      if(function.id() != ID_symbol)
        continue;

      const irep_idt &identifier = to_symbol_expr(function).get_identifier();

      if(identifier == CPROVER_PREFIX "field_decl_global")
      {
        convert_field_decl(
          ns, message_handler, code_function_call, global_fields);
        target->make_skip();
      }
      else if(identifier == CPROVER_PREFIX "field_decl_local")
      {
        convert_field_decl(
          ns, message_handler, code_function_call, local_fields);
        target->make_skip();
      }
    }
  }
  return std::make_pair(global_fields, local_fields);
}

void goto_symext::convert_field_decl(
  const namespacet &ns,
  message_handlert &message_handler,
  const code_function_callt &code_function_call,
  std::map<irep_idt, typet> &fields)
{
  INVARIANT(
    code_function_call.arguments().size() == 2,
    CPROVER_PREFIX "field_decl requires 2 arguments");
  irep_idt field_name = get_field_name(code_function_call.arguments()[0]);

  exprt expr = code_function_call.arguments()[1];

  messaget log(message_handler);
  log.debug() << "declare " << id2string(field_name) << " of type "
              << from_type(ns, "", expr.type()) << messaget::eom;

  // record field type
  fields[field_name] = expr.type();
}
