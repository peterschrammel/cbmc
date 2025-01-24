/// \file
/// LazyCSeq context-bounded concurrency SSA transformation

#include "lazy_c_seq.h"

#include <util/cprover_prefix.h>
#include <util/pointer_expr.h>
#include <util/prefix.h>

void lazy_c_seqt::operator()(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  log.statistics() << "Adding LazyCSeq constraints with " << rounds << " rounds" << messaget::eom;

  for(const auto &step : equation.SSA_steps)
  {
    const std::string &file =
      id2string(step.source.pc->source_location().get_file());
    if(
      file.find("builtin-library") != std::string::npos ||
      file.find("built-in-additions") != std::string::npos)
    {
      continue;
    }
    if(step.is_shared_read() || step.is_shared_write())
    {
      const bool has_cprover_prefix =
        can_cast_expr<symbol_exprt>(step.ssa_lhs) &&
        has_prefix(
          id2string(to_symbol_expr(step.ssa_lhs).get_identifier()),
          CPROVER_PREFIX);
      if(has_cprover_prefix)
      {
        continue;
      }
    }
    if(step.is_shared_read() || step.is_shared_write())
    {
      if(can_cast_expr<symbol_exprt>(step.ssa_lhs))
      {
        const typet &type = to_symbol_expr(step.ssa_lhs).type();
        //log.warning() << "Type: " << type.pretty() << messaget::eom;
        if(
          can_cast_type<pointer_typet>(type) &&
          can_cast_type<struct_tag_typet>(to_pointer_type(type).base_type()))
        {
          if(
            id2string(to_struct_tag_type(to_pointer_type(type).base_type())
                        .get_identifier()) == "tag-_opaque_pthread_t")
          {
            continue;
          }
        }
      }
    }

    /*
    if(step.is_assignment() && step.source.pc->is_assign()) {
      const exprt &lhs = to_code_assign(step.source.pc->code()).lhs();
      const bool has_cprover_prefix = can_cast_expr<symbol_exprt>(lhs) &&
                                      has_prefix(id2string(to_symbol_expr(lhs).get_identifier()), CPROVER_PREFIX);
      if(has_cprover_prefix) {
        continue;
      }
    }*/

    if(step.is_shared_write())
    {
      log.warning() << "Thread: " << step.source.thread_nr
                    << "\tWrite: " << step.source.pc->location_number
                    << messaget::eom;
    }
    if(step.is_shared_read())
    {
      log.warning() << "Thread: " << step.source.thread_nr
                    << "\tRead: " << step.source.pc->location_number
                    << messaget::eom;
    }

    exprt tmp;
    simplify(tmp, ns);
  }
}