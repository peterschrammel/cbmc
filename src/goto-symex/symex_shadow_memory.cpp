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
  std::map<irep_idt, exprt> &fields)
{
  typet type = ns.follow(expr.type());
  for(const auto &field_pair : fields)
  {
    exprt address_expr = expr;
    if(type.id() == ID_array && expr.id() == ID_symbol
      && to_array_type(type).size().get_bool(ID_C_SSA_symbol))
    {
      address_expr.type() = remove_array_type_l2(address_expr.type());
      exprt original_expr = to_ssa_expr(expr).get_original_expr();
      original_expr.type() = remove_array_type_l2(original_expr.type());
      to_ssa_expr(address_expr).set_expression(original_expr);
    }
    if(address_expr.id() == ID_string_constant)
    {
      address_expr = address_of_exprt(address_expr);
      address_expr.type() = pointer_type(char_type());
    }
    else if(address_expr.id() == ID_dereference)
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

    if(field_pair.second.id() == ID_typecast &&
       to_typecast_expr(field_pair.second).op().is_zero())
    {
      const auto zero_value =
          zero_initializer(type, expr.source_location(), ns);
      CHECK_RETURN(zero_value.has_value());

      symex_assign(state, field, *zero_value);
    }
    else
    {
      exprt init_expr = field_pair.second;
      if(init_expr.id() == ID_typecast)
        init_expr = to_typecast_expr(field_pair.second).op();
      const auto init_value =
        expr_initializer(type, expr.source_location(), ns, init_expr);
      CHECK_RETURN(init_value.has_value());

      symex_assign(state, field, *init_value);
    }

    log.conditional_output(
      log.debug(),
      [field, field_pair, this, address_expr](messaget::mstreamt &mstream) {
        mstream << "Shadow memory: initialize field " << id2string(field.get_identifier())
                << " for " << from_expr(ns, "", address_expr)
                << " with initial value " << from_expr(ns, "", field_pair.second)
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
    state.source.pc->source_location(),
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

static void adjust_array_types(typet &type)
{
  if(type.id() != ID_pointer) {
    return;
  }
  const typet &subtype = to_pointer_type(type).subtype();
  if(subtype.id() == ID_array)
  {
    to_pointer_type(type).subtype() = to_array_type(subtype).subtype();
  }
  if(subtype.id() == ID_string_constant)
  {
    to_pointer_type(type).subtype() = char_type();
  }
}

static void simplify(exprt &expr)
{
  if(expr.id() != ID_equal)
    return;

  const equal_exprt &equal_expr = to_equal_expr(expr);

  if(equal_expr.lhs().id() != ID_address_of || equal_expr.rhs().id() != ID_address_of)
    return;

  if(to_address_of_expr(equal_expr.lhs()).object().id() != ID_string_constant ||
      to_address_of_expr(equal_expr.rhs()).object().id() != ID_index)
    return;

  if(!to_index_expr(to_address_of_expr(equal_expr.rhs()).object()).index().is_zero() ||
      to_index_expr(to_address_of_expr(equal_expr.rhs()).object()).array().id() != ID_string_constant)
    return;

  if(to_string_constant(to_address_of_expr(equal_expr.lhs()).object()).get_value() ==
      to_string_constant(to_index_expr(to_address_of_expr(equal_expr.rhs()).object()).array()).get_value())
  {
    expr = true_exprt();
  }
}

static exprt get_matched_base_cond(
  const exprt &shadowed_address,
  const exprt &matched_base_address,
  const namespacet &ns,
  const messaget &log)
{
  typet shadowed_address_type = shadowed_address.type();
  adjust_array_types(shadowed_address_type);
  exprt lhs = typecast_exprt::conditional_cast(shadowed_address, shadowed_address_type);
  exprt rhs = typecast_exprt::conditional_cast(matched_base_address, shadowed_address_type);
  exprt base_cond = simplify_expr(equal_exprt(lhs, rhs), ns);
  simplify(base_cond);
  if(base_cond.id() == ID_equal &&
    to_equal_expr(base_cond).lhs() == to_equal_expr(base_cond).rhs())
  {
    return true_exprt();
  }
  if(base_cond.id() == ID_equal)
  {
    const equal_exprt &equal_expr = to_equal_expr(base_cond);
    const bool
        equality_types_match = equal_expr.lhs().type() == equal_expr.rhs().type();
    DATA_INVARIANT_WITH_DIAGNOSTICS(
        equality_types_match,
        "types of expressions on each side of equality should match",
        irep_pretty_diagnosticst{equal_expr.lhs()},
        irep_pretty_diagnosticst{equal_expr.rhs()});
  }

  return base_cond;
}

static exprt get_matched_expr_cond(
    const exprt &dereference_pointer,
    const exprt &expr,
    const namespacet &ns,
    const messaget &log)
{
  typet expr_type = expr.type();
  adjust_array_types(expr_type);
  exprt expr_cond = simplify_expr(
          equal_exprt(
              typecast_exprt::conditional_cast(expr, expr_type),
              typecast_exprt::conditional_cast(dereference_pointer, expr_type)),
      ns);
  if(expr_cond.id() == ID_equal &&
      to_equal_expr(expr_cond).lhs() == to_equal_expr(expr_cond).rhs())
  {
    return true_exprt();
  }
  if(expr_cond.id() == ID_equal)
  {
    const equal_exprt &equal_expr = to_equal_expr(expr_cond);
    const bool
        equality_types_match = equal_expr.lhs().type() == equal_expr.rhs().type();
    DATA_INVARIANT_WITH_DIAGNOSTICS(
        equality_types_match,
        "types of expressions on each side of equality should match",
        irep_pretty_diagnosticst{equal_expr.lhs()},
        irep_pretty_diagnosticst{equal_expr.rhs()});
  }
  return expr_cond;
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

static bool are_types_compatible(const typet &expr_type, const typet &shadow_type)
{
  if(expr_type.id() != ID_pointer || shadow_type.id() != ID_pointer)
    return true;

  // We filter out the particularly annoying case of char ** being definitely
  // incompatible with char[].
  const typet &expr_subtype = to_pointer_type(expr_type).subtype();
  const typet &shadow_subtype = to_pointer_type(shadow_type).subtype();
  if (expr_subtype.id() == ID_pointer && to_pointer_type(expr_subtype).subtype().id() != ID_pointer
      && shadow_subtype.id() == ID_array && to_array_type(shadow_subtype).subtype().id() != ID_pointer)
  {
    return false;
  }
  if (shadow_subtype.id() == ID_pointer && to_pointer_type(shadow_subtype).subtype().id() != ID_pointer
      && expr_subtype.id() != ID_pointer)
  {
    return false;
  }
  return true;
}

/// We simplify &string_constant[0] to &string_constant to facilitate expression
/// equality for exact matching.
static void clean_string_constant(exprt &expr) {
  if(expr.id() == ID_index && to_index_expr(expr).index().is_zero() &&
      to_index_expr(expr).array().id() == ID_string_constant) {
    expr = to_index_expr(expr).array();
  }
}

static exprt build_if_else_expr(std::vector<exprt::operandst> conds_values) {
  DATA_INVARIANT(!conds_values.empty(),
                 "build_if_else_expr requires non-empty argument");
  exprt result = nil_exprt();
  for(auto cond_value : conds_values) {
    DATA_INVARIANT(cond_value.size() == 2,
                   "build_if_else_expr requires elements with condition and value");
    if(result.is_nil()) {
      result = cond_value.at(1);
    } else {
      result = if_exprt(cond_value.at(0), cond_value.at(1), result);
    }
  }
  return result;
}

/// Used for set_field and shadow memory copy
static std::vector<exprt::operandst> get_shadow_memory_for_matched_object(
    const exprt &expr,
    const exprt &matched_object,
    const std::vector<goto_symex_statet::shadowed_addresst> &addresses,
    const namespacet &ns,
    const messaget &log,
    bool &exact_match)
{
  std::vector<exprt::operandst> result;
  for (const auto &shadowed_address : addresses)
  {
    log_try_shadow_address(ns, log, shadowed_address);

    if(!are_types_compatible(expr.type(), shadowed_address.address.type()))
    {
#ifdef DEBUG_SM
      log.debug() << "Shadow memory: incompatible types "
        << from_type(ns, "", expr.type()) << ", "
        << from_type(ns, "", shadowed_address.address.type())
        << messaget::eom;
#endif
      continue;
    }

    object_descriptor_exprt matched_base_descriptor =
        normalize(to_object_descriptor_expr(matched_object), ns);

    value_set_dereferencet::valuet dereference =
        value_set_dereferencet::build_reference_to(matched_base_descriptor, expr, ns);

    exprt matched_base_address = address_of_exprt(matched_base_descriptor.object());
    clean_string_constant(to_address_of_expr(matched_base_address).op());

    // NULL has already been handled in the caller; skip.
    if(matched_base_descriptor.object().id() == ID_null_object)
    {
      continue;
    }
    const value_set_dereferencet::valuet shadow_dereference =
        get_shadow(shadowed_address.shadow, matched_base_descriptor, expr, ns, log);
    log_value_set_match(
        ns, log, shadowed_address, matched_base_address, dereference, expr, shadow_dereference);

    const exprt base_cond = get_matched_base_cond(
        shadowed_address.address, matched_base_address,ns, log);
    log_cond(ns, log, "base_cond", base_cond);
    if(base_cond.is_false())
    {
      continue;
    }

    const exprt expr_cond = get_matched_expr_cond(
        dereference.pointer, expr, ns, log);
    if(expr_cond.is_false()) {
      continue;
    }

    if(base_cond.is_true() && expr_cond.is_true()) {
#ifdef DEBUG_SM
      log.debug() << "exact match" << messaget::eom;
#endif
      exact_match = true;
      result.clear();
      result.push_back({base_cond, shadow_dereference.pointer});
      break;
    }

    if(base_cond.is_true())
    {
      // No point looking at further shadow addresses
      // as only one of them can match.
#ifdef DEBUG_SM
      log.debug() << "base match" << messaget::eom;
#endif
      result.clear();
      result.push_back({expr_cond, shadow_dereference.pointer});
      break;
    }
    else
    {
#ifdef DEBUG_SM
      log.debug() << "conditional match" << messaget::eom;
#endif
      result.push_back({and_exprt(base_cond, expr_cond), shadow_dereference.pointer});
    }
  }
  return result;
}

/// Used for set_field and shadow memory copy
static optionalt<exprt> get_shadow_memory(
    const exprt &expr,
    const std::vector<exprt> &value_set,
    const std::vector<goto_symex_statet::shadowed_addresst> &addresses,
    const namespacet &ns,
    const messaget &log,
    size_t &mux_size)
{
  std::vector<exprt::operandst> conds_values;
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
    if(matched_object.type().get_bool(ID_C_is_failed_symbol))
    {
      log.warning()
          << "Shadow memory: value set contains failed symbol for "
          << from_expr(ns, "", expr)
          << messaget::eom;
      continue;
    }

    bool exact_match = false;
    auto per_value_set_conds_values = get_shadow_memory_for_matched_object(
        expr, matched_object, addresses, ns, log, exact_match);

    if (!per_value_set_conds_values.empty())
    {
      if (exact_match) {
        conds_values.clear();
      }
      conds_values.insert(conds_values.begin(),
          per_value_set_conds_values.begin(), per_value_set_conds_values.end());
      mux_size += conds_values.size() - 1;
      if (exact_match) {
        break;
      }
    }
  }
  if(!conds_values.empty())
  {
    return build_if_else_expr(conds_values);
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

/// Used for get_field
//
// value_set = get_value_set(expr)
// foreach object in value_set:
//   if(object invalid) continue;
//   foreach shadow in SM:
//     if(incompatible(shadow.object, object)) continue;
//     base_match = object == shadow_object;
//     if(!base_match) continue;
//     shadowed_dereference.pointer = deref(shadow.shadowed_object, expr);
//     expr_match = shadowed_dereference == expr
//     if(!expr_match) continue;
//     shadow_dereference = deref(shadow.object, expr);
//     if(expr_match)
//       result = shadow_dereference.value [exact match]
//       break;
//     if(base_match) [shadow memory matches]
//       result += (expr_match,  shadow_dereference.value)
//       break;
//     result += (base_match & expr_match,  shadow_dereference.value)
//  if(exact_match)
//    return;
// return;
static std::vector<exprt::operandst> get_field(
  const namespacet &ns,
  const messaget &log,
  const exprt &matched_object,
  const std::vector<goto_symex_statet::shadowed_addresst> &addresses,
  const typet &field_type,
  const exprt &expr,
  const typet &lhs_type,
  bool &exact_match)
{
  std::vector<exprt::operandst> result;

  for(const auto &shadowed_address : addresses)
  {
    log_try_shadow_address(ns, log, shadowed_address);
    if(!are_types_compatible(expr.type(), shadowed_address.address.type()))
    {
#ifdef DEBUG_SM
      log.debug() << "Shadow memory: incompatible types "
          << from_type(ns, "", expr.type()) << ", "
          << from_type(ns, "", shadowed_address.address.type())
          << messaget::eom;
#endif
      continue;
    }
    const object_descriptor_exprt &matched_base_descriptor =
        to_object_descriptor_expr(matched_object);
    exprt matched_base_address = address_of_exprt(matched_base_descriptor.object());
    clean_string_constant(to_address_of_expr(matched_base_address).op());

    // NULL has already been handled in the caller; skip.
    if(matched_base_descriptor.object().id() == ID_null_object)
    {
      continue;
    }

    value_set_dereferencet::valuet dereference =
        value_set_dereferencet::build_reference_to(matched_object, expr, ns);

    if(is_void_pointer(dereference.pointer.type()))
    {
      log.warning()
          << "Shadow memory: cannot access void* for "
          << from_expr(ns, "", expr)
          << messaget::eom;
      continue;
    }

    const value_set_dereferencet::valuet shadow_dereference =
        get_shadow(shadowed_address.shadow, matched_base_descriptor, expr, ns, log);
    log_value_set_match(
        ns, log, shadowed_address, matched_base_address, dereference, expr, shadow_dereference);

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

    const exprt base_cond = get_matched_base_cond(
        shadowed_address.address, matched_base_address, ns, log);
    log_cond(ns, log, "base_cond", base_cond);
    if(base_cond.is_false())
    {
      continue;
    }

    const exprt expr_cond = get_matched_expr_cond(
        dereference.pointer, expr, ns, log);
    if(expr_cond.is_false()) {
      continue;
    }

    if(base_cond.is_true() && expr_cond.is_true()) {
#ifdef DEBUG_SM
      log.debug() << "exact match" << messaget::eom;
#endif
      exact_match = true;
      result.clear();
      result.push_back({base_cond, value});
      break;
    }

    if(base_cond.is_true())
    {
      // No point looking at further shadow addresses
      // as only one of them can match.
#ifdef DEBUG_SM
      log.debug() << "base match" << messaget::eom;
#endif
      result.clear();
      result.push_back({expr_cond, value});
      break;
    }
    else
    {
#ifdef DEBUG_SM
      log.debug() << "conditional match" << messaget::eom;
#endif
      result.push_back({and_exprt(base_cond, expr_cond), value});
    }
  }
  return result;
}

void goto_symext::symex_set_field(
  goto_symex_statet &state,
  const exprt::operandst &arguments)
{
  // parse set_field call
  INVARIANT(
      arguments.size() == 3,
    CPROVER_PREFIX "set_field requires 3 arguments");
  irep_idt field_name = get_field_name(arguments[1]);

  exprt expr = arguments[0];
  typet expr_type = expr.type();
  DATA_INVARIANT_WITH_DIAGNOSTICS(
    expr_type.id() == ID_pointer,
    "shadow memory requires a pointer expression",
    irep_pretty_diagnosticst{expr_type});

  exprt value = arguments[2];
  log_set_field(ns, log, field_name, expr, value);
  INVARIANT(
    state.address_fields.count(field_name) == 1,
    id2string(field_name) + " should exist");
  const auto &addresses = state.address_fields.at(field_name);
  // const typet &field_type = get_field_type(field_name, state);

  const typet &expr_subtype = to_pointer_type(expr_type).subtype();
  DATA_INVARIANT_WITH_DIAGNOSTICS(expr_subtype.id() != ID_struct &&
      expr_subtype.id() != ID_struct_tag &&
      expr_subtype.id() != ID_union &&
      expr_subtype.id() != ID_union_tag &&
      expr_subtype.id() != ID_array,
    "set_field requires pointer expression to primitive type",
    irep_pretty_diagnosticst{expr_subtype});

  // get value set
  replace_invalid_object_by_null(expr);
  // log_set_field(ns, log, field_name, expr, value);
  std::vector<exprt> value_set = state.value_set.get_value_set(expr, ns);
  log_value_set(ns, log, value_set);
  if(set_field_check_null(ns, log, value_set, expr))
  {
    log.warning() << "Shadow memory: cannot set shadow memory of NULL" << messaget::eom;
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
  const exprt &lhs,
  const exprt::operandst &arguments)
{
  INVARIANT(
      arguments.size() == 2,
    CPROVER_PREFIX "get_field requires 2 arguments");
  irep_idt field_name = get_field_name(arguments[1]);

  exprt expr = arguments[0];
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
  const exprt &field_init_expr = get_field_init_expr(field_name, state);

  replace_invalid_object_by_null(expr);
  // log_get_field(ns, log, field_name, expr);
  std::vector<exprt> value_set = state.value_set.get_value_set(expr, ns);
  log_value_set(ns, log, value_set);

  std::vector<exprt::operandst> rhs_conds_values;
  const null_pointer_exprt null_pointer(to_pointer_type(expr.type()));
  if(filter_by_value_set(value_set, null_pointer))
  {
    log_value_set_match(ns, log, null_pointer, expr);
    rhs_conds_values.push_back(
        {true_exprt(), typecast_exprt::conditional_cast(field_init_expr, lhs.type())});
  }

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
    if(matched_object.type().get_bool(ID_C_is_failed_symbol))
    {
      log.warning()
          << "Shadow memory: value set contains failed symbol for "
          << from_expr(ns, "", expr)
          << messaget::eom;
      continue;
    }

    bool exact_match = false;
    auto per_matched_object_conds_values = get_field(
      ns, log, matched_object, addresses, field_init_expr.type(), expr, lhs.type(), exact_match);
    if(exact_match) {
      rhs_conds_values.clear();
    }
    rhs_conds_values.insert(rhs_conds_values.end(),
        per_matched_object_conds_values.begin(), per_matched_object_conds_values.end());
    if(exact_match) {
      break;
    }
  }

  if(!rhs_conds_values.empty())
  {
    exprt rhs = typecast_exprt::conditional_cast(build_if_else_expr(rhs_conds_values), lhs.type());
    const size_t mux_size = rhs_conds_values.size() - 1;
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
    symex_assign(
        state, lhs, typecast_exprt::conditional_cast(field_init_expr, lhs.type()));
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
  if(
    expr.get_original_expr().id() == ID_symbol &&
      has_prefix(
        id2string(to_symbol_expr(expr.get_original_expr()).get_identifier()),
        CPROVER_PREFIX))
  {
    return;
  }
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

std::pair<std::map<irep_idt, exprt>, std::map<irep_idt, exprt>>
goto_symext::preprocess_field_decl(
  goto_modelt &goto_model,
  message_handlert &message_handler)
{
  std::map<irep_idt, exprt> global_fields;
  std::map<irep_idt, exprt> local_fields;
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
  std::map<irep_idt, exprt> &fields)
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
  fields[field_name] = expr;
}

