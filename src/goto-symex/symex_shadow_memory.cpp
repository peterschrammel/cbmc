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

#if 0
static typet c_sizeof_type_rec(const exprt &expr)
{
  const irept &sizeof_type=expr.find(ID_C_c_sizeof_type);

  if(!sizeof_type.is_nil())
  {
    return static_cast<const typet &>(sizeof_type);
  }
  else if(expr.id()==ID_mult)
  {
    forall_operands(it, expr)
    {
      typet t=c_sizeof_type_rec(*it);
      if(t.is_not_nil())
        return t;
    }
  }

  return empty_typet();
}

static mp_integer get_malloc_size(const exprt &size, const namespacet &ns)
{
  if(size.id() == ID_typecast)
  {
    return get_malloc_size(size.op0(), ns);
  }
  else if(size.id() == ID_mult &&
          size.operands().size()==2)
  {
    return get_malloc_size(size.op0(), ns) *
      get_malloc_size(size.op1(), ns);
  }
#if 0
  else if(size.find(ID_C_c_sizeof_type).is_not_nil())
  {
    const auto offset = pointer_offset_size(c_sizeof_type_rec(size), ns);
    INVARIANT(offset.has_value(), "failed to get sizeof type size");
    return offset.value();
  }
#endif
  else if(size.id() == ID_constant)
  {
    mp_integer result;
    bool error = to_integer(size, result);
    CHECK_RETURN(!error);
    return result;
  }
  else
    INVARIANT(false, "constant malloc size expected");
}
#endif

void goto_symext::initialize_rec(
  const namespacet &ns,
  goto_symex_statet &state,
  const exprt &expr)
{
  typet type = ns.follow(expr.type());
  if(type.id() == ID_array)
  {
    const constant_exprt &size_expr =
      to_constant_expr(to_array_type(type).size());
    mp_integer array_size;
    to_integer(size_expr, array_size);
    for(mp_integer index = 0; index < array_size; ++index)
    {
      initialize_rec(
        ns,
        state,
        index_exprt(expr, from_integer(index, signed_long_int_type())));
    }
  }
  else if(type.id() == ID_struct)
  {
    for(const auto &component : to_struct_type(type).components())
    {
      initialize_rec(ns, state, member_exprt(expr, component));
    }
  }
  else
  {
    for(const auto &field_pair : fields)
    {
      symbol_exprt field =
        add_field(ns, state, address_of_exprt(expr), field_pair.first);
      code_assignt code_assign(
        field, from_integer(mp_integer(0), field.type()));
      symex_assign(state, code_assign);

      log.debug() << "initialize field " << id2string(field.get_identifier())
                  << " for " << from_expr(ns, "", address_of_exprt(expr))
                  << messaget::eom;
    }
  }
}

symbol_exprt goto_symext::add_field(
  const namespacet &ns,
  goto_symex_statet &state,
  const exprt &expr,
  const irep_idt &field_name)
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
  DATA_INVARIANT(
    expr.type().id() == ID_pointer,
    "shadow memory requires a pointer expression");

  exprt value = code_function_call.arguments()[2];

  log.debug() << "set " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << " to " << from_expr(ns, "", value)
              << messaget::eom;

#if 0
  const auto &addresses = address_fields.at(field_name);

  // t1: IF address_pair.first != expr THEN GOTO t0
  // t2: address_field[field_name] = value
  // t3: GOTO target
  // t0: IF ...
  // ...
  // target:
  goto_programt::targett t0 = goto_program.insert_before(target);
  t0->make_goto(target);
  for(const auto &address_pair : addresses)
  {
    const exprt &address = address_pair.first;
    if(expr.type() == address.type() ||
       to_pointer_type(expr.type()).get_width() ==
       to_pointer_type(address.type()).get_width())
    {
      const exprt &field = address_pair.second;
      goto_programt::targett t4 = goto_program.insert_before(target);
      t4->make_goto(target);
      goto_programt::targett t3 = goto_program.insert_before(t4);
      t3->make_goto(target);
      goto_programt::targett t2 = goto_program.insert_before(t3);
      t2->make_assignment();
      t2->code = code_assignt(
        field, typecast_exprt::conditional_cast(value, field.type()));
      goto_programt::targett t1 = t0;
      t1->make_goto(t4, not_exprt(equal_exprt(address, typecast_exprt::conditional_cast(expr, address.type()))));

      t0 = t4;
    }
  }
  target->make_skip();
