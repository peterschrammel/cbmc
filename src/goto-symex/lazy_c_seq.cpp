/// \file
/// LazyCSeq context-bounded concurrency SSA transformation

#include "lazy_c_seq.h"

void lazy_c_seqt::operator()(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  log.statistics() << "Adding LazyCSeq constraints" << messaget::eom;
}