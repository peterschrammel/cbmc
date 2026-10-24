/*******************************************************************\

Module: Assembler -> Goto

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Assembler -> Goto

#include "goto_convert_class.h"

#include <iostream>

void goto_convertt::convert_asm(
  const code_asmt &code,
  goto_programt &dest)
{
  // __SZH_ADD_BEGIN__ : we do not support asm code for now
  std::cout << "Error: Deagle does not support asm code.\n";
  std::exit(1);
  // __SZH_ADD_END__

  // copy as OTHER
  copy(code, OTHER, dest);
}
