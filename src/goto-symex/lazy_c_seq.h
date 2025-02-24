/// \file
/// LazyCSeq context-bounded concurrency SSA transformation

#ifndef CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H
#define CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H

#include "symex_target_equation.h"

class lazy_c_seqt
{
public:
  explicit lazy_c_seqt(const namespacet &ns, const std::size_t rounds)
    : ns(ns), rounds(rounds)
  {
  }

  void operator()(symex_target_equationt &, message_handlert &);

private:
  const namespacet &ns;
  const std::size_t rounds;

  void collect_reads_and_writes(
    const symex_target_equationt::SSA_stepst &ssa_steps,
    std::vector<std::pair<
      symex_target_equationt::SSA_stepst::const_iterator,
      std::optional<symex_target_equationt::SSA_stepst::const_iterator>>>
      &reads,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator> &main_reads,
    std::unordered_map<
      unsigned,
      std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
    message_handlert &message_handler);

  void create_write_constraints(
    symex_target_equationt &equation,
    const std::unordered_map<
      unsigned,
      std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
    std::unordered_map<
      irep_idt,
      symex_target_equationt::SSA_stepst::const_iterator> &last_update,
    std::unordered_map<irep_idt, irep_idt> &last_update_main,
    message_handlert &message_handler);

  void create_read_constraints(
    symex_target_equationt &equation,
    const std::vector<std::pair<
      symex_target_equationt::SSA_stepst::const_iterator,
      std::optional<symex_target_equationt::SSA_stepst::const_iterator>>>
      &reads,
    std::unordered_map<
      irep_idt,
      symex_target_equationt::SSA_stepst::const_iterator> &last_update,
    std::unordered_map<irep_idt, irep_idt> &last_update_main,
    message_handlert &message_handler);

  void create_main_read_constraints(
    symex_target_equationt &equation,
    std::unordered_map<
      irep_idt,
      symex_target_equationt::SSA_stepst::const_iterator> &last_update,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator> &main_reads,
    message_handlert &message_handler);

  void create_reach_constraint(
    symex_target_equationt &equation,
    std::vector<std::pair<
      symex_target_equationt::SSA_stepst::const_iterator,
      std::optional<symex_target_equationt::SSA_stepst::const_iterator>>>
      &reads,
    std::unordered_map<
      unsigned,
      std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
    message_handlert &message_handler);

  void handling_guards(
    symex_target_equationt &equation,
    message_handlert &message_handler);
};

#endif //CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H
