/// \file
/// \author Peter Schrammel

#include "add_library_static_variable_initializations.h"

#include "goto_model.h"
#include "goto_convert_functions.h"

#include <linking/static_lifetime_init.h>
#include <util/message.h>

void add_library_static_variable_initializations(
    goto_modelt &goto_model,
    message_handlert &message_handler)
{
  auto const initialize_function_name = INITIALIZE_FUNCTION;
  PRECONDITION(
    goto_model.get_goto_functions().function_map.count(
      initialize_function_name) == 1);
  source_locationt source_location =
      goto_model.symbol_table.lookup(initialize_function_name)->location;

  goto_model.unload(initialize_function_name);
  static_lifetime_init(goto_model.symbol_table, source_location);
  goto_convert(
      initialize_function_name,
      goto_model.symbol_table,
      goto_model.goto_functions,
      message_handler);
}
