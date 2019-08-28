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

#include <langapi/language_util.h>

#include <goto-programs/goto_model.h>

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
    expr = expr.op0();
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

void goto_symext::initialize_rec(
  const namespacet &ns,
  goto_symex_statet &state,
  const exprt &expr,
  std::map<irep_idt, typet> &fields)
{
  typet type = ns.follow(expr.type());
#if 0
  if(type.id() == ID_array && !shadow_per_object)
  {
    exprt size_expr = to_array_type(type).size();
    if(!size_expr.is_constant())
    {
      log.error() << "constant array size expected:\n"
                  << size_expr.pretty()
                  << messaget::eom;
      throw 0;
    }
    mp_integer array_size;
    to_integer(to_constant_expr(size_expr), array_size);
    for(mp_integer index = 0; index < array_size; ++index)
    {
      initialize_rec(
        ns,
        state,
        index_exprt(expr, from_integer(index, signed_long_int_type())),
        fields);
    }
  }
  else if(type.id() == ID_struct && !shadow_per_object)
  {
    for(const auto &component : to_struct_type(type).components())
    {
      initialize_rec(ns, state, member_exprt(expr, component), fields);
    }
  }
#endif
  if(shadow_per_object || (type.id() != ID_array && type.id() != ID_struct))
  {
    for(const auto &field_pair : fields)
    {
      exprt address_expr = expr;
      //if(type.id() != ID_array)
      if (type.id() == ID_array) {
        address_expr.type() = remove_array_type_l2(address_expr.type());
        exprt original_expr = to_ssa_expr(expr).get_original_expr();
        original_expr.type() = remove_array_type_l2(original_expr.type());
        to_ssa_expr(address_expr).set_expression(original_expr);
      }
      address_expr = address_of_exprt(address_expr);
      /*else
    {
      // arrays appear as &array[0]
      address_expr = address_of_exprt(
        index_exprt(expr, from_integer(0, signed_long_int_type())));
      log.debug() << "array: " << from_expr(ns, "", address_expr)
                  << messaget::eom;
                  }*/

      symbol_exprt field =
        add_field(ns, state, address_expr, field_pair.first, fields);
      code_assignt code_assign(
        field, from_integer(mp_integer(0), field.type()));
      symex_assign(state, code_assign);

      log.debug() << "initialize field " << id2string(field.get_identifier())
                  << " for " << from_expr(ns, "", address_expr)
                  << messaget::eom;
    }
  }
}

symbol_exprt goto_symext::add_field(
  const namespacet &ns,
  goto_symex_statet &state,
  const exprt &expr,
  const irep_idt &field_name,
  std::map<irep_idt, typet> &fields)
{
  auto &addresses = address_fields[field_name];
  symbolt &new_symbol = get_fresh_aux_symbol(
    fields.at(field_name),
    id2string(state.source.function_id),
    from_expr(ns, "", expr) + "." + id2string(field_name),
    state.source.pc->source_location,
    ID_C,
    state.symbol_table);

  addresses.push_back(
    std::pair<exprt, symbol_exprt>(expr, new_symbol.symbol_expr()));
  return new_symbol.symbol_expr();
}

