/*******************************************************************\

Module: Remove Shadow Memory Instrumentation

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Remove Shadow Memory Instrumentation

#include "remove_shadow_memory.h"

#include <util/base_type.h>
#include <util/c_types.h>
#include <util/cprover_prefix.h>
#include <util/fresh_symbol.h>
#include <util/invariant.h>
#include <util/message.h>
#include <util/pointer_offset_size.h>
#include <util/replace_expr.h>
#include <util/source_location.h>
#include <util/std_expr.h>
#include <util/type_eq.h>

#include <langapi/language_util.h>

#include "goto_model.h"

class remove_shadow_memoryt : public messaget
{
public:
  explicit remove_shadow_memoryt(message_handlert &message_handler)
    : messaget(message_handler)
  {
  }

  void operator()(goto_modelt &goto_model);

protected:
  irep_idt get_field_name(const exprt &string_expr);

  void convert_field_decl(
    const namespacet &ns,
    const code_function_callt &code_function_call,
    std::map<irep_idt, typet> &fields);

  void convert_get_field(
    const namespacet &ns,
    const code_function_callt &code_function_call,
    const std::map<irep_idt, typet> &fields,
    goto_programt::targett target,
    goto_programt &goto_program,
    const std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
      &address_fields);

  void convert_set_field(
    const namespacet &ns,
    const code_function_callt &code_function_call,
    const std::map<irep_idt, typet> &fields,
    symbol_tablet &symbol_table,
    const irep_idt &function_id,
    goto_programt::targett target,
    goto_programt &goto_program,
    std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
      &address_fields);

  void add_fields(
    const namespacet &ns,
    const std::map<irep_idt, typet> &fields,
    const exprt &expr,
    symbol_tablet &symbol_table,
    const irep_idt &function_id,
    const source_locationt &source_location,
    std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
      &address_fields,
    const irep_idt &field_name);
};

void remove_shadow_memoryt::operator()(goto_modelt &goto_model)
{
  std::map<irep_idt, typet> fields;
  // addresses must remain in sequence
  std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
    address_fields;
  namespacet ns(goto_model.symbol_table);
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

      if(identifier == CPROVER_PREFIX "set_field")
      {
        convert_set_field(
          ns,
          code_function_call,
          fields,
          goto_model.symbol_table,
          f_it->first,
          target,
          goto_program,
          address_fields);
      }
      else if(identifier == CPROVER_PREFIX "get_field")
      {
        convert_get_field(
          ns, code_function_call, fields, target, goto_program, address_fields);
      }
      else if(identifier == CPROVER_PREFIX "field_decl")
      {
        convert_field_decl(ns, code_function_call, fields);
      }
    }
  }
}

void remove_shadow_memoryt::convert_set_field(
  const namespacet &ns,
  const code_function_callt &code_function_call,
  const std::map<irep_idt, typet> &fields,
  symbol_tablet &symbol_table,
  const irep_idt &function_id,
  goto_programt::targett target,
  goto_programt &goto_program,
  std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
    &address_fields)
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

  debug() << "set " << id2string(field_name) << " for "
          << from_expr(ns, "", expr) << " to " << from_expr(ns, "", value)
          << eom;

  // add new symbols if needed
  add_fields(
    ns,
    fields,
    expr,
    symbol_table,
    function_id,
    target->source_location,
    address_fields,
    field_name);
  // t1: IF address_pair.first != expr THEN GOTO t0
  // t2: address_field[field_name] = value
  // t3: GOTO target
  // t0: IF ...
  // ...
  // target:
  const auto &addresses = address_fields.at(field_name);
  goto_programt::targett t0 = goto_program.insert_before(target);
  t0->make_goto(target);
  for(const auto &address_pair : addresses)
  {
    const exprt &address = address_pair.first;
    if(expr.type() == address.type())
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
      t1->make_goto(t4, not_exprt(equal_exprt(address, expr)));

      t0 = t4;
    }
  }
  target->make_skip();
}

void remove_shadow_memoryt::add_fields(
  const namespacet &ns,
  const std::map<irep_idt, typet> &fields,
  const exprt &expr,
  symbol_tablet &symbol_table,
  const irep_idt &function_id,
  const source_locationt &source_location,
  std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
    &address_fields,
  const irep_idt &field_name)
{
  auto &addresses = address_fields[field_name];
  std::size_t pos = 0;
  for(const auto &address_pair : addresses)
  {
    if(address_pair.first == expr)
      break;
    ++pos;
  }
  if(pos == addresses.size())
  {
    symbolt &new_symbol = get_fresh_aux_symbol(
      fields.at(field_name),
      id2string(function_id),
      from_expr(ns, "", expr) + "." + id2string(field_name),
      source_location,
      ID_C,
      symbol_table);

    addresses.push_back(
      std::pair<exprt, symbol_exprt>(expr, new_symbol.symbol_expr()));
  }
}

void remove_shadow_memoryt::convert_get_field(
  const namespacet &ns,
  const code_function_callt &code_function_call,
  const std::map<irep_idt, typet> &fields,
  goto_programt::targett target,
  goto_programt &goto_program,
  const std::map<irep_idt, std::vector<std::pair<exprt, symbol_exprt>>>
    &address_fields)
{
  INVARIANT(
    code_function_call.arguments().size() == 2,
    CPROVER_PREFIX "get_field requires 2 arguments");
  irep_idt field_name = get_field_name(code_function_call.arguments()[1]);

  exprt expr = code_function_call.arguments()[0];
  DATA_INVARIANT(
    expr.type().id() == ID_pointer,
    "shadow memory requires a pointer expression");

  debug() << "get " << id2string(field_name) << " for "
          << from_expr(ns, "", expr) << eom;

  // t1: IF address_pair.first != expr THEN GOTO n
  // t2: lhs = address_pair.second[field_name]
  // t3: GOTO target
  // t0: IF ...
  // ...
  // target:
  INVARIANT(
    address_fields.count(field_name) == 1,
    id2string(field_name) + " should exist");
  const auto &addresses = address_fields.at(field_name);
  goto_programt::targett t0 = goto_program.insert_before(target);
  t0->make_goto(target);
  for(const auto &address_pair : addresses)
  {
    const exprt &address = address_pair.first;
    if(expr.type() == address.type())
    {
      const exprt &field = address_pair.second;
      goto_programt::targett t4 = goto_program.insert_before(target);
      t4->make_goto(target);
      goto_programt::targett t3 = goto_program.insert_before(t4);
      t3->make_goto(target);
      goto_programt::targett t2 = goto_program.insert_before(t3);
      t2->make_assignment();
      const exprt &lhs = to_code_function_call(target->code).lhs();
      t2->code =
        code_assignt(lhs, typecast_exprt::conditional_cast(field, lhs.type()));
      goto_programt::targett t1 = t0;
      t1->make_goto(t4, not_exprt(equal_exprt(address, expr)));

      t0 = t4;
    }
  }
  target->make_skip();
}

void remove_shadow_memoryt::convert_field_decl(
  const namespacet &ns,
  const code_function_callt &code_function_call,
  std::map<irep_idt, typet> &fields)
{
  INVARIANT(
    code_function_call.arguments().size() == 2,
    CPROVER_PREFIX "field_decl requires 2 arguments");
  irep_idt field_name = get_field_name(code_function_call.arguments()[0]);

  exprt expr = code_function_call.arguments()[1];

  debug() << "declare " << id2string(field_name) << " of type "
          << from_type(ns, "", expr.type()) << eom;

  // record field type
  fields[field_name] = expr.type();
}

irep_idt remove_shadow_memoryt::get_field_name(const exprt &string_expr)
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

void remove_shadow_memory(
  goto_modelt &goto_model,
  message_handlert &_message_handler)
{
  remove_shadow_memoryt rsm(_message_handler);
  rsm(goto_model);
}
