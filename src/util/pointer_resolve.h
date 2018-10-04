/// \file
/// Pointer utilities

#ifndef CPROVER_UTIL_POINTER_RESOLVE_H
#define CPROVER_UTIL_POINTER_RESOLVE_H

#include "expr.h"
#include "optional.h"

class messaget;
class namespacet;

void resolve_value_set_expr(
  const namespacet &ns,
  messaget &log,
  exprt &target,
  const exprt &value_set_expr);

#endif // CPROVER_UTIL_POINTER_RESOLVE_H
