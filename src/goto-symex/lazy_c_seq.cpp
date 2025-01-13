/// \file
/// LazyCSeq context-bounded concurrency SSA transformation

#include "lazy_c_seq.h"

void lazy_c_seqt::operator()(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  log.statistics() << "Adding LazyCSeq constraints" << messaget::eom;

  for(const auto &step : equation.SSA_steps)
  {
    if(step.is_shared_write())
    {
      log.warning() << "Thread: " << step.source.thread_nr
                    << "Write: " << step.source.pc->location_number
                    << messaget::eom;
    }
    if(step.is_shared_read())
    {
      log.warning() << "Thread: " << step.source.thread_nr
                    << "Read: " << step.source.pc->location_number
                    << messaget::eom;
    }
  }
}