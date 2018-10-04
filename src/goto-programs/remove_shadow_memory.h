/*******************************************************************\

Module: Remove Shadow Memory Instrumentation

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Remove Shadow Memory Instrumentation

#ifndef CPROVER_GOTO_PROGRAMS_REMOVE_SHADOW_MEMORY_H
#define CPROVER_GOTO_PROGRAMS_REMOVE_SHADOW_MEMORY_H

class goto_functionst;
class goto_programt;
class goto_modelt;
class message_handlert;
class symbol_tablet;

void remove_shadow_memory(
  goto_modelt &goto_model,
  message_handlert &_message_handler);

#endif // CPROVER_GOTO_PROGRAMS_REMOVE_SHADOW_MEMORY_H