#endif
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
  DATA_INVARIANT(
    expr.type().id() == ID_pointer,
    "shadow memory requires a pointer expression");

  log.debug() << "get " << id2string(field_name) << " for "
              << from_expr(ns, "", expr) << messaget::eom;

#if 0  
  INVARIANT(
    address_fields.count(field_name) == 1,
    id2string(field_name) + " should exist");
  const auto &addresses = address_fields.at(field_name);
  exprt lhs = to_code_function_call(target->code).lhs();
  exprt rhs;
  bool first = true;
  for(const auto &address_pair : addresses)
  {
    const exprt &address = address_pair.first;
    if(
      expr.type() == address.type() ||
      to_pointer_type(expr.type()).get_width() ==
        to_pointer_type(address.type()).get_width())
    {
      const exprt &field = address_pair.second;
      if(first)
      {
        first = false;
        rhs = typecast_exprt::conditional_cast(field, lhs.type());
      }
      else
      {
        exprt cond = equal_exprt(
          address, typecast_exprt::conditional_cast(expr, address.type()));
        rhs = if_exprt(
          cond, typecast_exprt::conditional_cast(field, lhs.type()), rhs);
      }
    }
  }
  target->make_assignment();
  target->code =
    code_assignt(lhs, typecast_exprt::conditional_cast(rhs, lhs.type()));
#endif
}

void goto_symext::symex_field_static_init(
  const namespacet &ns,
  goto_symex_statet &state,
  const code_assignt &code_assign)
{
  if(state.source.function_id != CPROVER_PREFIX "initialize")
    return;

  find_symbols_sett identifiers;
  find_symbols(code_assign.lhs(), identifiers);
  if(identifiers.size() != 1)
    return;

  const irep_idt &identifier = *identifiers.begin();
  if(has_prefix(id2string(identifier), CPROVER_PREFIX))
    return;

  const symbolt &symbol = ns.lookup(identifier);

  if(symbol.is_auxiliary || !symbol.is_static_lifetime)
    return;
  if(id2string(symbol.name).find("__cs_") != std::string::npos)
    return;

  const typet &type = symbol.type;
  log.debug() << "memory " << id2string(symbol.name) << " of type "
              << from_type(ns, "", type) << messaget::eom;

  initialize_rec(ns, state, symbol.symbol_expr());
}

#if 0
void goto_symext::symex_field_local_init()
{
if(target->is_decl())
      {
        const code_declt &code_decl = to_code_decl(target->code);

        const symbolt &symbol =
          goto_model.symbol_table.lookup_ref(code_decl.get_identifier());

        if(symbol.is_auxiliary)
          continue;
        if(id2string(symbol.name).find("__cs_") != std::string::npos)
          continue;

        const typet &type = code_decl.symbol().type();
        log.debug()
          << "memory " << id2string(code_decl.get_identifier()) << " of type "
          << from_type(ns, "", type) << messaget::eom;

        initialize_rec(
          ns,
          fields,
          code_decl.symbol(),
          goto_model.symbol_table,
          f_it->first,
          target,
          goto_program,
          address_fields);
      }
}
#endif

void goto_symext::symex_field_dynamic_init()
{
#if 0
  mp_integer malloc_size = get_malloc_size(size_expr, ns);
  for(mp_integer index = 0; index < malloc_size; ++index)
  {
    initialize_rec(
              ns,
              fields,
              dereference_exprt(
                plus_exprt(code_function_call.lhs(), from_integer(index, signed_long_int_type()))),
              goto_model.symbol_table,
              f_it->first,
              target,
              goto_program,
              address_fields);
   }
#endif
}

std::map<irep_idt, typet> goto_symext::preprocess_field_decl(
  goto_modelt &goto_model,
  message_handlert &message_handler)
{
  std::map<irep_idt, typet> fields;
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

      if(identifier == CPROVER_PREFIX "field_decl")
      {
        convert_field_decl(ns, message_handler, code_function_call, fields);
        target->make_skip();
      }
    }
  }
  return fields;
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
