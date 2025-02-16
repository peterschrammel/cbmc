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
      symex_target_equationt::SSA_stepst::const_iterator>> &reads,
    std::unordered_map<
      unsigned,
      std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
    message_handlert &message_handler);

  void create_write_constraints(
    symex_target_equationt &equation,
    const std::unordered_map<
      unsigned,
      std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
    message_handlert &message_handler);

  void create_read_constraints(
    symex_target_equationt &equation,
    const std::vector<std::pair<
      symex_target_equationt::SSA_stepst::const_iterator,
      symex_target_equationt::SSA_stepst::const_iterator>> &reads,
    message_handlert &message_handler);
};

#endif //CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H
