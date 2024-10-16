/*******************************************************************\

Module: Java string literal processing

Author: Chris Smowton, chris.smowton@diffblue.com

\*******************************************************************/

#include "java_string_literals.h"
#include "java_root_class.h"
#include "java_string_literal_expr.h"
#include "java_types.h"
#include "java_utils.h"

#include <linking/static_lifetime_init.h>
#include <util/arith_tools.h>
#include <util/expr_initializer.h>
#include <util/namespace.h>
#include <util/string_utils.h>
#include <util/symbol_table_base.h>
#include <util/unicode.h>

/// Convert UCS-2 or UTF-16 to an array expression.
/// \param in: wide string to convert
/// \return Returns a Java char array containing the same wchars.
static array_exprt utf16_to_array(const std::wstring &in)
{
  const auto jchar=java_char_type();
  array_exprt ret(
    {}, array_typet(jchar, from_integer(in.length(), java_int_type())));
  for(const auto c : in)
    ret.copy_to_operands(from_integer(c, jchar));
  return ret;
}

symbol_exprt get_or_create_string_literal_symbol(
  const java_string_literal_exprt &string_expr,
  symbol_table_baset &symbol_table,
  bool string_refinement_enabled)
{
  const irep_idt value = string_expr.value();
  const struct_tag_typet string_type("java::java.lang.String");

  const std::string escaped_symbol_name = escape_non_alnum(id2string(value));
  const std::string escaped_symbol_name_with_prefix =
    JAVA_STRING_LITERAL_PREFIX "." + escaped_symbol_name;

  auto findit = symbol_table.symbols.find(escaped_symbol_name_with_prefix);
  if(findit != symbol_table.symbols.end())
    return findit->second.symbol_expr();

#ifndef CPROVER_INVARIANT_DO_NOT_CHECK
  const auto initialize_function = symbol_table.lookup(INITIALIZE_FUNCTION);
  INVARIANT(
    !initialize_function || initialize_function->value.is_nil(),
    "Cannot create more string literals after making " INITIALIZE_FUNCTION);
#endif

  // Create a new symbol:
  symbolt new_symbol{escaped_symbol_name_with_prefix, string_type, ID_java};
  new_symbol.type.set(ID_C_constant, true);
  new_symbol.base_name = escaped_symbol_name;
  new_symbol.pretty_name = value;
  new_symbol.is_lvalue = true;
  new_symbol.is_state_var = true;
  new_symbol.is_static_lifetime = true;

  namespacet ns(symbol_table);

  // Regardless of string refinement setting, at least initialize
  // the literal with @clsid = String
  struct_tag_typet jlo_symbol("java::java.lang.Object");
  const auto &jlo_struct = ns.follow_tag(jlo_symbol);
  struct_exprt jlo_init({}, jlo_symbol);
  const auto &jls_struct = ns.follow_tag(string_type);
  java_root_class_init(jlo_init, jlo_struct, "java::java.lang.String");

  // If string refinement *is* around, populate the actual
  // contents as well:
  if(string_refinement_enabled)
  {
    const array_exprt data =
      utf16_to_array(utf8_to_utf16_native_endian(id2string(value)));

    struct_exprt literal_init({}, new_symbol.type);
    literal_init.operands().resize(jls_struct.components().size());
    const std::size_t jlo_nb = jls_struct.component_number("@java.lang.Object");
    literal_init.operands()[jlo_nb] = jlo_init;

    const std::size_t length_nb = jls_struct.component_number("length");
    const typet &length_type = jls_struct.components()[length_nb].type();
    const exprt length = from_integer(data.operands().size(), length_type);
    literal_init.operands()[length_nb] = length;

    // Initialize the string with a constant utf-16 array:
    symbolt array_symbol{
      escaped_symbol_name_with_prefix + "_constarray", data.type(), ID_java};
    array_symbol.base_name = escaped_symbol_name + "_constarray";
    array_symbol.pretty_name = value;
    array_symbol.is_lvalue = true;
    // These are basically const global data:
    array_symbol.is_static_lifetime = true;
    array_symbol.is_state_var = true;
    array_symbol.value = data;
    array_symbol.type.set(ID_C_constant, true);

    if(symbol_table.add(array_symbol))
      throw "failed to add constarray symbol to symbol table";

    const symbol_exprt array_expr = array_symbol.symbol_expr();
    const address_of_exprt array_pointer(
      index_exprt(array_expr, from_integer(0, java_int_type())));

    const std::size_t data_nb = jls_struct.component_number("data");
    literal_init.operands()[data_nb] = array_pointer;

    // Associate array with pointer
    symbolt return_symbol{
      escaped_symbol_name_with_prefix + "_return_value", typet{}, ID_java};
    return_symbol.base_name = escaped_symbol_name + "_return_value";
    return_symbol.pretty_name =
      escaped_symbol_name.length() > 10
        ? escaped_symbol_name.substr(0, 10) + "..._return_value"
        : escaped_symbol_name + "_return_value";
    return_symbol.is_lvalue = true;
    return_symbol.is_static_lifetime = true;
    return_symbol.is_state_var = true;
    return_symbol.value = make_function_application(
      ID_cprover_associate_array_to_pointer_func,
      {array_symbol.value, array_pointer},
      java_int_type(),
      symbol_table);
    return_symbol.type = return_symbol.value.type();
    return_symbol.type.set(ID_C_constant, true);
    if(symbol_table.add(return_symbol))
      throw "failed to add return symbol to symbol table";

    // Initialise the literal structure with
    // (ABC_return_value, { ..., .length = N, .data = &ABC_constarray }),
    // using a C-style comma expression to indicate that we need the
    // _return_value global for its side-effects.
    exprt init_comma_expr(ID_comma);
    init_comma_expr.type() = literal_init.type();
    init_comma_expr.add_to_operands(
      return_symbol.symbol_expr(), std::move(literal_init));
    new_symbol.value = init_comma_expr;
  }
  else if(jls_struct.components().size()>=1 &&
          jls_struct.components()[0].get_name()=="@java.lang.Object")
  {
    // Case where something defined java.lang.String, so it has
    // a proper base class (always java.lang.Object in practical
    // JDKs seen so far)
    struct_exprt literal_init({std::move(jlo_init)}, new_symbol.type);
    for(const auto &comp : jls_struct.components())
    {
      if(comp.get_name()=="@java.lang.Object")
        continue;
      // Other members of JDK's java.lang.String we don't understand
      // without string-refinement. Just zero-init them; consider using
      // test-gen-like nondet object trees instead.
      const auto init =
        zero_initializer(comp.type(), string_expr.source_location(), ns);
      CHECK_RETURN(init.has_value());
      literal_init.copy_to_operands(*init);
    }
    new_symbol.value = literal_init;
  }
  else if(to_java_class_type(jls_struct).get_is_stub())
  {
    // Case where java.lang.String was stubbed, and so directly defines
    // @class_identifier
    new_symbol.value = jlo_init;
    new_symbol.value.type() = string_type;
  }

  bool add_failed = symbol_table.add(new_symbol);
  INVARIANT(
    !add_failed,
    "string literal symbol was already checked not to be "
    "in the symbol table, so adding it should succeed");

  return new_symbol.symbol_expr();
}

symbol_exprt get_or_create_string_literal_symbol(
  const irep_idt &string_value,
  symbol_table_baset &symbol_table,
  bool string_refinement_enabled)
{
  return get_or_create_string_literal_symbol(
    java_string_literal_exprt{string_value},
    symbol_table,
    string_refinement_enabled);
}
