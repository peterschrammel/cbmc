/*******************************************************************\

Module: Symex Shadow Memory Instrumentation

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Symex Shadow Memory Instrumentation

#include "goto_symex.h"

#include "symex_shadow_memory_util.h"

#include <util/arith_tools.h>
#include <util/c_types.h>
#include <util/cprover_prefix.h>
#include <util/expr_initializer.h>
#include <util/find_symbols.h>
#include <util/fresh_symbol.h>
#include <util/invariant.h>
#include <util/message.h>
#include <util/pointer_offset_size.h>
#include <util/pointer_resolve.h>
#include <util/prefix.h>
#include <util/range.h>
#include <util/std_expr.h>
#include <util/string_constant.h>

#include <langapi/language_util.h>

#include <goto-programs/goto_model.h>
#include <pointer-analysis/value_set_dereference.h>


void goto_symext::initialize_shadow_memory(
  goto_symex_statet &state,
  const exprt &expr,
  std::map<irep_idt, typet> &fields)
{
  typet type = ns.follow(expr.type());
  for(const auto &field_pair : fields)
  {
    exprt address_expr = expr;
    if(type.id() == ID_array && expr.id() == ID_symbol && !type.get_bool(ID_C_dynamic))
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
      state,
      address_expr,
      field_pair.first,
      type);

    const auto zero_value =
        zero_initializer(type, expr.source_location(), ns);
    CHECK_RETURN(zero_value.has_value());

    symex_assign(state, field, *zero_value);

    log.conditional_output(
      log.debug(),
      [field, this, address_expr](messaget::mstreamt &mstream) {
        mstream << "Shadow memory: initialize field " << id2string(field.get_identifier())
                << " for " << from_expr(ns, "", address_expr)
                << messaget::eom;
      });
  }
}

symbol_exprt goto_symext::add_field(
  goto_symex_statet &state,
  const exprt &expr,
  const irep_idt &field_name,
  const typet &field_type)
{
  auto &addresses = state.address_fields[field_name];
  symbolt &new_symbol = get_fresh_aux_symbol(
    field_type,
    id2string(state.source.function_id),
    "SM__" + from_expr(ns, "", expr) + "__" + id2string(field_name),
    state.source.pc->source_location,
    ID_C,
    state.symbol_table);

  addresses.push_back(goto_symex_statet::shadowed_addresst{
    expr, new_symbol.symbol_expr()});
  return new_symbol.symbol_expr();
}

/// returns true if we attempt to set/get a field on a NULL pointer
static bool set_field_check_null(
  const namespacet &ns,
  const messaget &log,
  const std::vector<exprt> &value_set,
  const exprt &expr)
{
  const null_pointer_exprt null_pointer(to_pointer_type(expr.type()));
  if(value_set.size() == 1 && filter_by_value_set(value_set, null_pointer))
  {
#ifdef DEBUG_SM
    log.conditional_output(
      log.debug(), [ns, null_pointer, expr](messaget::mstreamt &mstream) {
        mstream << "Shadow memory: value set match: " << from_expr(ns, "", null_pointer)
                << " <-- " << from_expr(ns, "", expr) << messaget::eom;
        mstream << "Shadow memory: ignoring set field on NULL object" << messaget::eom;
      });
#endif
    return true;
  }
  return false;
}

static value_set_dereferencet::valuet get_shadow(
  const exprt &shadow,
  const object_descriptor_exprt &matched_base_descriptor,
  const exprt &expr,
  const namespacet &ns,
  const messaget &log)
{
  object_descriptor_exprt shadowed_object = matched_base_descriptor;
  shadowed_object.object() = shadow;
  value_set_dereferencet::valuet shadow_dereference =
      value_set_dereferencet::build_reference_to(shadowed_object, expr, ns);
#ifdef DEBUG_SM
  log.debug() << "Shadow memory: shadow-deref: " << from_expr(ns, "", shadow_dereference.value) << messaget::eom;
#endif
  return shadow_dereference;
}

static exprt get_cond(
  const exprt &shadowed_address,
  const exprt &dereference_pointer,
  const exprt &matched_base,
  const exprt &expr,
  const namespacet &ns,
  const messaget &log)
{
  exprt cond = simplify_expr(
      and_exprt(
          equal_exprt(
              expr,
              typecast_exprt::conditional_cast(dereference_pointer, expr.type())),
          equal_exprt(
              shadowed_address,
              typecast_exprt::conditional_cast(
                  matched_base, shadowed_address.type()))),
      ns);
  log_cond(ns, log, cond);

  return cond;
}

static object_descriptor_exprt normalize(
    const object_descriptor_exprt &expr, const namespacet &ns)
{
  if(expr.object().id() == ID_symbol) {
    return expr;
  }
  if(expr.offset().id() == ID_unknown) {
    return expr;
  }

  object_descriptor_exprt result = expr;
  mp_integer offset = numeric_cast_v<mp_integer>(to_constant_expr(expr.offset()));
  exprt object = expr.object();

  while(true)
  {
    if(object.id() == ID_index)
    {
      const index_exprt &index_expr = to_index_expr(object);
      mp_integer index =
          numeric_cast_v<mp_integer>(to_constant_expr(index_expr.index()));

      offset += *pointer_offset_size(index_expr.type(), ns) * index;
      object = index_expr.array();
    }
    else if(object.id() == ID_member)
    {
      const member_exprt &member_expr = to_member_expr(object);
      const struct_typet &struct_type =
          to_struct_type(ns.follow(member_expr.struct_op().type()));
      offset +=
          *member_offset(struct_type, member_expr.get_component_name(), ns);
      object = member_expr.struct_op();
    }
    else
    {
      break;
    }
  }
  result.object() = object;
  result.offset() = from_integer(offset, expr.offset().type());
  return result;
}

static optionalt<exprt> get_shadow_memory_for_shadow_address(
    const exprt &expr,
    const std::vector<exprt> &value_set,
    const goto_symex_statet::shadowed_addresst &shadowed_address,
    const namespacet &ns,
    const messaget &log,
    exprt result,
    size_t &mux_size)
{
  log_value_set(ns, log, value_set);
  for(const auto &matched_object : value_set)
  {
    if(matched_object.id() != ID_object_descriptor)
    {
      log.warning()
          << "Shadow memory: value set contains unknown for "
          << from_expr(ns, "", expr)
          << messaget::eom;
      continue;
    }
    if(to_object_descriptor_expr(matched_object).root_object().id() == ID_integer_address)
    {
      log.warning()
          << "Shadow memory: value set contains integer_address for "
          << from_expr(ns, "", expr)
          << messaget::eom;
      continue;
    }

    object_descriptor_exprt matched_base_descriptor =
        normalize(to_object_descriptor_expr(matched_object), ns);

    value_set_dereferencet::valuet dereference =
        value_set_dereferencet::build_reference_to(matched_base_descriptor, expr, ns);

    const exprt matched_base = address_of_exprt(matched_base_descriptor.object());

    // NULL has already been handled in the caller; skip.
    if(matched_base.id() == ID_address_of &&
        to_address_of_expr(matched_base).object().id() == ID_null_object)
    {
      continue;
    }
    const value_set_dereferencet::valuet shadow_dereference =
        get_shadow(shadowed_address.shadow, matched_base_descriptor, expr, ns, log);
    log_value_set_match(
        ns, log, shadowed_address, matched_base, dereference, expr, shadow_dereference);

    const exprt cond = get_cond(
        shadowed_address.address, dereference.pointer, matched_base, expr, ns, log);
    if(cond.is_true())
    {
      return shadow_dereference.pointer;
    }
    else if(!cond.is_false())
    {
      mux_size++;
      if(result.is_nil())
        result = shadow_dereference.pointer;
      else
        result = if_exprt(cond, shadow_dereference.pointer, result);
    }
  }
  if(result.is_not_nil())
  {
    return result;
  }
  return {};
}

static optionalt<exprt> get_shadow_memory(
    const exprt &expr,
    const std::vector<exprt> &value_set,
    const std::vector<goto_symex_statet::shadowed_addresst> &addresses,
    const namespacet &ns,
    const messaget &log,
    size_t &mux_size)
{
  exprt result = nil_exprt();
  for (const auto &shadowed_address : addresses)
  {
    log_try_shadow_address(ns, log, shadowed_address);

    auto maybe_result = get_shadow_memory_for_shadow_address(
        expr, value_set, shadowed_address, ns, log, result, mux_size);
    if (maybe_result)
    {
      result = *maybe_result;
      if (result.id() == ID_symbol)
        break;
    }
  }
  if(result.is_not_nil())
  {
    return result;
  }
  return {};
}

static exprt get_original_ssa_expr(const exprt &expr)
{
  exprt full_expr = expr;
  exprt ssa_object = nil_exprt();
  exprt *maybe_object = &full_expr;
  while(true)
  {
    if(maybe_object->get_bool(ID_C_SSA_symbol))
    {
      exprt original_expr = to_ssa_expr(*maybe_object).get_original_expr();
      ssa_object = to_ssa_expr(*maybe_object).get_l1_object();
      *maybe_object = original_expr;
    }
    if(maybe_object->id() == ID_index)
    {
      maybe_object = &to_index_expr(*maybe_object).array();
    }
    else if(maybe_object->id() == ID_member)
    {
      maybe_object = &to_member_expr(*maybe_object).struct_op();
    }
    else
    {
      break;
    }
  }
  *maybe_object = ssa_object;
  return full_expr;
}

void goto_symext::symex_shadow_memory_copy(
    goto_symex_statet &state,
    const address_of_exprt &dest,
    const address_of_exprt &src)
{
  const std::unordered_set<irep_idt> dest_identifiers = find_symbol_identifiers(dest);
  if(std::find_if(
      dest_identifiers.begin(),
      dest_identifiers.end(),
      [](irep_idt identifier) {
        return id2string(identifier).find("SM__") != std::string::npos;
      }) != dest_identifiers.end())
  {
    return;
  }
  if(src.object().id() == ID_struct)
  {
    const auto &components = to_struct_type(ns.follow(src.object().type())).components();
    const auto &src_struct = to_struct_expr(src.object());

    for(const auto &comp_src : make_range(components).zip(src_struct.operands()))
    {
      const auto &comp = comp_src.first;
      symex_shadow_memory_copy(
          state,
          address_of_exprt(member_exprt{dest.object(), comp.get_name(), comp.type()}),
          address_of_exprt(comp_src.second));
    }
    return;
  }
  if(src.object().id() == ID_array)
  {
    const auto &src_array = to_array_expr(src.object());

    mp_integer index = 0;
    for(const auto &src_element : src_array.operands())
    {
      symex_shadow_memory_copy(
          state,
          address_of_exprt(index_exprt{dest.object(), from_integer(index, index_type()), src_element.type()}),
          address_of_exprt(src_element));
      ++index;
    }
    return;
  }
  const address_of_exprt dest_expr =
      address_of_exprt(get_original_ssa_expr(dest.object()));
  const address_of_exprt src_expr =
      address_of_exprt(get_original_ssa_expr(src.object()));
  // we ignore assignments to NULL
  const exprt src_without_casts = remove_casts(src.object());
  if(src_without_casts.is_constant() &&
      (to_constant_expr(src_without_casts).get_value() == ID_NULL ||
       to_constant_expr(src_without_casts).is_zero()))
  {
    return;
  }
  log.conditional_output(
      log.debug(), [this, dest_expr, src_expr](messaget::mstreamt &mstream) {
        mstream << "Shadow memory: copy from "
                << from_expr(ns, "", src_expr.object())
                << " to " << from_expr(ns, "", dest_expr.object())
                << messaget::eom;
      });
  for(const auto &entry : state.address_fields)
  {
    size_t mux_size = 0;
    std::vector<exprt> lhs_value_set = state.value_set.get_value_set(dest_expr, ns);
    optionalt<exprt> maybe_lhs = get_shadow_memory(
        dest_expr,
        lhs_value_set,
        entry.second,
        ns,
        log,
        mux_size);
    if(!maybe_lhs.has_value())
    {
      log.warning()
          << "Shadow memory: cannot copy shadow memory to "
          << from_expr(ns, "", dest_expr)
          << messaget::eom;
      continue;
    }
    std::vector<exprt> rhs_value_set = state.value_set.get_value_set(src_expr, ns);
    optionalt<exprt> maybe_rhs = get_shadow_memory(
        src_expr,
        rhs_value_set,
        entry.second,
        ns,
        log,
        mux_size);
    if(!maybe_rhs.has_value())
    {
      log.warning()
          << "Shadow memory: cannot copy shadow memory from "
          << from_expr(ns, "", src_expr)
          << messaget::eom;
      continue;
    }
    if(maybe_lhs->id() == ID_address_of && maybe_rhs->id() == ID_address_of)
    {
      symex_assign(
          state,
          state.field_sensitivity.apply(ns, state, to_address_of_expr(*maybe_lhs).object(), true),
          state.field_sensitivity.apply(ns, state, to_address_of_expr(*maybe_rhs).object(), false));
    }
    else
    {
      log.warning()
          << "Shadow memory: failed to copy shadow memory from "
          << from_expr(ns, "", src_expr)
          << " to "
          << from_expr(ns, "", dest_expr)
          << messaget::eom;
    }
  }
}


static optionalt<exprt> get_field(
  const namespacet &ns,
  const messaget &log,
  const std::vector<exprt> &value_set,
  const goto_symex_statet::shadowed_addresst &shadowed_address,
  const typet &field_type,
  const exprt &expr,
  exprt rhs,
  const typet &lhs_type,
  size_t &mux_size)
{
  bool found = false;
  for(const auto &matched_object : value_set)
  {
    if(matched_object.id() != ID_object_descriptor)
    {
      log.warning()
          << "Shadow memory: value set contains unknown for "
          << from_expr(ns, "", expr)
          << messaget::eom;
      continue;
    }
    if(to_object_descriptor_expr(matched_object).root_object().id() == ID_integer_address)
    {
      log.warning()
          << "Shadow memory: value set contains integer_address for "
          << from_expr(ns, "", expr)
          << messaget::eom;
      continue;
    }
    const object_descriptor_exprt &matched_base_descriptor =
        to_object_descriptor_expr(matched_object);
    const exprt &matched_base = address_of_exprt(matched_base_descriptor.object());

    // NULL has already been handled in the caller; skip.
    if(matched_base.id() == ID_address_of &&
       to_address_of_expr(matched_base).object().id() == ID_null_object)
    {
      continue;
    }

    value_set_dereferencet::valuet dereference =
      value_set_dereferencet::build_reference_to(matched_object, expr, ns);

    const value_set_dereferencet::valuet shadow_dereference =
        get_shadow(shadowed_address.shadow, matched_base_descriptor, expr, ns, log);
    log_value_set_match(
      ns, log, shadowed_address, matched_base, dereference, expr, shadow_dereference);

    if(is_void_pointer(dereference.pointer.type()))
    {
      log.warning()
          << "Shadow memory: cannot access void* for "
          << from_expr(ns, "", expr)
          << messaget::eom;
    }

    const bool is_union = matched_base_descriptor.type().id() == ID_union ||
        matched_base_descriptor.type().id() == ID_union_tag;
    // const exprt value = typecast_exprt::conditional_cast(shadow_dereference.value, lhs_type);
    exprt value;
    if(field_type.id() == ID_c_bool || field_type.id() == ID_bool)
    {
      value = typecast_exprt::conditional_cast(
          compute_or_over_cells(shadow_dereference.value, field_type, ns, log, is_union),
          lhs_type);
    }
    else
    {
      value = typecast_exprt::conditional_cast(
          compute_max_over_cells(shadow_dereference.value, field_type, ns, log, is_union),
          lhs_type);
    }
    const exprt cond = get_cond(
        shadowed_address.address, dereference.pointer, matched_base, expr, ns, log);
    if(cond.is_true())
    {
      return value;
    }
    else if(!cond.is_false())
    {
      mux_size++;
      found = true;
      if(rhs.is_nil())
      {
        rhs = if_exprt(cond, value, from_integer(-1, lhs_type));
      }
      else
      {
        rhs = if_exprt(cond, value, rhs);
      }
    }
  }
  if (found)
  {
    return typecast_exprt::conditional_cast(rhs, lhs_type);
  }
  return {};
}

void goto_symext::symex_set_field(
  goto_symex_statet &state,
  const code_function_callt &code_function_call)
{
  // parse set_field call
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
  log_set_field(ns, log, field_name, expr, value);
  INVARIANT(
    state.address_fields.count(field_name) == 1,
    id2string(field_name) + " should exist");
  const auto &addresses = state.address_fields.at(field_name);
  // const typet &field_type = get_field_type(field_name, state);

  // get value set
  std::vector<exprt> value_set = state.value_set.get_value_set(expr, ns);
  log_value_set(ns, log, value_set);
  if(set_field_check_null(ns, log, value_set, expr))
  {
    return;
  }

  // build lhs
  const exprt &rhs = value;
  size_t mux_size = 0;
  optionalt<exprt> maybe_lhs = get_shadow_memory(
      expr, value_set, addresses, ns, log, mux_size);

  // add to equation
  if(maybe_lhs.has_value())
  {
    log.debug() << "Shadow memory: mux size set_field: " << mux_size << messaget::eom;
    const exprt lhs = deref_expr(*maybe_lhs);
#ifdef DEBUG_SM
    log.debug() << "Shadow memory: LHS: " << from_expr(ns, "", lhs) << messaget::eom;
#endif
    // We replicate the rhs value on each byte of the value that we set.
    // This allows the get_field or/max semantics to operate correctly
    // on unions.
    exprt per_byte_rhs = duplicate_per_byte(rhs, lhs.type(), ns, log);
#ifdef DEBUG_SM
    log.debug() << "Shadow memory: RHS: " << from_expr(ns, "", per_byte_rhs) << messaget::eom;
#endif
    symex_assign(state, lhs, typecast_exprt::conditional_cast(per_byte_rhs, lhs.type()));
  }
  else
  {
    log.warning()
      << "Shadow memory: cannot set_field for "
      << from_expr(ns, "", expr)
      << messaget::eom;
  }
}

void goto_symext::symex_get_field(
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
  log_get_field(ns, log, field_name, expr);

  INVARIANT(
    state.address_fields.count(field_name) == 1,
    id2string(field_name) + " should exist");
  const auto &addresses = state.address_fields.at(field_name);
  // Should actually be fields.at(field_name)
  symbol_exprt lhs(CPROVER_PREFIX "get_field#return_value", signedbv_typet(32));
  const typet &field_type = get_field_type(field_name, state);

  std::vector<exprt> value_set = state.value_set.get_value_set(expr, ns);
  log_value_set(ns, log, value_set);

  exprt rhs = nil_exprt();
  size_t mux_size = 0;
  const null_pointer_exprt null_pointer(to_pointer_type(expr.type()));
  if(filter_by_value_set(value_set, null_pointer))
  {
    log_value_set_match(ns, log, null_pointer, expr);
    rhs = if_exprt(
      equal_exprt(expr, null_pointer),
      from_integer(0, lhs.type()),
      from_integer(-1, lhs.type()));
    mux_size = 1;
  }

  for(const auto &shadow_address : addresses)
  {
    log_try_shadow_address(ns, log, shadow_address);

    auto maybe_rhs = get_field(
      ns, log, value_set, shadow_address, field_type, expr, rhs, lhs.type(), mux_size);
    if(maybe_rhs)
    {
      rhs = *maybe_rhs;
      if(rhs.id() == ID_symbol)
        break;
    }
  }

  if(rhs.is_not_nil())
  {
    log.debug() << "Shadow memory: mux size get_field: " << mux_size << messaget::eom;
#ifdef DEBUG_SM
    log.debug() << "Shadow memory: RHS: " << from_expr(ns, "", rhs) << messaget::eom;
#endif
    symex_assign(
      state, lhs, typecast_exprt::conditional_cast(rhs, lhs.type()));
  }
  else
  {
    log.warning()
      << "Shadow memory: cannot get_field for "
      << from_expr(ns, "", expr)
      << messaget::eom;
    symex_assign(state, lhs, from_integer(0, lhs.type()));
  }
}

void goto_symext::symex_field_static_init(
  goto_symex_statet &state,
  const ssa_exprt &expr)
{
  if(state.source.function_id != CPROVER_PREFIX "initialize")
    return;

  if(expr.get_original_expr().id() != ID_symbol)
    return;

  const irep_idt &identifier =
    to_symbol_expr(expr.get_original_expr()).get_identifier();
  if(has_prefix(id2string(identifier), CPROVER_PREFIX))
    return;

  const symbolt &symbol = ns.lookup(identifier);

  if(symbol.is_auxiliary || !symbol.is_static_lifetime)
    return;
  if(id2string(symbol.name).find("__cs_") == 0)
    return;

  const typet &type = symbol.type;
  log.debug()
    << "Shadow memory: global memory " << id2string(identifier) << " of type "
    << from_type(ns, "", type) << messaget::eom;

  initialize_shadow_memory(state, expr, state.global_fields);
}

void goto_symext::symex_field_static_init_string_constant(
  goto_symex_statet &state,
  const ssa_exprt &expr,
  const exprt &rhs)
{
  const irep_idt &identifier =
    to_symbol_expr(expr.get_original_expr()).get_identifier();
  if(has_prefix(id2string(identifier), CPROVER_PREFIX))
    return;

  const index_exprt &index_expr =
    to_index_expr(to_address_of_expr(rhs).object());

  const typet &type = index_expr.array().type();
  log.debug()
    << "Shadow memory: global memory "
    << id2string(to_string_constant(index_expr.array()).get_value())
    << " of type " << from_type(ns, "", type) << messaget::eom;

  initialize_shadow_memory(
    state, index_expr.array(), state.global_fields);
}

void goto_symext::symex_field_local_init(
  goto_symex_statet &state,
  const ssa_exprt &expr)
{
  const symbolt &symbol =
    ns.lookup(to_symbol_expr(expr.get_original_expr()).get_identifier());

  if(symbol.is_auxiliary)
    return;
  const std::string symbol_name = id2string(symbol.name);
  if(
    symbol_name.find("malloc::") != std::string::npos &&
      (symbol_name.find("malloc_size") != std::string::npos ||
       symbol_name.find("malloc_res") != std::string::npos ||
       symbol_name.find("record_malloc") != std::string::npos ||
       symbol_name.find("record_may_leak") != std::string::npos))
  {
    return;
  }
  if(
    symbol_name.find("__builtin_alloca::") != std::string::npos &&
      (symbol_name.find("alloca_size") != std::string::npos ||
       symbol_name.find("record_malloc") != std::string::npos ||
       symbol_name.find("record_alloca") != std::string::npos ||
       symbol_name.find("res") != std::string::npos))
  {
    return;
  }
  if(symbol_name.find("__cs_") != std::string::npos)
    return;

  const typet &type = expr.type();
  ssa_exprt expr_l1 = remove_level_2(expr);
  log.debug()
    << "Shadow memory: local memory " << id2string(expr_l1.get_identifier())
    << " of type " << from_type(ns, "", type) << messaget::eom;

  initialize_shadow_memory(state, expr_l1, state.local_fields);
}

void goto_symext::symex_field_dynamic_init(
  goto_symex_statet &state,
  const exprt &expr,
  const side_effect_exprt &code)
{
  log.debug()
    << "Shadow memory: dynamic memory of type " << from_type(ns, "", expr.type())
    << messaget::eom;

  initialize_shadow_memory(state, expr, state.global_fields);
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
  for(auto &goto_function : goto_model.goto_functions.function_map)
  {
    goto_programt &goto_program = goto_function.second.body;
    Forall_goto_program_instructions(target, goto_program)
    {
      if(!target->is_function_call())
        continue;

      const code_function_callt &code_function_call =
        to_code_function_call(target->get_code());
      const exprt &function = code_function_call.function();

      if(function.id() != ID_symbol)
        continue;

      const irep_idt &identifier = to_symbol_expr(function).get_identifier();

      if(identifier == CPROVER_PREFIX "field_decl_global")
      {
        convert_field_decl(
          ns, message_handler, code_function_call, global_fields);
        target->turn_into_skip();
      }
      else if(identifier == CPROVER_PREFIX "field_decl_local")
      {
        convert_field_decl(
            ns, message_handler, code_function_call, local_fields);
        target->turn_into_skip();
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
  log.debug()
    << "Shadow memory: declare " << id2string(field_name) << " of type "
    << from_type(ns, "", expr.type()) << messaget::eom;
  if(!can_cast_type<bitvector_typet>(expr.type()))
  {
    throw unsupported_operation_exceptiont(
        "A shadow memory field must be of a bitvector type.");
  }
  if(to_bitvector_type(expr.type()).get_width() > 8)
  {
    throw unsupported_operation_exceptiont(
      "A shadow memory field must not be larger than 8 bits.");
  }

  // record field type
  fields[field_name] = expr.type();
}