bool goto_symext::filter_by_value_set(const value_setst::valuest &value_set,
                                      const exprt &address)
{
  // log.debug() << "address: " << address.pretty() << messaget::eom;

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

  INVARIANT(
    address.id() == ID_address_of, "address of shadowed object expected");

  exprt expr2 = to_address_of_expr(address).object();
  if (expr2.id() == ID_index) {
    expr2 = to_index_expr(expr2).array();
  }
  INVARIANT(expr2.id() == ID_symbol, "symbol of shadowed object expected");

  for(const auto &e : value_set)
  {
    // log.debug() << "object: " << e.pretty() << messaget::eom;
    if(e.id() == ID_unknown)
      return true;

    if(e.id() != ID_object_descriptor)
      continue;

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

  log.debug() << "set_field: " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << " to " << from_expr(ns, "", value)
              << messaget::eom;

  if(shadow_per_object)
  {
    expr = pointer_object(expr);
    do_simplify(expr);
    remove_pointer_object(expr);
  }

  log.debug() << "set_field: " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << " to " << from_expr(ns, "", value)
              << messaget::eom;

  INVARIANT(
    address_fields.count(field_name) == 1,
    id2string(field_name) + " should exist");
  const auto &addresses = address_fields.at(field_name);
  const exprt &rhs = value;
  exprt lhs = nil_exprt();
  size_t mux_size = 0;
  value_setst::valuest value_set;
  state.value_set.get_value_set(expr, value_set, ns);
  for (const auto &e : value_set)
  {
    log.debug() << "value set: " << from_expr(ns, "", e) << messaget::eom;
  }
  /*
    bool has_entry = false;
    for(const auto &address_pair : addresses)
    {
      if(filter_by_value_set(value_set, address_pair.first))
      {
        has_entry = true;
        break;
      }
    }
  */
  const null_pointer_exprt null_pointer(to_pointer_type(expr.type()));
  if(value_set.size() == 1 && filter_by_value_set(value_set, null_pointer))
  {
    log.debug() << "value set match: " << from_expr(ns, "", null_pointer)
                << " <-- " << from_expr(ns, "", expr) << messaget::eom;
    log.debug() << "ignoring set field on NULL object" << messaget::eom;
    return;
  }
  for(const auto &address_pair : addresses)
  {
    const exprt &address = address_pair.first;

    // exact match
    /*    log.debug() << address.pretty() << messaget::eom
                        << " == " << messaget::eom
                        << expr.pretty() << messaget::eom;*/
    if(address == expr)
/*      address == code_function_call.arguments()[0] ||
      (code_function_call.arguments()[0].id() == ID_typecast &&
      address == to_typecast_expr(code_function_call.arguments()[0]).op()))*/
    {
      log.debug() << "exact match: " << from_expr(ns, "", address)
                  << " == " << from_expr(ns, "", expr) << messaget::eom;
      lhs = address_of_exprt(address_pair.second);
      mux_size = 1;
      break;
    }

    if(!filter_by_value_set(value_set, address))
      continue;

    log.debug() << "value set match: " << from_expr(ns, "", address)
                << " <-- " << from_expr(ns, "", expr) << messaget::eom;

    /*
        if(has_entry && !filter_by_value_set(value_set, address))
          continue;
    */
    /*    const typet &expr_subtype = to_pointer_type(expr_type).subtype();
        const typet &address_subtype =
       to_pointer_type(address.type()).subtype();
        if(expr_subtype == address_subtype ||
           (can_cast_type<bitvector_typet>(expr_subtype) &&
            can_cast_type<bitvector_typet>(address_subtype) &&
            to_bitvector_type(expr_subtype).get_width() ==
            to_bitvector_type(address_subtype).get_width()))*/
    {
      const exprt &field = address_pair.second;
      exprt cond = equal_exprt(
        address, typecast_exprt::conditional_cast(expr, address.type()));
      log.debug() << "cond: " << from_expr(ns, "", cond) << messaget::eom;
      // do_simplify(cond);
      if(!cond.is_false())
      {
        mux_size++;
        if(lhs.is_nil())
        {
          lhs = address_of_exprt(field);
        }
        else
        {
          lhs = if_exprt(cond, address_of_exprt(field), lhs);
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
    log.debug() << "cannot set_field for " << from_expr(ns, "", expr)
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

  log.debug() << "get_field: " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << messaget::eom;

  if(shadow_per_object)
  {
    expr = pointer_object(expr);
    do_simplify(expr);
    remove_pointer_object(expr);
  }

  log.debug() << "get_field: " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << messaget::eom;

  INVARIANT(
    address_fields.count(field_name) == 1,
    id2string(field_name) + " should exist");
  const auto &addresses = address_fields.at(field_name);
  // Should actually be fields.at(field_name)
  symbol_exprt lhs(CPROVER_PREFIX "get_field#return_value", signedbv_typet(32));
  exprt rhs = nil_exprt();
  size_t mux_size = 0;
  value_setst::valuest value_set;
  state.value_set.get_value_set(expr, value_set, ns);
  for (const auto &e : value_set)
  {
    log.debug() << "value set: " << from_expr(ns, "", e) << messaget::eom;
  }
  /*
    bool has_entry = false;
    for(const auto &address_pair : addresses)
    {
      if(filter_by_value_set(value_set, address_pair.first))
      {
        has_entry = true;
        break;
      }
    }
  */

  const null_pointer_exprt null_pointer(to_pointer_type(expr.type()));
  if(filter_by_value_set(value_set, null_pointer))
  {
    log.debug() << "value set match: " << from_expr(ns, "", null_pointer)
                << " <-- " << from_expr(ns, "", expr) << messaget::eom;
    rhs = if_exprt(equal_exprt(expr, null_pointer),
      from_integer(0, lhs.type()), from_integer(-1, lhs.type()));
    mux_size = 1;
  }

  for(const auto &address_pair : addresses)
  {
    const exprt &address = address_pair.first;

    // exact match
    if(address == expr)
    {
      log.debug() << "exact match: " << from_expr(ns, "", address)
                  << " == " << from_expr(ns, "", expr) << messaget::eom;
      rhs = typecast_exprt::conditional_cast(address_pair.second, lhs.type());
      mux_size = 1;
      break;
    }

    if(!filter_by_value_set(value_set, address))
      continue;

    log.debug() << "value set match: " << from_expr(ns, "", address)
                << " <-- " << from_expr(ns, "", expr) << messaget::eom;

/*
    if(has_entry && !filter_by_value_set(value_set, address))
      continue;
*/
/*    const typet &expr_subtype = to_pointer_type(expr_type).subtype();
    const typet &address_subtype = to_pointer_type(address.type()).subtype();
    if(expr_subtype == address_subtype ||
       (can_cast_type<bitvector_typet>(expr_subtype) &&
        can_cast_type<bitvector_typet>(address_subtype) &&
        to_bitvector_type(expr_subtype).get_width() ==
        to_bitvector_type(address_subtype).get_width()))*/
    {
      const exprt &field = address_pair.second;
      exprt cond = equal_exprt(
        address, typecast_exprt::conditional_cast(expr, address.type()));
      log.debug() << "cond: " << from_expr(ns, "", cond) << messaget::eom;
      // do_simplify(cond);
      if(!cond.is_false())
      {
        mux_size++;
        if(rhs.is_nil())
        {
          rhs = if_exprt(
            cond, typecast_exprt::conditional_cast(field, lhs.type()),
            from_integer(-1, lhs.type()));
          // rhs = typecast_exprt::conditional_cast(field, lhs.type());
        }
        else
        {
          rhs = if_exprt(
            cond, typecast_exprt::conditional_cast(field, lhs.type()), rhs);
        }
      }
    }
  }
  if (rhs.is_not_nil()) {
    log.debug() << "mux size get_field: " << mux_size << messaget::eom;
    symex_assign(state, code_assignt(lhs, typecast_exprt::conditional_cast(
                                              rhs, lhs.type())));
  } else {
    log.debug() << "cannot get_field for " << from_expr(ns, "", expr)
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

  initialize_rec(ns, state, expr, global_fields);
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

  initialize_rec(ns, state, expr_l1, local_fields);
}

void goto_symext::symex_field_dynamic_init(
  const namespacet &ns,
  goto_symex_statet &state,
  const exprt &expr,
  const mp_integer &size)
{
  if(shadow_per_object)
  {
    log.debug() << "dynamic memory of type " << from_type(ns, "", expr.type())
                << messaget::eom;

    initialize_rec(
      ns,
      state,
      expr,
      global_fields);
  }
#if 0
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
          plus_exprt(expr, from_integer(index, signed_long_int_type()))),
        global_fields);
    }
  }
#endif
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
