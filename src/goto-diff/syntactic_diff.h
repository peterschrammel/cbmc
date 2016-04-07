/*******************************************************************\

Module: Syntactic GOTO-DIFF

Author: Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_GOTO_DIFF_SYNTACTIC_H
#define CPROVER_GOTO_DIFF_SYNTACTIC_H

#include "goto_diff.h"

class syntactic_difft : public goto_difft
{
 public:
  virtual bool operator()(
    const goto_modelt &goto_model1,
    const goto_modelt &goto_model2);

};

#endif
