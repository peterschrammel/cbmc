/*******************************************************************\

Module: 

Author: Areej Alatawi

Date: October 2017

\*******************************************************************/

/// \file
/// 


#ifndef CPROVER_GOTO_INSTRUMENT_LOOP_TO_RECURSION_H
#define CPROVER_GOTO_INSTRUMENT_LOOP_TO_RECURSION_H


class goto_functionst;
class message_handlert;
class symbol_tablet;

void loop_to_recursion(
  goto_functionst &goto_functions,
  symbol_tablet &symbol_table,
  message_handlert &message_handler);


#endif // CPROVER_GOTO_INSTRUMENT_LOOP_TO_RECURSION_H
