/// \file
/// LazyCSeq context-bounded concurrency SSA transformation

#ifndef CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H
#define CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H

#include "symex_target_equation.h"

class lazy_c_seqt
{
public:
  explicit lazy_c_seqt(const namespacet &_ns);

  void operator()(symex_target_equationt &, message_handlert &);
};

#endif //CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H
