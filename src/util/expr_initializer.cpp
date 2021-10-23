/*******************************************************************\

Module: Expression Initialization

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Expression Initialization

#include "expr_initializer.h"

#include "arith_tools.h"
#include "bitvector_expr.h"
#include "c_types.h"
#include "magic.h"
#include "namespace.h"
#include "pointer_offset_size.h"
#include "std_code.h"

class expr_initializert
{
public:
  explicit expr_initializert(const namespacet &_ns) : ns(_ns)
  {
  }

  optionalt<exprt>
  operator()(const typet &type, const source_locationt &source_location, const exprt &init_expr)
  {
    return expr_initializer_rec(type, source_location, init_expr);
  }

protected:
  const namespacet &ns;

  optionalt<exprt> expr_initializer_rec(
    const typet &type,
    const source_locationt &source_location,
    const exprt &init_expr);
};

optionalt<exprt> expr_initializert::expr_initializer_rec(
  const typet &type,
  const source_locationt &source_location,
  const exprt &init_expr)
{
  const irep_idt &type_id=type.id();

  if(type_id==ID_unsignedbv ||
     type_id==ID_signedbv ||
     type_id==ID_pointer ||
     type_id==ID_c_enum ||
     type_id==ID_c_bit_field ||
     type_id==ID_bool ||
     type_id==ID_c_bool ||
     type_id==ID_floatbv ||
     type_id==ID_fixedbv)
  {
    exprt result;
    if(init_expr.id() == ID_nondet)
      result = side_effect_expr_nondett(type, source_location);
    else if(init_expr.is_zero())
      result = from_integer(0, type);
    else
      result = duplicate_per_byte(init_expr, type, ns);

    result.add_source_location()=source_location;
    return result;
  }
  else if(type_id==ID_rational ||
          type_id==ID_real)
  {
    exprt result;
    if(init_expr.id() == ID_nondet)
      result = side_effect_expr_nondett(type, source_location);
    else if(init_expr.is_zero())
      result = constant_exprt(ID_0, type);
    else
      result = duplicate_per_byte(init_expr, type, ns);

    result.add_source_location()=source_location;
    return result;
  }
  else if(type_id==ID_verilog_signedbv ||
          type_id==ID_verilog_unsignedbv)
  {
    exprt result;
    if(init_expr.id() == ID_nondet)
      result = side_effect_expr_nondett(type, source_location);
    else if(init_expr.is_zero())
    {
      const std::size_t width = to_bitvector_type(type).get_width();
      std::string value(width, '0');

      result = constant_exprt(value, type);
    }
    else
      result = duplicate_per_byte(init_expr, type, ns);

    result.add_source_location()=source_location;
    return result;
  }
  else if(type_id==ID_complex)
  {
    exprt result;
    if(init_expr.id() == ID_nondet)
      result = side_effect_expr_nondett(type, source_location);
    else if(init_expr.is_zero())
    {
      auto sub_zero =
        expr_initializer_rec(to_complex_type(type).subtype(), source_location, init_expr);
      if(!sub_zero.has_value())
        return {};

      result = complex_exprt(*sub_zero, *sub_zero, to_complex_type(type));
    }
    else
      result = duplicate_per_byte(init_expr, type, ns);

    result.add_source_location()=source_location;
    return result;
  }
  else if(type_id==ID_array)
  {
    const array_typet &array_type=to_array_type(type);

    if(array_type.size().is_nil())
    {
      // we initialize this with an empty array

      array_exprt value({}, array_type);
      value.type().size() = from_integer(0, size_type());
      value.add_source_location()=source_location;
      return std::move(value);
    }
    else
    {
      auto tmpval = expr_initializer_rec(array_type.element_type(), source_location, init_expr);
      if(!tmpval.has_value())
        return {};

      const auto array_size = numeric_cast<mp_integer>(array_type.size());
      if(
        array_type.size().id() == ID_infinity || !array_size.has_value() ||
        *array_size > MAX_FLATTENED_ARRAY_SIZE)
      {
        if(init_expr.id() == ID_nondet)
          return side_effect_expr_nondett(type, source_location);

        array_of_exprt value(*tmpval, array_type);
        value.add_source_location()=source_location;
        return std::move(value);
      }

      if(*array_size < 0)
        return {};

      array_exprt value({}, array_type);
      value.operands().resize(
        numeric_cast_v<std::size_t>(*array_size), *tmpval);
      value.add_source_location()=source_location;
      return std::move(value);
    }
  }
  else if(type_id==ID_vector)
  {
    const vector_typet &vector_type=to_vector_type(type);

    auto tmpval = expr_initializer_rec(vector_type.element_type(), source_location, init_expr);
    if(!tmpval.has_value())
      return {};

    const mp_integer vector_size =
      numeric_cast_v<mp_integer>(vector_type.size());

    if(vector_size < 0)
      return {};

    vector_exprt value({}, vector_type);
    value.operands().resize(numeric_cast_v<std::size_t>(vector_size), *tmpval);
    value.add_source_location()=source_location;

    return std::move(value);
  }
  else if(type_id==ID_struct)
  {
    const struct_typet::componentst &components=
      to_struct_type(type).components();

    struct_exprt value({}, type);

    value.operands().reserve(components.size());

    for(const auto &c : components)
    {
      if(c.type().id() == ID_code)
      {
        constant_exprt code_value(ID_nil, c.type());
        code_value.add_source_location()=source_location;
        value.add_to_operands(std::move(code_value));
      }
      else
      {
        const auto member = expr_initializer_rec(c.type(), source_location, init_expr);
        if(!member.has_value())
          return {};

        value.add_to_operands(std::move(*member));
      }
    }

    value.add_source_location()=source_location;

    return std::move(value);
  }
  else if(type_id==ID_union)
  {
    const union_typet &union_type = to_union_type(type);

    if(union_type.components().empty())
    {
      empty_union_exprt value{type};
      value.add_source_location() = source_location;
      return std::move(value);
    }

    const auto &widest_member = union_type.find_widest_union_component(ns);
    if(!widest_member.has_value())
      return {};

    auto component_value =
      expr_initializer_rec(widest_member->first.type(), source_location, init_expr);

    if(!component_value.has_value())
      return {};

    union_exprt value{widest_member->first.get_name(), *component_value, type};
    value.add_source_location() = source_location;

    return std::move(value);
  }
  else if(type_id==ID_c_enum_tag)
  {
    auto result = expr_initializer_rec(
      ns.follow_tag(to_c_enum_tag_type(type)), source_location, init_expr);

    if(!result.has_value())
      return {};

    // use the tag type
    result->type() = type;

    return *result;
  }
  else if(type_id==ID_struct_tag)
  {
    auto result = expr_initializer_rec(
      ns.follow_tag(to_struct_tag_type(type)), source_location, init_expr);

    if(!result.has_value())
      return {};

    // use the tag type
    result->type() = type;

    return *result;
  }
  else if(type_id==ID_union_tag)
  {
    auto result = expr_initializer_rec(
      ns.follow_tag(to_union_tag_type(type)), source_location, init_expr);

    if(!result.has_value())
      return {};

    // use the tag type
    result->type() = type;

    return *result;
  }
  else if(type_id==ID_string)
  {
    exprt result;
    if(init_expr.id() == ID_nondet)
      result = side_effect_expr_nondett(type, source_location);
    else if(init_expr.is_zero())
      result = constant_exprt(irep_idt(), type);
    else
      result = duplicate_per_byte(init_expr, type, ns);

    result.add_source_location()=source_location;
    return result;
  }
  else
    return {};
}

/// Create the equivalent of zero for type `type`.
/// \param type: Type of the target expression.
/// \param source_location: Location to record in all created sub-expressions.
/// \param ns: Namespace to perform type symbol/tag lookups.
/// \return An expression if a constant expression of the input type can be
///   built.
optionalt<exprt> zero_initializer(
  const typet &type,
  const source_locationt &source_location,
  const namespacet &ns)
{
  return expr_initializert(ns)(type, source_location, constant_exprt(ID_0, char_type()));
}

/// Create a non-deterministic value for type `type`, with all subtypes
/// independently expanded as non-deterministic values.
/// \param type: Type of the target expression.
/// \param source_location: Location to record in all created sub-expressions.
/// \param ns: Namespace to perform type symbol/tag lookups.
/// \return An expression if a non-deterministic expression of the input type
///   can be built.
optionalt<exprt> nondet_initializer(
  const typet &type,
  const source_locationt &source_location,
  const namespacet &ns)
{
  return expr_initializert(ns)(type, source_location, side_effect_expr_nondett(empty_typet(), source_location));
}

/// Create a value for type `type`, with all subtype bytes
/// initialized to the given value.
/// \param type: Type of the target expression.
/// \param source_location: Location to record in all created sub-expressions.
/// \param ns: Namespace to perform type symbol/tag lookups.
/// \param init_expr: Value to be used for initialization.
/// \return An expression if a byte-initialized expression of the input type
///   can be built.
optionalt<exprt> expr_initializer(
    const typet &type,
    const source_locationt &source_location,
    const namespacet &ns,
    const exprt &init_expr)
{
  return expr_initializert(ns)(type, source_location, init_expr);
}

static exprt or_values(const exprt::operandst &values, const typet &field_type)
{
  if(values.size() == 1)
  {
    return values[0];
  }
  return multi_ary_exprt(ID_bitor, values, field_type);
}

exprt duplicate_per_byte(
    const exprt &init_byte_expr,
    const typet &output_type,
    const namespacet &ns)
{
  if(output_type.id() == ID_unsignedbv || output_type.id() == ID_signedbv)
  {
    const size_t size = to_bitvector_type(output_type).get_width() / 8;
    if(init_byte_expr.is_constant())
    {
      const mp_integer value = numeric_cast_v<mp_integer>(to_constant_expr(init_byte_expr));
      mp_integer duplicated_value = value;
      for(size_t i = 1; i < size; ++i) {
        duplicated_value = bitwise_or(duplicated_value << 8,  value);
      }
      return from_integer(duplicated_value, output_type);
    }
    exprt::operandst values;
    values.push_back(init_byte_expr);
    for(size_t i = 1; i < size; ++i) {
      values.push_back(shl_exprt(init_byte_expr, from_integer(8*i, size_type())));
    }
    return or_values(values, output_type);
  }
  return typecast_exprt::conditional_cast(init_byte_expr, output_type);
}
