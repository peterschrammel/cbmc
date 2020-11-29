#include "pointer_resolve.h"

#include "arith_tools.h"
#include "c_types.h"
#include "message.h"
#include "namespace.h"
#include "optional.h"
#include "pointer_offset_size.h"
#include "std_expr.h"
#include "std_types.h"

// remove this after debugging
#include <langapi/language_util.h>

static void remove_typecasts(exprt &expr)
{
  while(expr.id() == ID_typecast)
  {
    expr = to_typecast_expr(expr).op();
  }
}

static optionalt<exprt>
resolve_array_index(const namespacet &ns, messaget &log, exprt expr)
{
  remove_typecasts(expr);
  if(expr.type().id() != ID_pointer)
    return {};

  const typet &subtype = to_pointer_type(expr.type()).subtype();
  if(subtype.id() == ID_signedbv || subtype.id() == ID_unsignedbv ||
    subtype.id() == ID_pointer)
  {
    log.debug() << "      INTEGER" << messaget::eom;
    mp_integer subtype_bytes =
      to_bitvector_type(subtype).get_width() / mp_integer(8);
    if(expr.id() == ID_plus)
    {
      const plus_exprt &plus_expr = to_plus_expr(expr);
      log.debug() << "        PLUS " //<< plus_expr.op1().pretty()
                  << messaget::eom;
      exprt plus_op1 = plus_expr.op1();
      remove_typecasts(plus_op1);
      if(plus_op1.id() == ID_constant)
      {
        log.debug() << "          CONSTANT" << messaget::eom;
        return std::move(from_integer(
          numeric_cast_v<mp_integer>(to_constant_expr(plus_op1)) /
            subtype_bytes,
          signed_long_int_type()));
      }
      else if(plus_op1.id() == ID_symbol)
      {
        log.debug() << "          SYMBOL" << messaget::eom;
        return plus_op1;
      }
      else if(plus_op1.id() == ID_mult)
      {
        log.debug() << "          MULT" << messaget::eom;
        const mult_exprt &mult_expr = to_mult_expr(plus_op1);
        INVARIANT(
          numeric_cast_v<mp_integer>(to_constant_expr(mult_expr.op0())) ==
            subtype_bytes,
          "subtype bytes expected to match");
        exprt array_index = mult_expr.op1();
        remove_typecasts(array_index);
        return array_index;
      }
      else
      {
        log.debug() << "          UNKNOWN" << messaget::eom;
      }
    }
  }
  return {};
}

static optionalt<exprt>
resolve_array_object(const namespacet &ns, messaget &log, exprt expr)
{
  remove_typecasts(expr);
  if(expr.type().id() != ID_pointer)
    return {};

  const typet &subtype = to_pointer_type(expr.type()).subtype();
  if(subtype.id() == ID_signedbv || subtype.id() == ID_unsignedbv ||
    subtype.id() == ID_pointer)
  {
    if(expr.id() == ID_plus)
    {
      const plus_exprt &plus_expr = to_plus_expr(expr);
      return plus_expr.op0();
    }
  }
  return {};
}

void resolve_value_set_expr(
  const namespacet &ns,
  messaget &log,
  exprt &target,
  const exprt &value_set_expr)
{
  //log.status() << value_set.begin()->pretty() << messaget::eom;
  if(value_set_expr.id() == ID_unknown)
  {
    log.debug() << "VALUE SET UNKNOWN" << messaget::eom;
    auto maybe_array_index = resolve_array_index(ns, log, target);
    auto maybe_object = resolve_array_object(ns, log, target);
    if(maybe_object && maybe_array_index)
    {
      log.debug() << "  WITH ARRAY INDEX" << messaget::eom;
      target = address_of_exprt(index_exprt(*maybe_object, *maybe_array_index));
      log.debug() << "result: " << from_expr(ns, "", target) << messaget::eom;
    }
    return;    
  }

  if(value_set_expr.id() != ID_object_descriptor)
    return;
  //log.debug() << target.pretty() << messaget::eom;

  const object_descriptor_exprt &object_descriptor =
    to_object_descriptor_expr(value_set_expr);
  const typet &followed_type = ns.follow(object_descriptor.object().type());
  if(followed_type.id() == ID_struct)
  {
    log.debug() << "STRUCT" << messaget::eom;
    const struct_typet &struct_type = to_struct_type(followed_type);

    // TODO: This should be implemented inside get_subexpression_at_offset?
    auto component_offset =
      numeric_cast<mp_integer>(object_descriptor.offset());

    // TODO: strange uninitialized error from gcc when using optional here.
    exprt *array_index = nullptr;
    if(!component_offset.has_value())
    {
      log.debug() << "  COMPONENT" << messaget::eom;
      auto array_index2 = resolve_array_index(ns, log, target);
      if(array_index2.has_value())
      {
        array_index = &*array_index2;
        component_offset = mp_integer(0);
      }
    }
    if(!component_offset.has_value())
    {
      log.debug() << "  NO COMPONENT" << messaget::eom;
      return;
    }
    for(const auto &component : struct_type.components())
    {
      auto offset = member_offset(struct_type, component.get_name(), ns);
      if(!offset.has_value())
      {
        continue;
      }
      if(*component_offset == *offset)
      {
        log.debug() << "  COMPONENT IDENTIFIED" << messaget::eom;
        if(array_index)
        {
          log.debug() << "    WITH ARRAY INDEX" << messaget::eom;
          target = address_of_exprt(index_exprt(
            member_exprt(
              object_descriptor.object(),
              component.get_name(),
              component.type()),
            *array_index));
        }
        else
        {
          log.debug() << "    WITHOUT ARRAY INDEX" << messaget::eom;
          target = address_of_exprt(member_exprt(
            object_descriptor.object(),
            component.get_name(),
            component.type()));
        }
        log.debug() << "result: " << from_expr(ns, "", target) << messaget::eom;
        break;
      }
    }
  }
  else if(
    followed_type.id() == ID_signedbv || followed_type.id() == ID_unsignedbv)
  {
    log.debug() << "INTEGER" << messaget::eom;
    auto offset = numeric_cast<mp_integer>(object_descriptor.offset());
    optionalt<exprt> array_index = resolve_array_index(ns, log, target);
    if(array_index.has_value())
    {
      log.debug() << "    WITH ARRAY INDEX" << messaget::eom;
      exprt object = object_descriptor.object();
      if(object.id() == ID_index && to_index_expr(object).index().is_zero())
      {
        object = to_index_expr(object).array();
      }
      target = address_of_exprt(index_exprt(object, *array_index));
    }
    else
    {
      if(offset.has_value() && offset->is_zero())
      {
        log.debug() << "    WITH ZERO OFFSET" << messaget::eom;
        target = address_of_exprt(object_descriptor.object());
      }
    }
    log.debug() << "result: " << from_expr(ns, "", target) << messaget::eom;
  }
  else
  {
    log.debug() << "OTHER" << messaget::eom;
  }
}
