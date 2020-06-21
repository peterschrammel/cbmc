/*******************************************************************\

Module: Writing DIMACS Files

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Writing DIMACS Files

#ifndef CPROVER_SOLVERS_FLATTENING_BV_DIMACS_H
#define CPROVER_SOLVERS_FLATTENING_BV_DIMACS_H

#include "bv_pointers.h"

class bv_dimacst : public bv_pointerst
{
public:
  bv_dimacst(
    const namespacet &_ns,
    propt &_prop,
    message_handlert &message_handler,
    const std::string &_filename,
    const std::list<std::string> &vars_to_show)
    : bv_pointerst(_ns, _prop, message_handler),
      filename(_filename),
      vars_to_show(vars_to_show)
  {
  }

  virtual ~bv_dimacst()
  {
    show_prop_vars();
    write_dimacs();
  }

protected:
  const std::string filename;
  const std::list<std::string> &vars_to_show;
  bool write_dimacs();
  bool write_dimacs(std::ostream &);
  void show_prop_vars();
};

#endif // CPROVER_SOLVERS_FLATTENING_BV_DIMACS_H
