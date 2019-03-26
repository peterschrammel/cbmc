/*******************************************************************\

Module: Context-based Incremental Solver Interface

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Context-based interface for incremental solvers

#ifndef CPROVER_SOLVERS_PROP_PROP_ASSUMPTION_H
#define CPROVER_SOLVERS_PROP_PROP_ASSUMPTION_H

#include "literal.h"

class prop_assumptiont
{
public:
  virtual void set_assumptions(const bvt &) = 0;
  virtual bool is_in_conflict(literalt) const = 0;

  virtual ~prop_assumptiont() = default;
};

#endif // CPROVER_SOLVERS_PROP_PROP_ASSUMPTION_H