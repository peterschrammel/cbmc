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
  log.statistics() << "Adding LazyCSeq constraints with " << rounds << " rounds"
                   << messaget::eom;

  std::
    unordered_map<unsigned, symex_target_equationt::SSA_stepst::const_iterator>
      reads;
  std::
    unordered_map<unsigned, symex_target_equationt::SSA_stepst::const_iterator>
      writes;
  collect_reads_and_writes(equation.SSA_steps, reads, writes, message_handler);

  exprt tmp;
  simplify(tmp, ns);
}

void lazy_c_seqt::collect_reads_and_writes(
  symex_target_equationt::SSA_stepst ssa_steps,
  std::unordered_map<
    unsigned,
    symex_target_equationt::SSA_stepst::const_iterator> reads,
  std::unordered_map<
    unsigned,
    symex_target_equationt::SSA_stepst::const_iterator> writes,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    const std::string &file =
      id2string(s_it->source.pc->source_location().get_file());
    if(
      file.find("builtin-library") != std::string::npos ||
      file.find("built-in-additions") != std::string::npos)
    {
      continue;
    }
    if(s_it->is_shared_read() || s_it->is_shared_write())
    {
      const bool has_cprover_prefix =
        can_cast_expr<symbol_exprt>(s_it->ssa_lhs) &&
        has_prefix(
          id2string(to_symbol_expr(s_it->ssa_lhs).get_identifier()),
          CPROVER_PREFIX);
      if(has_cprover_prefix)
      {
        continue;
      }
    }
    if(s_it->is_shared_read() || s_it->is_shared_write())
    {
      if(can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        const typet &type = to_symbol_expr(s_it->ssa_lhs).type();
        //log.warning() << "Type of " << to_symbol_expr(s_it->ssa_lhs).get_identifier() << ": " << type.pretty() << messaget::eom;
        if(
          can_cast_type<pointer_typet>(type) &&
          can_cast_type<struct_tag_typet>(to_pointer_type(type).base_type()))
        {
          if(
            id2string(to_struct_tag_type(to_pointer_type(type).base_type())
                        .get_identifier()) == "tag-_opaque_pthread_t")
          {
            //log.warning() << "Skipped" << messaget::eom;
            continue;
          }
        }
      }
    }
    //log.warning() << "Not skipped" << messaget::eom;

    /*
    if(s_it->is_assignment() && s_it->source.pc->is_assign()) {
      const exprt &lhs = to_code_assign(s_it->source.pc->code()).lhs();
      const bool has_cprover_prefix = can_cast_expr<symbol_exprt>(lhs) &&
                                      has_prefix(id2string(to_symbol_expr(lhs).get_identifier()), CPROVER_PREFIX);
      if(has_cprover_prefix) {
        continue;
      }
    }*/

    if(s_it->is_shared_write())
    {
      // TODO: this may be too restrictive
      if(can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        log.warning() << "Thread: " << s_it->source.thread_nr
                      << "\tWrite: " << s_it->source.pc->location_number
                      << "   \t"
                      << to_symbol_expr(s_it->ssa_lhs).get_identifier() << "\tL"
                      << s_it->source.pc->location_number << messaget::eom;
        writes[s_it->source.thread_nr] = s_it;
      }
      else
      {
        log.warning() << "Skipping: "
                      << "Thread: " << s_it->source.thread_nr
                      << "\tWrite: " << s_it->source.pc->location_number
                      << messaget::eom;
      }
    }
    if(s_it->is_shared_read())
    {
      // TODO: this may be too restrictive
      if(can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        log.warning() << "Thread: " << s_it->source.thread_nr
                      << "\tRead: " << s_it->source.pc->location_number
                      << "     \t"
                      << to_symbol_expr(s_it->ssa_lhs).get_identifier() << "\tL"
                      << s_it->source.pc->location_number << messaget::eom;

        reads[s_it->source.thread_nr] = s_it;
      }
      else
      {
        log.warning() << "Skipping: "
                      << "Thread: " << s_it->source.thread_nr
                      << "\tWrite: " << s_it->source.pc->location_number
                      << messaget::eom;
      }
    }
  }
}